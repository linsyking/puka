#include "utils/component_proxy.hpp"
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include "mgrs/actor.hpp"
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "sol/sol.hpp"
#include "utils/mutex_wrapper.hpp"
#include "utils/types.hpp"
namespace Engine {

void set_value(lua_ref_raw &dest, lua_ref &dest_key, lua_ref_raw &&v) {
    if (dest_key.has_value()) {
        dest.as<sol::table>().set(dest_key.value(), v);
    } else {
        dest = v;
    }
}

lua_ref_raw ComponentProxy::copy(lua_ref_raw &obj, sol::state &target) {
    sol::type tp = obj.get_type();
    if (tp == sol::type::number) {
        if (obj.is<int>()) {
            return sol::make_object(target, obj.as<int>());
        } else {
            return sol::make_object(target, obj.as<double>());
        }
    } else if (tp == sol::type::boolean) {
        return sol::make_object(target, obj.as<bool>());
    } else if (tp == sol::type::string) {
        return sol::make_object(target, obj.as<std::string>());
    } else if (tp == sol::type::userdata) {
        if (obj.is<ComponentProxy>()) {
            return sol::make_object(target, obj.as<ComponentProxy>());
        } else if (obj.is<Actor *>()) {
            return sol::make_object(target, obj.as<Actor *>());
        } else {
            std::cerr << "warning: userdata type not registered\n";
        }
    } else if (tp == sol::type::function) {
        return sol::make_object(target, obj.as<sol::function>());
    } else if (tp == sol::type::table) {
        sol::table t     = obj.as<sol::table>();
        sol::table tcopy = target.create_table();
        for (auto it = t.begin(); it != t.end(); ++it) {
            auto [key, val] = *it;
            tcopy.set(copy(key, target), copy(val, target));
        }
        return tcopy;
    }
    return {};
}

lua_ref_raw ComponentProxy::get() {
    if (trivial) {
        return component->ref_tbl.value();
    }
    if (!ref.has_value()) {
        // Copy from component VM to tmp VM
        LuaRunner &runner      = TaskManager::get_lua_runner();
        LuaRunner &comp_runner = TaskManager::get_lua_runner(component->lua_vm_id);
        LuaRunner &tmp_runner  = TaskManager::get().tmp_runner;
        runner.mtx.get().unlock();
        lua_ref tmp;
        {
            std::unique_lock<std::mutex> lock(comp_runner.mtx.get());
            std::unique_lock<std::mutex> tmp_lock(tmp_runner.mtx.get());
            tmp = copy(component->ref_tbl.value(), tmp_runner.state);
        }
        runner.mtx.get().lock();
        // Copy from tmp VM to ref
        std::unique_lock<std::mutex> tmp_lock(tmp_runner.mtx.get());
        ref = copy(tmp.value(), runner.state);
    }
    return ref.value();
}

void ComponentProxy::wb() {
    if (trivial || !ref.has_value()) {
        return;
    }
    LuaRunner &runner      = TaskManager::get_lua_runner();
    LuaRunner &comp_runner = TaskManager::get_lua_runner(component->lua_vm_id);
    LuaRunner &tmp_runner  = TaskManager::get().tmp_runner;
    lua_ref    tmp;
    // Copy from ref to tmp VM
    {
        std::unique_lock<std::mutex> tmp_lock(tmp_runner.mtx.get());
        tmp = copy(ref.value(), tmp_runner.state);
    }
    // Copy from tmp to component VM
    runner.mtx.get().unlock();
    {
        std::unique_lock<std::mutex> lock(comp_runner.mtx.get());
        std::unique_lock<std::mutex> tmp_lock(tmp_runner.mtx.get());
        component->ref_tbl = copy(tmp.value(), comp_runner.state);
    }
    runner.mtx.get().lock();
}

}  // namespace Engine
