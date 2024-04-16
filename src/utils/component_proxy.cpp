#include "utils/component_proxy.hpp"
#include <iostream>
#include <mutex>
#include <string>
#include <variant>
#include <vector>
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "task_runner/runner.hpp"
#include "sol/sol.hpp"
#include "utils/dbg.hpp"
#include "utils/mutex_wrapper.hpp"
#include "utils/types.hpp"
namespace Engine {

void ComponentProxy::load_to_proxy(lua_ref_raw, ProxyData &pd) {

//     sol::type tp = obj.get_type();
//     if (tp == sol::type::number) {
//         if (obj.is<int>()) {
//             const int d = obj.as<int>();
//             return sol::make_object(target, d);
//         } else {
//             const double d = obj.as<double>();
//             return sol::make_object(target, d);
//         }
//     } else if (tp == sol::type::boolean) {
//         const bool b = obj.as<bool>();
//         return sol::make_object(target, b);
//     } else if (tp == sol::type::string) {
//         const std::string s = obj.as<std::string>();
//         return sol::make_object(target, s);
//     } else if (tp == sol::type::userdata) {
//         if (obj.is<ComponentProxy>()) {
//             return sol::make_object(target, obj.as<ComponentProxy>());
//         }
//         std::cerr << "warning: userdata type not registered\n";
//     } else if (tp == sol::type::table) {
//         sol::table t     = obj.as<sol::table>();
//         sol::table tcopy = target.create_table();
//         for (auto it = t.begin(); it != t.end(); ++it) {
//             auto [key, val] = *it;
//             tcopy.set(copy(key, target), copy(val, target));
//         }
//         return tcopy;
//     }
}

lua_ref_raw ComponentProxy::save_proxy_data(sol::state &state, ProxyData &pd) {
    if (std::holds_alternative<int>(pd.data)) {
        return sol::make_object(state, std::get<int>(pd.data));
    } else if (std::holds_alternative<double>(pd.data)) {
        return sol::make_object(state, std::get<double>(pd.data));
    } else if (std::holds_alternative<bool>(pd.data)) {
        return sol::make_object(state, std::get<bool>(pd.data));
    } else if (std::holds_alternative<std::string>(pd.data)) {
        return sol::make_object(state, std::get<std::string>(pd.data));
    } else if (std::holds_alternative<ComponentProxy>(pd.data)) {
        return sol::make_object(state, std::get<ComponentProxy>(pd.data));
    } else if (std::holds_alternative<std::unordered_map<table_key_t, ProxyData>>(pd.data)) {
        sol::table t = state.create_table();
        for (auto &[key, val] : std::get<std::unordered_map<table_key_t, ProxyData>>(pd.data)) {
            if (std::holds_alternative<std::string>(key)) {
                t.set(std::get<std::string>(key), save_proxy_data(state, val));
            } else if (std::holds_alternative<int>(key)) {
                t.set(std::get<int>(key), save_proxy_data(state, val));
            }
        }
        return t;
    }
    return {};
}


void ComponentProxy::get_table_keys(sol::table &t, std::vector<std::string> &p) {
    for (auto it = t.begin(); it != t.end(); ++it) {
        std::string key = (*it).first.as<std::string>();
        p.push_back(key);
    }
}

void ComponentProxy::register_usertype(sol::usertype<ComponentProxy>  &usertype,
                                       const std::vector<std::string> &keys) {
    for (auto &key : keys) {
        usertype[key] =
            sol::property([key](ComponentProxy &tb) { return tb.getter(key); },
                          [key](ComponentProxy &tb, sol::object obj) { tb.setter(key, obj); });
    }
}

lua_ref_raw ComponentProxy::getter(const std::string &name) {
    // Get component[name]
    LuaRunner &runner = TaskManager::get_lua_runner();
    if (component->lua_vm_id != runner.runner_id) {
        // Acquire lock only if the component is not running on the current VM
        ProxyData pd;
        runner.mtx.get().unlock();
        {
            std::unique_lock<std::mutex> lock(
                TaskManager::get_lua_runner(component->lua_vm_id).mtx.get());
            load_to_proxy(component->ref_tbl.value(), pd);
        }
        runner.mtx.get().lock();
        // Copy the value to the current VM
        return save_proxy_data(runner.state, pd);
    } else {
        return component->ref_tbl.value()[name];
    }
}

void ComponentProxy::setter(const std::string &name, lua_ref_raw obj) {
    LuaRunner &runner = TaskManager::get_lua_runner();
    if (component->lua_vm_id != runner.runner_id) {
        // Acquire lock only if the component is not running on the current VM
        opt_lock.emplace(TaskManager::get_lua_runner(component->lua_vm_id).mtx.get());
        // Copy the value to the component's VM
        component->ref_tbl.value()[name] =
            copy(obj, TaskManager::get_lua_runner(component->lua_vm_id).state);
    } else {
        component->ref_tbl.value()[name] = obj;
    }
}

}  // namespace Engine
