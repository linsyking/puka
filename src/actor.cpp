#include "actor.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <mutex>
#include <numeric>
#include <shared_mutex>
#include <string>
#include <vector>
#include "components/rigidbody.hpp"
#include "game.hpp"
#include "mgrs/lua_runner.hpp"
#include "mgrs/task_mgr.hpp"
#include "sol/sol.hpp"
#include "utils/builtin_component.hpp"
#include "utils/component.hpp"
#include "utils/component_proxy.hpp"
#include "utils/dbg.hpp"
#include "utils/json.hpp"
#include "utils/types.hpp"
#include "yyjson/yyjson.h"

namespace Engine {

void Actor::load_from_value_pure(yyjson_val *a) {
    EngineUtils::ignore_set(name, a, "name");
    if (yyjson_val *res = yyjson_obj_get(a, "components")) {
        // Iterate over res
        yyjson_val     *ival = nullptr, *ikey = nullptr;
        yyjson_obj_iter iter = yyjson_obj_iter_with(res);

        while ((ikey = yyjson_obj_iter_next(&iter))) {
            ival            = yyjson_obj_iter_get_val(ikey);
            std::string key = yyjson_get_str(ikey);
            jmap        m;
            if (component_attr_map.find(key) != component_attr_map.end()) {
                m = component_attr_map[key];
            }
            yyjson_val     *ival2 = nullptr, *ikey2 = nullptr;
            yyjson_obj_iter iter2 = yyjson_obj_iter_with(ival);
            while ((ikey2 = yyjson_obj_iter_next(&iter2))) {
                ival2           = yyjson_obj_iter_get_val(ikey2);
                std::string key = yyjson_get_str(ikey2);
                jvalue      v;
                if (yyjson_is_str(ival2)) {
                    v.string_v = yyjson_get_str(ival2);
                    v.type_v   = jvalue::type::STRING;
                } else if (yyjson_is_int(ival2)) {
                    v.int_v  = yyjson_get_int(ival2);
                    v.type_v = jvalue::type::INT;
                } else if (yyjson_is_real(ival2)) {
                    v.float_v = yyjson_get_real(ival2);
                    v.type_v  = jvalue::type::NUMBER;
                } else if (yyjson_is_bool(ival2)) {
                    v.bool_v = yyjson_get_bool(ival2);
                    v.type_v = jvalue::type::BOOL;
                }
                m[key] = v;
            }
            component_attr_map[key] = m;
        }
    }
}

void Actor::component_insert(std::vector<component_ref> &components, component_ref &c) {
    // Assume components are sorted by key
    auto it = std::lower_bound(
        components.begin(), components.end(), c,
        [](const component_ref &a, const component_ref &b) -> bool { return a->key < b->key; });
    components.insert(it, c);
}

void Actor::load_from_value_inner() {
    // Load components
    for (auto &c : component_attr_map) {
        std::string key = c.first;
        DBGOUT("Creating component " << key);
        component_ref component =
            game().get_component_manager().create_component(c.second["type"].string_v);
        if (!component) {
            DBGOUT("Component creation failed");
            return;
        }
        // Initialization, no need to lock
        component->key        = key;
        component->actor_name = name;
        component_map[key]    = component;

        // Inject attributes
        if (auto rb = std::dynamic_pointer_cast<RigidbodyComponent>(component)) {
            Rigidbody_Def def;
            for (auto &e : c.second) {
                if (e.first == "x") {
                    rb->x = e.second.get_num();
                } else if (e.first == "y") {
                    rb->y = e.second.get_num();
                } else if (e.first == "body_type") {
                    def.body_type = e.second.string_v;
                } else if (e.first == "precise") {
                    def.precise = e.second.bool_v;
                } else if (e.first == "gravity_scale") {
                    rb->gravity_scale = e.second.get_num();
                } else if (e.first == "density") {
                    rb->density = e.second.get_num();
                } else if (e.first == "angular_friction") {
                    rb->angular_friction = e.second.get_num();
                } else if (e.first == "rotation") {
                    rb->rotation = e.second.get_num();
                } else if (e.first == "has_collider") {
                    def.has_collider = e.second.bool_v;
                } else if (e.first == "has_trigger") {
                    def.has_trigger = e.second.bool_v;
                } else if (e.first == "width") {
                    def.width = e.second.get_num();
                } else if (e.first == "height") {
                    def.height = e.second.get_num();
                } else if (e.first == "radius") {
                    def.radius = e.second.get_num();
                } else if (e.first == "friction") {
                    def.friction = e.second.get_num();
                } else if (e.first == "bounciness") {
                    def.bounciness = e.second.get_num();
                } else if (e.first == "collider_type") {
                    def.collider_type = e.second.string_v;
                } else if (e.first == "trigger_type") {
                    def.trigger_type = e.second.string_v;
                } else if (e.first == "trigger_width") {
                    def.trigger_width = e.second.get_num();
                } else if (e.first == "trigger_height") {
                    def.trigger_height = e.second.get_num();
                } else if (e.first == "trigger_radius") {
                    def.trigger_radius = e.second.get_num();
                }
            }
            rb->actor = this;
            rb->set_def(def);
        }
        if (lua_component_ref lc = std::dynamic_pointer_cast<LuaComponent>(component)) {
            for (auto &e : c.second) {
                switch (e.second.type_v) {
                    case jvalue::type::STRING:
                        lc->ref_tbl.value()[e.first.c_str()] = e.second.string_v;
                        break;
                    case jvalue::type::INT:
                        lc->ref_tbl.value()[e.first.c_str()] = e.second.int_v;
                        break;
                    case jvalue::type::NUMBER:
                        lc->ref_tbl.value()[e.first.c_str()] = e.second.float_v;
                        break;
                    case jvalue::type::BOOL:
                        lc->ref_tbl.value()[e.first.c_str()] = e.second.bool_v;
                        break;
                }
            }
            component->set_actor(this);
        }
        components.push_back(component);
    }
    // Sort components by key
    std::sort(
        components.begin(), components.end(),
        [](const component_ref &a, const component_ref &b) -> bool { return a->key < b->key; });
}

void Actor::load_from_value(yyjson_val *a) {
    load_from_value_pure(a);
    load_from_value_inner();
}

std::vector<size_t> Actor::order_actors_rendering(const std::vector<actor_ref> &actors) {
    std::vector<size_t> ordered_actors(actors.size());
    std::iota(ordered_actors.begin(), ordered_actors.end(), 0);
    std::sort(ordered_actors.begin(), ordered_actors.end(),
              [&](const size_t aid, const size_t bid) -> bool {
                  const actor_ref &a = actors[aid];
                  const actor_ref &b = actors[bid];
                  return a->index_in_scene() < b->index_in_scene();
              });
    return ordered_actors;
}

void Actor::init_component(component_ref &c) {
    if (c->has_update) {
        update_components.push_back(c);
    }
    if (c->has_late_update) {
        update_late_components.push_back(c);
    }
    if (c->has_start) {
        game().get_task_manager().get_on_start_components().push(c);
    }
}

void Actor::init_components() {
    for (auto &c : components) {
        c->init();
        init_component(c);
    }
}

void Actor::cleanup_actors(std::vector<actor_ref> &actors) {
    // Erase all actors with destroyable = true
    for (auto &a : actors) {
        if (a->destroyable) {
            a->on_destroy();
        }
    }
    auto it = std::remove_if(actors.begin(), actors.end(),
                             [](const actor_ref &a) -> bool { return a->destroyable; });
    actors.erase(it, actors.end());
}

lua_ref_raw Actor::get_component_ref(component_ref &comp) {
    if (lua_component_ref lc = std::dynamic_pointer_cast<LuaComponent>(comp)) {
        LuaRunner &runner = TaskManager::get_lua_runner();
        return sol::make_object(runner.state,
                                ComponentProxy(lc.get(), runner.runner_id == lc->lua_vm_id));
    }
    if (std::dynamic_pointer_cast<BuiltinComponent>(comp)) {
        return sol::make_object(TaskManager::get_lua_runner().state, comp.get());
    }
    return {};
}

lua_ref_raw Actor::get_component_by_key(const std::string &key) {
    std::shared_lock<std::shared_mutex> lock(mtx.get());
    if (component_map.find(key) != component_map.end()) {
        return get_component_ref(component_map[key]);
    }
    return {};
}

lua_ref_raw Actor::get_component_by_type(const std::string &type) {
    std::shared_lock<std::shared_mutex> lock(mtx.get());
    // Search for the first component of the given type
    for (auto &c : components) {
        if (c->type == type) {
            return get_component_ref(c);
        }
    }
    return {};
}

lua_ref_raw Actor::get_components_by_type(const std::string &type) {
    std::shared_lock<std::shared_mutex> lock(mtx.get());
    // Return an indexed table with all components of the given type

    sol::table table = TaskManager::get_lua_runner().state.create_table();
    for (auto &c : components) {
        if (c->type == type) {
            table.add(get_component_ref(c));
        }
    }
    return table;
}

lua_ref_raw find_actor(const std::string &name) {
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        std::shared_lock<std::shared_mutex> lock(main_scene->get_scene_manager().mtx);
        for (auto &actor : main_scene->running_actors()) {
            if (actor->get_name() == name) {
                return sol::make_object(TaskManager::get_lua_runner().state, actor.get());
            }
        }
    }
    return {};
}

lua_ref_raw find_actors(const std::string &name) {
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        std::shared_lock<std::shared_mutex> lock(main_scene->get_scene_manager().mtx);

        sol::table table = TaskManager::get_lua_runner().state.create_table();
        for (auto &actor : main_scene->running_actors()) {
            if (actor->get_name() == name) {
                table.add(sol::make_object(TaskManager::get_lua_runner().state, actor.get()));
            }
        }
        return table;
    }
    return {};
}

lua_ref_raw Actor::add_component(const std::string &type) {
    // TODO
    static size_t idx       = 0;
    component_ref component = game().get_component_manager().create_component(type);
    if (!component) {
        return {};
    }
    component->actor_name = name;
    std::string key       = "r" + std::to_string(idx++);
    component->key        = key;
    component_map[key]    = component;
    component->set_actor(this);
    {
        std::unique_lock<std::shared_mutex> lock(mtx.get());
        component_to_add.push(component);
    }
    component->init();
    return get_component_ref(component);
}

void Actor::update_unhandled_components() {
    std::unique_lock<std::shared_mutex> lock(mtx.get());
    while (!component_to_add.empty()) {
        init_component(component_to_add.front());
        component_insert(components, component_to_add.front());
        component_to_add.pop();
    }
    while (!component_to_remove.empty()) {
        auto it = std::remove_if(
            components.begin(), components.end(),
            [&](const component_ref &c) -> bool { return c == component_to_remove.front(); });
        components.erase(it, components.end());
        it = std::remove_if(
            update_components.begin(), update_components.end(),
            [&](const component_ref &c) -> bool { return c == component_to_remove.front(); });
        update_components.erase(it, update_components.end());
        it = std::remove_if(
            update_late_components.begin(), update_late_components.end(),
            [&](const component_ref &c) -> bool { return c == component_to_remove.front(); });
        update_late_components.erase(it, update_late_components.end());
        component_to_remove.front()->on_destroy();
        component_to_remove.pop();
    }
}

void Actor::remove_component(lua_ref_raw comp) {
    std::string key;
    if (comp.is<RigidbodyComponent *>()) {
        RigidbodyComponent *rb = comp.as<RigidbodyComponent *>();
        key                    = rb->key;
    } else if (comp.is<ComponentProxy>()) {
        key = comp.as<ComponentProxy>().component->key;
    } else {
        DBGOUT("warning: invalid component")
        return;
    }
    std::unique_lock<std::shared_mutex> lock(mtx.get());
    if (component_map.find(key) != component_map.end()) {
        component_ref component = component_map[key];
        component->set_enabled(false);
        component->die();
        component_map.erase(key);
        component_to_remove.push(component);
    }
}

void Actor::call_collision_enter(Collision &collision) {
    for (auto &c : components) {
        c->on_collision_enter(collision);
    }
}

void Actor::call_collision_exit(Collision &collision) {
    for (auto &c : components) {
        c->on_collision_exit(collision);
    }
}

void Actor::call_trigger_enter(Collision &collision) {
    for (auto &c : components) {
        c->on_trigger_enter(collision);
    }
}

void Actor::call_trigger_exit(Collision &collision) {
    for (auto &c : components) {
        c->on_trigger_exit(collision);
    }
}

void Actor::on_destroy() {
    for (auto &c : components) {
        c->on_destroy();
    }
}

}  // namespace Engine
