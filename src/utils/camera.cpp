#include "camera.hpp"
#include "game.hpp"

namespace Engine {

void set_position(float x, float y) {
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        main_scene->get_scene_manager().get_camera().position = {x, y};
    }
}

float get_position_x() {
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        return main_scene->get_scene_manager().get_camera().position.x;
    }
    return 0;
}

float get_position_y() {
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        return main_scene->get_scene_manager().get_camera().position.y;
    }
    return 0;
}

void set_zoom(float zoom) {
    game().get_config().zoom_factor = zoom;
}

float get_zoom() {
    return game().get_config().zoom_factor;
}

}  // namespace Engine
