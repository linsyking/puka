#include "mgrs/lua_runner.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include "consts.hpp"
#include "game.hpp"
#include "mgrs/task_mgr.hpp"
#include "utils/component_proxy.hpp"
namespace Engine {

void LuaRunner::init_vm() {
    TaskManager::init_state(state);
}

bool LuaRunner::load_component_proto(const std::string &type) {
    if (component_types.find(type) != component_types.end()) {
        return true;
    }
    std::string file_path = lua_script_folder + "/" + type + ".lua";
    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        std::cout << "error: failed to locate component " << type;
        game().terminate();
        return false;
    }
    try {
        component_types[type] = state.script_file(file_path);
        return true;
    } catch (std::exception &e) {
        std::cout << "problem with lua file " << type;
        std::cout << e.what() << "\n";
        game().terminate();
        return false;
    }
}

lua_component_ref LuaRunner::create_component(const std::string &type) {
    // Create a lua component with given type
    bool res = false;
    if (component_types.find(type) == component_types.end()) {
        res = load_component_proto(type);
    }
    if (!res) {
        // Faild to load proto, exit
        return nullptr;
    }
    sol::table        instance = ComponentProxy::copy(component_types[type].value(), state);
    lua_component_ref comp     = std::make_shared<LuaComponent>(runner_id);
    comp->ref_tbl              = instance;
    comp->type                 = type;
    return comp;
}

void LuaRunner::quit() {
    component_types.clear();
}

}  // namespace Engine
