#pragma once

#include "utils/types.hpp"
namespace Engine {
class Camera {
public:
    vec2 position = {0, 0};
};

// Lua bindings

void  set_position(float x, float y);
float get_position_x();
float get_position_y();
void  set_zoom(float zoom);
float get_zoom();

}  // namespace Engine
