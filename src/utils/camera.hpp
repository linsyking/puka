#pragma once

#include "utils/mutex_wrapper.hpp"
#include "utils/types.hpp"
namespace Engine {
class Camera {
public:
    shared_mutex mtx;

    vec2 position = {0, 0};
};

// Lua bindings

void  set_position(float x, float y);
float get_position_x();
float get_position_y();
void  set_zoom(float zoom);
float get_zoom();

}  // namespace Engine
