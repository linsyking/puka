#include "main_scene.hpp"
#include "game.hpp"
#include "mgrs/actor.hpp"
#include "task_runner/runner.hpp"
#include "utils/component.hpp"
#include <vector>

namespace Engine {

std::vector<actor_ref> &MainScene::actors() {
    return scene_manager.get_current_scene().actors;
}

std::vector<actor_ref> &MainScene::running_actors() {
    return scene_manager.get_current_scene().running_actors;
}

void MainScene::init() {
    Game &g = Game::getInstance();

    scene_manager.init();
    scene_manager.load_scene(g.get_config().initial_scene);
}

void MainScene::exeute_onstart_tasks() {
    auto &q = game().get_task_manager().get_on_start_components();
    while (!q.empty()) {
        auto task = q.front();
        if (task) {
            runner::add_task(task->onstart_task);
        }
        q.pop();
    }
    runner::commit();
    runner::wait();
}

void MainScene::update_components() {
    for (auto &actor : actors()) {
        for (auto &c : actor->update_components) {
            runner::add_task(c->update_task);
        }
    }
    runner::commit();
    for (auto &actor : actors()) {
        for (auto &c : actor->update_late_components) {
            runner::add_task(c->late_update_task);
        }
    }
    runner::commit();
    runner::wait();
    for (auto &actor : actors()) {
        actor->update_unhandled_components();
    }
    scene_manager.update_unhandled_actors();
}

void MainScene::update_user() {
    exeute_onstart_tasks();
    update_components();
    game().get_event_manager().update();
    // Update world
    if (auto &mgr = Game::getInstance().get_box2d_manager()) {
        mgr->step();
    }
}

std::shared_ptr<GeneralScene> MainScene::update() {
    Game &g = Game::getInstance();
    g.get_renderer().reset_reder_queue();
    scene_manager.load_next_scene_if_need();
    update_user();
    // This scene doesn't change to any other scene so always return nullptr
    return nullptr;
}

}  // namespace Engine
