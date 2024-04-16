#include "lua_component.hpp"
#include "mgrs/actor.hpp"
#include "mgrs/lua_mgr.hpp"
#include "mgrs/task_mgr.hpp"
#include "utils/types.hpp"
namespace Engine {

LuaComponent::LuaComponent(size_t id) {
    lua_vm_id        = id;
    onstart_task     = std::make_shared<lua_onstart_task>(this);
    update_task      = std::make_shared<lua_update_task>(this);
    late_update_task = std::make_shared<lua_late_update_task>(this);
}

void LuaComponent::init() {
    if (ref_tbl.has_value()) {
        // Test if the component has a start method
        lua_ref_raw start = ref_tbl.value().as<sol::table>()["OnStart"];
        has_start         = start.is<sol::function>();

        lua_ref_raw updater = ref_tbl.value().as<sol::table>()["OnUpdate"];
        has_update          = updater.is<sol::function>();

        lua_ref_raw late_updater = ref_tbl.value().as<sol::table>()["OnLateUpdate"];
        has_late_update          = late_updater.is<sol::function>();

        lua_ref_raw collision_enter = ref_tbl.value().as<sol::table>()["OnCollisionEnter"];
        has_collision_enter         = collision_enter.is<sol::function>();

        lua_ref_raw collision_exit = ref_tbl.value().as<sol::table>()["OnCollisionExit"];
        has_collision_exit         = collision_exit.is<sol::function>();

        lua_ref_raw trigger_enter = ref_tbl.value().as<sol::table>()["OnTriggerEnter"];
        has_trigger_enter         = trigger_enter.is<sol::function>();

        lua_ref_raw trigger_exit = ref_tbl.value().as<sol::table>()["OnTriggerExit"];
        has_trigger_exit         = trigger_exit.is<sol::function>();

        lua_ref_raw destroy = ref_tbl.value().as<sol::table>()["OnDestroy"];
        has_destroy         = destroy.is<sol::function>();

        // Inject key attribute
        ref_tbl.value().as<sol::table>()["key"]     = key;
        ref_tbl.value().as<sol::table>()["enabled"] = true;
    }
}

LuaComponent::~LuaComponent() {
    ref_tbl->reset();
}

bool LuaComponent::is_enabled() {
    return ref_tbl.value().as<sol::table>()["enabled"];
}

void LuaComponent::on_start() {
    if (!dead && has_start && is_enabled()) {
        execute_component_function(this, "OnStart");
    }
}

void LuaComponent::update() {
    if (!dead && has_update && is_enabled()) {
        execute_component_function(this, "OnUpdate");
    }
}

void LuaComponent::late_update() {
    if (!dead && has_late_update && is_enabled()) {
        execute_component_function(this, "OnLateUpdate");
    }
}

void LuaComponent::set_enabled(bool enabled) {
    if (!dead) {
        ref_tbl.value().as<sol::table>()["enabled"] = enabled;
    }
}

void LuaComponent::on_collision_enter(Collision &collision) {
    if (!dead && has_collision_enter && is_enabled()) {
        execute_component_function_col(this, "OnCollisionEnter", collision);
    }
}

void LuaComponent::on_collision_exit(Collision &collision) {
    if (!dead && has_collision_exit && is_enabled()) {
        execute_component_function_col(this, "OnCollisionExit", collision);
    }
}

void LuaComponent::on_trigger_enter(Collision &collision) {
    if (!dead && has_trigger_enter && is_enabled()) {
        execute_component_function_col(this, "OnTriggerEnter", collision);
    }
}

void LuaComponent::on_trigger_exit(Collision &collision) {
    if (!dead && has_trigger_exit && is_enabled()) {
        execute_component_function_col(this, "OnTriggerExit", collision);
    }
}

void LuaComponent::set_actor(Actor *actor) {
    ref_tbl.value().as<sol::table>()["actor"] = actor;
}

void LuaComponent::on_destroy() {
    if (has_destroy) {
        execute_component_function(this, "OnDestroy");
    }
    ref_tbl = {};
}

}  // namespace Engine
