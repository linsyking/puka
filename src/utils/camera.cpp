#include "camera.hpp"
#include "game.hpp"

namespace Engine {

void set_position(float x, float y) {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        main_scene->get_scene_manager().get_camera().position = {x, y};
    }
}

float get_position_x() {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        return main_scene->get_scene_manager().get_camera().position.x;
    }
    return 0;
}

float get_position_y() {
    Game &g = Game::getInstance();
    if (std::shared_ptr<MainScene> main_scene = g.get_main_scene()) {
        return main_scene->get_scene_manager().get_camera().position.y;
    }
    return 0;
}

void set_zoom(float zoom) {
    Game::getInstance().get_config().zoom_factor = zoom;
}

float get_zoom() {
    return Game::getInstance().get_config().zoom_factor;
}

}  // namespace Engine
