#include "utils/component_proxy.hpp"
#include <iostream>
#include <mutex>
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "task_runner/runner.hpp"
#include "sol/sol.hpp"
#include "utils/types.hpp"
namespace Engine {

lua_ref_raw ComponentProxy::copy(lua_ref_raw obj, sol::state &target) {
    sol::type tp = obj.get_type();
    if (tp == sol::type::number) {
        if (obj.is<int>()) {
            const int d = obj.as<int>();
            return sol::make_object(target, d);
        } else {
            const double d = obj.as<double>();
            return sol::make_object(target, d);
        }
    } else if (tp == sol::type::boolean) {
        const bool b = obj.as<bool>();
        return sol::make_object(target, b);
    } else if (tp == sol::type::string) {
        const std::string s = obj.as<std::string>();
        return sol::make_object(target, s);
    } else if (tp == sol::type::userdata) {
        if (obj.is<ComponentProxy>()) {
            return sol::make_object(target, obj.as<ComponentProxy>());
        }
        std::cerr << "warning: userdata type not registered\n";
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

lua_ref_raw ComponentProxy::getter(const char *name) {
    // Get component[name]
    std::optional<std::unique_lock<std::mutex>> opt_lock;
    if (component->lua_vm_id != runner::thread_num()) {
        // Acquire lock only if the component is not running on the current VM
        opt_lock.emplace(TaskManager::get_lua_runner(component->lua_vm_id).mtx.get());
    }
    // Copy the value to the current VM
    return copy(component->ref_tbl.value()[name], TaskManager::get_lua_runner().state);
}

void ComponentProxy::setter(const char *name, lua_ref_raw obj) {
    std::optional<std::unique_lock<std::mutex>> opt_lock;
    if (component->lua_vm_id != runner::thread_num()) {
        // Acquire lock only if the component is not running on the current VM
        opt_lock.emplace(TaskManager::get_lua_runner(component->lua_vm_id).mtx.get());
    }
    // Copy the value to the component's VM
    component->ref_tbl.value()[name] =
        copy(obj, TaskManager::get_lua_runner(component->lua_vm_id).state);
}

}  // namespace Engine
