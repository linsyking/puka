#pragma once

#include <memory>
#include <optional>
#include "mgrs/audio_mgr.hpp"
#include "config.hpp"
#include "general_scene.hpp"
#include "mgrs/box2d_mgr.hpp"
#include "mgrs/component_mgr.hpp"
#include "mgrs/event_mgr.hpp"
#include "mgrs/image_mgr.hpp"
#include "mgrs/input_mgr.hpp"
#include "mgrs/task_mgr.hpp"
#include "render.hpp"
#include "mgrs/text_mgr.hpp"
#include "scenes/main_scene.hpp"

namespace Engine {

class Game {
    // Singleton
private:
    Game();
    bool   running   = true;
    size_t frame_num = 0;

    Config config;

    Renderer                    renderer;
    ImageManager                image_manager;
    TextManager                 text_manager;
    AudioManager                audio_manager;
    InputManager                input_manager;
    ComponentManager            component_manager;
    std::optional<Box2DManager> box2d_manager;
    EventManager                event_manager;
    TaskManager                 task_manager;

    void quit();
    void update_input();

public:
    static Game &getInstance() {
        static Game instance;
        return instance;
    }
    void                          terminate() { running = false; }
    std::shared_ptr<GeneralScene> current_scene;
    void                          main_loop();
    void                          init();
    void                          end_game() { running = false; }
    Config                       &get_config() { return config; }
    Renderer                     &get_renderer() { return renderer; }
    ImageManager                 &get_image_manager() { return image_manager; }
    TextManager                  &get_text_manager() { return text_manager; }
    AudioManager                 &get_audio_manager() { return audio_manager; }
    InputManager                 &get_input_manager() { return input_manager; }
    ComponentManager             &get_component_manager() { return component_manager; }
    std::optional<Box2DManager>  &get_box2d_manager() { return box2d_manager; }
    EventManager                 &get_event_manager() { return event_manager; }
    TaskManager                  &get_task_manager() { return task_manager; }

    std::shared_ptr<MainScene> get_main_scene() {
        return std::dynamic_pointer_cast<MainScene>(current_scene);
    }
    static int get_frame_num() { return Game::getInstance().frame_num; }

    Game(const Game &)            = delete;
    Game &operator=(const Game &) = delete;
    Game(Game &&)                 = delete;
    Game &operator=(Game &&)      = delete;
};

void check_resource_folder();
void check_config();

Game &game();

}  // namespace Engine
