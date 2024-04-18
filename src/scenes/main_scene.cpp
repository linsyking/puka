#include "main_scene.hpp"
#include "game.hpp"
#include "mgrs/actor.hpp"
#include "mgrs/task_mgr.hpp"
#include "sol.hpp"
#include "task_runner/runner.hpp"
#include "utils/component_proxy.hpp"
#include "utils/dbg.hpp"
#include "utils/lua_component.hpp"
#include "utils/types.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace Engine {

std::vector<actor_ref> &MainScene::actors() {
    return scene_manager.get_current_scene().actors;
}

std::vector<actor_ref> &MainScene::running_actors() {
    return scene_manager.get_current_scene().running_actors;
}

void MainScene::init() {
    scene_manager.init();
    scene_manager.load_scene(game().get_config().initial_scene);
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
            if (auto lc = std::dynamic_pointer_cast<LuaComponent>(c)) {
                lua_ref_raw update_before = lc->ref_tbl.value()["update_before"];
                lua_ref_raw update_after  = lc->ref_tbl.value()["update_after"];
                if (update_before.is<sol::table>()) {
                    // Iterate over the table
                    sol::table t = update_before.as<sol::table>();
                    for (auto it = t.begin(); it != t.end(); ++it) {
                        auto [key, val] = *it;
                        if (val.is<ComponentProxy>()) {
                            ComponentProxy cp = val.as<ComponentProxy>();
                            if (cp.component->is_enabled() && cp.component->has_update) {
                                runner::add_order(c->update_task, cp.component->update_task);
                            }
                        }
                    }
                }
                if (update_after.is<sol::table>()) {
                    // Iterate over the table
                    sol::table t = update_after.as<sol::table>();
                    for (auto it = t.begin(); it != t.end(); ++it) {
                        auto [key, val] = *it;
                        if (val.is<ComponentProxy>()) {
                            ComponentProxy cp = val.as<ComponentProxy>();
                            if (cp.component->is_enabled() && cp.component->has_update) {
                                runner::add_order(cp.component->update_task, c->update_task);
                            }
                        }
                    }
                }
            }
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
    if (auto &mgr = game().get_box2d_manager()) {
        mgr->step();
    }
}

std::shared_ptr<GeneralScene> MainScene::update() {
    game().get_renderer().reset_reder_queue();
    scene_manager.load_next_scene_if_need();
    update_user();
    // This scene doesn't change to any other scene so always return nullptr
    return nullptr;
}

}  // namespace Engine
