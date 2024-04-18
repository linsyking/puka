#include "game.hpp"
#include "SDL2/SDL_events.h"
#include "general_scene.hpp"
#include "scenes/main_scene.hpp"
#include "utils/dbg.hpp"
#include <memory>

namespace Engine {

Game::Game() {
    check_resource_folder();
    check_config();
    config = load_config();
}

void Game::quit() {
    // Destructor
    renderer.quit();
    image_manager.quit();
    text_manager.quit();
    audio_manager.quit();
    task_manager.quit();
    event_manager.quit();
}

void Game::init() {
    // Create window
    renderer.init();
    image_manager.init();
    text_manager.init();
    audio_manager.init();
    task_manager.init();
    // Init Scene
    current_scene = std::make_shared<MainScene>();
    // MainScene init cannot fail
    while (std::shared_ptr<GeneralScene> next = current_scene->pre_check()) {
        current_scene = next;
    }
    if (running) {
        current_scene->init();
    }
}

void Game::update_input() {
    SDL_Event event;
    input_manager.refresh();
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            input_manager.add_key(event.key.keysym.scancode);
            input_manager.add_keyboard_event(event.type, event.key);
        }
        if (event.type == SDL_KEYUP) {
            input_manager.remove_key(event.key.keysym.scancode);
            input_manager.add_keyboard_event(event.type, event.key);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            input_manager.add_mouse(event.button.button);
            input_manager.add_mouse_event(event.type, event.button);
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            input_manager.remove_mouse(event.button.button);
            input_manager.add_mouse_event(event.type, event.button);
        }
        if (event.type == SDL_MOUSEMOTION) {
            input_manager.set_mouse_position({event.motion.x, event.motion.y});
        }
        if (event.type == SDL_MOUSEWHEEL) {
            input_manager.set_mouse_scroll_delta(event.wheel.preciseY);
        }
    }
}

void Game::main_loop() {
    while (running) {
        // Do not render when updating
        DBGOUT("---- Frame " << get_frame_num() << " ----");
        update_input();
        std::shared_ptr<GeneralScene> res = current_scene->update();
        if (!running) {
            break;
        }
        if (res) {
            current_scene->post_quit();
            current_scene = res;
            while (std::shared_ptr<GeneralScene> next = current_scene->pre_check()) {
                current_scene = next;
            }
            if (running) {
                current_scene->init();
            }
        }

        // Render start
        renderer.clear();
        renderer.render_all();
        renderer.render_present();

        frame_num++;
    }
    // Quit the game
    quit();
}

Game &game() {
    return Game::getInstance();
}

}  // namespace Engine
