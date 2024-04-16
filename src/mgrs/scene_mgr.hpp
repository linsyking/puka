#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>
#include "mgrs/actor.hpp"
#include "mgrs/actor_template_mgr.hpp"
#include "utils/camera.hpp"
#include "utils/types.hpp"
#include "yyjson.h"

namespace Engine {
struct Scene {
    std::vector<actor_ref> actors;
    std::vector<actor_ref> running_actors;
    std::vector<actor_ref> removed_actors;
};

class SceneManager {
private:
    size_t               id_counter   = 0;
    size_t               uuid_counter = 0;
    Scene                current_scene;
    std::string          current_scene_name;
    std::string          next_scene_name;
    Camera               camera;
    ActorTemplateManager actor_template_manager;
    bool                 load_next = false;

    void load_document_to_scene(yyjson_val *doc);
    void cleanup();

public:
    /// Update to_add_actors and to_remove_actors to actors
    void update_unhandled_actors();

    SceneManager() {}
    std::shared_mutex mtx;
    void              init();
    size_t            uuid() { return uuid_counter++; }
    void              load_scene(const std::string &scene_name);
    std::string       get_current_scene_name() { return current_scene_name; }
    Scene            &get_current_scene() { return current_scene; }
    void              set_next_scene(const std::string &scene_name) {
        next_scene_name = scene_name;
        load_next       = true;
    }
    void               load_next_scene_if_need();
    Camera            &get_camera() { return camera; }
    static lua_ref_raw instantiate_actor(const std::string &name);
    static void        destroy_actor(Actor *actor);
};

// Lua bindings

void        load_scene(const std::string &scene_name);
std::string get_current_scene_name();
void        dont_destroy(Actor *actor);

}  // namespace Engine
