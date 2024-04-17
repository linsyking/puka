#include "utils/component_proxy.hpp"
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include "mgrs/actor.hpp"
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "sol/sol.hpp"
#include "utils/dbg.hpp"
#include "utils/mutex_wrapper.hpp"
#include "utils/types.hpp"
namespace Engine {

void ComponentProxy::copy(lua_ref_raw &obj, sol::state &target, lua_ref_raw &dest) {
    sol::type tp = obj.get_type();
    if (tp == sol::type::number) {
        if (obj.is<int>()) {
            const int d = obj.as<int>();
            dest        = sol::make_object(target, d);
        } else {
            const double d = obj.as<double>();
            dest           = sol::make_object(target, d);
        }
    } else if (tp == sol::type::boolean) {
        const bool b = obj.as<bool>();
        dest         = sol::make_object(target, b);
    } else if (tp == sol::type::string) {
        const std::string s = obj.as<std::string>();
        dest                = sol::make_object(target, s);
    } else if (tp == sol::type::userdata) {
        if (obj.is<ComponentProxy>()) {
            dest = sol::make_object(target, obj.as<ComponentProxy>());
        } else if (obj.is<Actor *>()) {
            dest = sol::make_object(target, obj.as<Actor *>());
        } else {
            std::cerr << "warning: userdata type not registered\n";
        }
    } else if (tp == sol::type::table) {
        sol::table t        = obj.as<sol::table>();
        sol::table dest_tbl = dest.as<sol::table>();
        for (auto it = t.begin(); it != t.end(); ++it) {
            auto [key, val]        = *it;
            lua_ref_raw key_target = sol::lua_nil;
            copy(key, target, key_target);
            lua_ref_raw val_target = dest_tbl[key_target];
            if (val_target.valid()) {
                copy(val, target, val_target);
            } else {
                lua_ref_raw val_target = sol::lua_nil;
                copy(val, target, val_target);
                dest_tbl.set(key_target, val_target);
            }
        }
    }
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
            tmp = tmp_runner.state.create_table();
            copy(component->ref_tbl.value(), tmp_runner.state, tmp.value());
        }
        runner.mtx.get().lock();
        // Copy from tmp VM to ref
        std::unique_lock<std::mutex> tmp_lock(tmp_runner.mtx.get());
        ref = runner.state.create_table();
        copy(tmp.value(), runner.state, ref.value());
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
        tmp = tmp_runner.state.create_table();
        copy(ref.value(), tmp_runner.state, tmp.value());
    }
    runner.mtx.get().unlock();
    {
        // Copy from tmp to component VM
        std::unique_lock<std::mutex> lock(comp_runner.mtx.get());
        copy(tmp.value(), comp_runner.state, component->ref_tbl.value());
    }
    runner.mtx.get().lock();
}

}  // namespace Engine
