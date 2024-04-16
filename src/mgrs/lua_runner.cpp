#include "mgrs/lua_runner.hpp"
#include <filesystem>
#include "consts.hpp"
#include "mgrs/task_mgr.hpp"
namespace Engine {

void LuaRunner::init_vm() {
    TaskManager::init_state(state);
}

void LuaRunner::load_component_proto(const std::string &type) {
    if (component_types.find(type) != component_types.end()) {
        return;
    }
    std::string file_path = lua_script_folder + "/" + type + ".lua";
    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        std::cout << "error: failed to locate component " << type;
        exit(0);
    }
    if (!state.do_file(file_path).valid()) {
        std::cout << "problem with lua file " << type;
        exit(0);
    }
    lua_ref_raw prototype = state[type];
    component_types[type] = prototype;
}

lua_component_ref LuaRunner::create_component(const std::string &type) {
    // Create a lua component with given type
    if (component_types.find(type) == component_types.end()) {
        load_component_proto(type);
    }
    sol::table instance = state.create_table();
    establish_inheritance(instance, component_types[type].value());
    lua_component_ref comp = std::make_shared<LuaComponent>();
    comp->ref_tbl          = instance;
    comp->type             = type;
    comp->lua_vm_id        = runner_id;
    return comp;
}

void LuaRunner::establish_inheritance(sol::table &instance_table, sol::table &parent_table) {
    sol::table metatable = instance_table[sol::metatable_key].get_or_create<sol::table>();
    metatable["__index"] = parent_table;
}

void LuaRunner::quit() {
    component_types.clear();
}

}  // namespace Engine
