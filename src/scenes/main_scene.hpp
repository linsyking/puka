#pragma once

#include "general_scene.hpp"
#include "mgrs/scene_mgr.hpp"
namespace Engine {

class MainScene : public GeneralScene {
private:
    SceneManager scene_manager;
    void         update_user();
    void         exeute_onstart_tasks();
    void         update_components();

public:
    void                          init() override;
    std::shared_ptr<GeneralScene> update() override;
    std::vector<actor_ref>       &actors();
    std::vector<actor_ref>       &running_actors();
    SceneManager                 &get_scene_manager() { return scene_manager; }
};
}  // namespace Engine
