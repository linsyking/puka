#pragma once

// Manage Images

#include "SDL2/SDL_render.h"
#include "utils/mutex_wrapper.hpp"
#include <string>
#include <unordered_map>
namespace Engine {
class ImageManager {
private:
    std::unordered_map<std::string, SDL_Texture *> images;

public:
    void                init();
    void                quit();
    SDL_Texture        *load_image(const std::string &name);
    void                unload_image(const std::string &name);
    std::pair<int, int> get_size(const std::string &text);
    unique_mutex        mtx;
};

// Lua bindings

void image_drawui(const std::string &name, float x, float y);
void image_drawui_ex(const std::string &name, float x, float y, float r, float g, float b, float a,
                     int sorting_order);
void image_draw(const std::string &name, float x, float y);
void image_draw_ex(const std::string &name, float x, float y, float degrees, float scale_x,
                   float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a,
                   int sorting_order);
void image_draw_pixel(float x, float y, float r, float g, float b, float a);

}  // namespace Engine
