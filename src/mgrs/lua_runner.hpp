#pragma once

#include "sol/sol.hpp"
#include "utils/component_proxy.hpp"
#include "utils/lua_component.hpp"
#include "utils/mutex_wrapper.hpp"
namespace Engine {

struct LuaRunner {
private:
    std::unordered_map<std::string, std::optional<sol::table>> component_types;
    void establish_inheritance(sol::table &instance_table, sol::table &parent_table);

public:
    size_t runner_id = 0;
    LuaRunner(size_t id) : runner_id(id) {}
    LuaRunner(const LuaRunner &other) : runner_id(other.runner_id) {}

    // Manage components

    void              load_component_proto(const std::string &name);
    lua_component_ref create_component(const std::string &type);

    /// Initialize the lua VM
    void init_vm();

    /// Quit the lua VM
    void quit();

    /// Lua VM
    sol::state state;

    sol::optional<sol::usertype<ComponentProxy>> component_proxy_type;

    /// Mutex for the lua VM
    ///
    /// Acquire the lock when accessing the VM.
    /// Cannot use mutex in a vector, so we use a unique_ptr
    unique_mutex mtx;
};
}  // namespace Engine
