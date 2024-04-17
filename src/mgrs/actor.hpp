#pragma once
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include "utils/component.hpp"
#include "utils/json.hpp"
#include "utils/mutex_wrapper.hpp"
#include "utils/types.hpp"

namespace Engine {

class Actor;

using actor_ref = std::shared_ptr<Actor>;

class Actor {
private:
    std::string name;
    lua_ref_raw get_component_ref(component_ref &);

public:
    Actor() {}
    size_t index_in_scene() const { return idx; }

    void        load_from_value_pure(yyjson_val *a);
    void        load_from_value(yyjson_val *a);
    void        load_from_value_inner();
    std::string get_name() const { return name; }
    /// IDX is unique within a scene
    size_t idx;

    /// UUID is unique among all scenes
    size_t uuid;

    /// Set false to indicate not destroying this actor when switching scenes
    bool destroyable = true;

    /// Component mapping
    std::unordered_map<std::string, jmap>          component_attr_map;
    std::unordered_map<std::string, component_ref> component_map;
    std::vector<component_ref>                     components;

    std::queue<component_ref> component_to_add;
    std::queue<component_ref> component_to_remove;

    std::vector<component_ref> update_components;
    std::vector<component_ref> update_late_components;

    static std::vector<size_t> order_actors_rendering(const std::vector<actor_ref> &actors);
    void                       init_components();
    void                       init_component(component_ref &);

    /// Add components_to_add to components
    /// and remove components_to_remove from components
    void update_unhandled_components();

    /// Add a component to the actor by key
    static void component_insert(std::vector<component_ref> &components, component_ref &c);
    static void cleanup_actors(std::vector<actor_ref> &);

    void call_collision_enter(Collision &collision);
    void call_collision_exit(Collision &collision);

    void call_trigger_enter(Collision &collision);
    void call_trigger_exit(Collision &collision);

    void on_destroy();

    shared_mutex mtx;

    /// Extra Lua bindings

    size_t      get_id() const { return idx; }
    lua_ref_raw get_component_by_key(const std::string &);
    lua_ref_raw get_component_by_type(const std::string &);
    lua_ref_raw get_components_by_type(const std::string &);

    lua_ref_raw add_component(const std::string &type);
    void        remove_component(lua_ref_raw comp);
};

// Find other actors
lua_ref_raw find_actor(const std::string &);
lua_ref_raw find_actors(const std::string &);

}  // namespace Engine
