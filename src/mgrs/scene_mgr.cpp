#include "scene_mgr.hpp"
#include <filesystem>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>
#include "mgrs/actor.hpp"
#include "consts.hpp"
#include "game.hpp"
#include "mgrs/task_mgr.hpp"
#include "scenes/main_scene.hpp"
#include "sol/sol.hpp"
#include "utils/json.hpp"
#include "yyjson/yyjson.h"

namespace Engine {

void SceneManager::init() {
    // Initialize the scene manager
    // Only run once, NOT running every scene
}

void SceneManager::load_document_to_scene(yyjson_val *root) {
    yyjson_val *actors = yyjson_obj_get(root, "actors");
    if (!actors) {
        std::cout << "error: scene " << current_scene_name << " is missing actors";
        exit(0);
    }
    yyjson_val     *val  = nullptr;
    yyjson_arr_iter iter = yyjson_arr_iter_with(actors);
    while ((val = yyjson_arr_iter_next(&iter))) {
        actor_ref actor;
        if (yyjson_val *tmpl = yyjson_obj_get(val, "template")) {
            // Use template to load actor
            std::string template_name = yyjson_get_str(tmpl);
            actor =
                std::make_shared<Actor>(actor_template_manager.get_actor_template(template_name));
        } else {
            actor = std::make_shared<Actor>();
        }
        actor->load_from_value(val);
        actor->idx  = id_counter++;
        actor->uuid = uuid();
        actor->init_components();
        current_scene.actors.push_back(actor);
        current_scene.running_actors.push_back(actor);
    }
}

void SceneManager::load_scene(const std::string &scene_name) {
    // Scene initialization
    current_scene_name     = scene_name;
    std::string scene_path = scene_folder + "/" + scene_name + ".scene";
    if (!std::filesystem::exists(scene_path)) {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }
    yyjson_doc *doc = EngineUtils::read_json_from_file(scene_path);
    load_document_to_scene(yyjson_doc_get_root(doc));
    yyjson_doc_free(doc);
}

void SceneManager::cleanup() {
    // Clean up the current scene
    Actor::cleanup_actors(current_scene.actors);
    Actor::cleanup_actors(current_scene.running_actors);
    id_counter = 0;
}

void SceneManager::load_next_scene_if_need() {
    if (load_next) {
        cleanup();
        load_scene(next_scene_name);
        load_next = false;
    }
}

void SceneManager::update_unhandled_actors() {
    for (auto &actor : current_scene.removed_actors) {
        actor->on_destroy();
    }
    current_scene.removed_actors.clear();
    current_scene.actors = current_scene.running_actors;
}

lua_ref_raw SceneManager::instantiate_actor(const std::string &name) {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        std::unique_lock<std::shared_mutex> lock(main_scene->get_scene_manager().mtx);
        actor_ref                           actor = std::make_shared<Actor>(
            main_scene->get_scene_manager().actor_template_manager.get_actor_template(name));
        // Directly instantiate from template
        actor->load_from_value_inner();
        actor->idx  = main_scene->get_scene_manager().id_counter++;
        actor->uuid = main_scene->get_scene_manager().uuid();
        actor->init_components();
        main_scene->get_scene_manager().current_scene.running_actors.push_back(actor);

        std::unique_lock<std::mutex> lock2(TaskManager::get_lua_runner().mtx.get());
        return sol::make_object(TaskManager::get_lua_runner().state, actor.get());
    }
    return {};
}

void SceneManager::destroy_actor(Actor *actor) {
    // Disable all components
    for (auto &c : actor->components) {
        c->set_enabled(false);
    }
    actor_ref actor_r;
    Game     &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        std::unique_lock<std::shared_mutex> lock(main_scene->get_scene_manager().mtx);
        for (auto &a : main_scene->actors()) {
            if (a.get() == actor) {
                actor_r = a;
                break;
            }
        }
        Scene &current_scene = main_scene->get_scene_manager().current_scene;
        auto   it =
            std::remove_if(current_scene.running_actors.begin(), current_scene.running_actors.end(),
                           [&](const actor_ref &a) -> bool { return a == actor_r; });
        current_scene.removed_actors.push_back(actor_r);
        current_scene.running_actors.erase(it, current_scene.running_actors.end());
    }
}

void load_scene(const std::string &scene_name) {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        main_scene->get_scene_manager().set_next_scene(scene_name);
    }
}

std::string get_current_scene_name() {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        return main_scene->get_scene_manager().get_current_scene_name();
    }
    return "";
}

void dont_destroy(Actor *actor) {
    actor->destroyable = false;
}

}  // namespace Engine
