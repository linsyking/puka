#include "image_mgr.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include <iostream>
#include <mutex>
#include <utility>
#include "consts.hpp"
#include "game.hpp"
#include "render.hpp"

namespace Engine {

SDL_Texture *ImageManager::load_image(const std::string &name) {
    if (images.find(name) != images.end()) {
        // already loaded
        return images[name];
    }
    std::string   path     = image_folder + "/" + name + ".png";
    SDL_Renderer *renderer = game().get_renderer().get_raw_renderer();
    SDL_Texture  *texture  = IMG_LoadTexture(renderer, path.c_str());
    if (texture == nullptr) {
        std::cout << "error: missing image " << name;
        exit(0);
    }
    images[name] = texture;
    return texture;
}

void ImageManager::unload_image(const std::string &name) {
    if (images.find(name) != images.end()) {
        // already loaded
        SDL_DestroyTexture(images[name]);
        images.erase(name);
        return;
    } else {
        return;
    }
}

void ImageManager::init() {
    IMG_Init(IMG_INIT_PNG);
}

void ImageManager::quit() {
    // Free all textures
    for (auto &image : images) {
        SDL_DestroyTexture(image.second);
    }
    IMG_Quit();
}

std::pair<int, int> ImageManager::get_size(const std::string &img) {
    if (images.find(img) != images.end()) {
        int w = 0, h = 0;
        SDL_QueryTexture(images[img], NULL, NULL, &w, &h);
        return std::make_pair(w, h);
    } else {
        load_image(img);
        return get_size(img);
    }
}

void image_drawui(const std::string &name, float x, float y) {
    image_drawui_ex(name, x, y, 255, 255, 255, 255, 0);
}

void image_drawui_ex(const std::string &name, float x, float y, float r, float g, float b, float a,
                     int sorting_order) {
    rect_renderable_with_order renderable;
    SDL_Rect                   target;
    target.x = static_cast<int>(x);
    target.y = static_cast<int>(y);
    {
        ImageManager                &im = game().get_image_manager();
        std::unique_lock<std::mutex> lock(im.mtx.get());
        renderable.texture = game().get_image_manager().load_image(name);
        auto size          = im.get_size(name);
        target.w           = size.first;
        target.h           = size.second;
    }
    renderable.rect    = target;
    renderable.order   = sorting_order;
    renderable.color.r = static_cast<uint8_t>(r);
    renderable.color.g = static_cast<uint8_t>(g);
    renderable.color.b = static_cast<uint8_t>(b);
    renderable.color.a = static_cast<uint8_t>(a);
    game().get_renderer().add_ui_render_task(renderable);
}

void image_draw(const std::string &name, float x, float y) {
    image_draw_ex(name, x, y, 0, 1, 1, 0.5f, 0.5f, 255, 255, 255, 255, 0);
}

void image_draw_ex(const std::string &name, float x, float y, float degrees, float scale_x,
                   float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a,
                   int sorting_order) {
    rect_renderable_full renderable;
    vec2                 camera_position = {};
    if (std::shared_ptr<MainScene> main_scene = game().get_main_scene()) {
        camera_position = main_scene->get_scene_manager().get_camera().position;
    }
    float               center_x = game().get_config().x_resolution / 2.0f;
    float               center_y = game().get_config().y_resolution / 2.0f;
    std::pair<int, int> size     = {0, 0};
    {
        ImageManager &im = game().get_image_manager();
        std::unique_lock<std::mutex> lock(im.mtx.get());
        size               = im.get_size(name);
        renderable.texture = im.load_image(name);
    }
    float scale         = game().get_config().zoom_factor;
    int   pivot_pixel_x = static_cast<int>(size.first * pivot_x * scale_x);
    int   pivot_pixel_y = static_cast<int>(size.second * pivot_y * scale_y);
    renderable.center.x = pivot_pixel_x;
    renderable.center.y = pivot_pixel_y;

    renderable.rect.x  = static_cast<int>((((x - camera_position.x) * 100.0f + center_x) * scale +
                                          (1 - scale) * center_x - pivot_pixel_x * scale) /
                                         scale);
    renderable.rect.y  = static_cast<int>((((y - camera_position.y) * 100.0f + center_y) * scale +
                                          (1 - scale) * center_y - pivot_pixel_y * scale) /
                                         scale);
    renderable.rect.w  = static_cast<int>(size.first * scale_x);
    renderable.rect.h  = static_cast<int>(size.second * scale_y);
    renderable.angle   = degrees;
    renderable.flip    = SDL_FLIP_NONE;
    renderable.order   = sorting_order;
    renderable.color.r = static_cast<uint8_t>(r);
    renderable.color.g = static_cast<uint8_t>(g);
    renderable.color.b = static_cast<uint8_t>(b);
    renderable.color.a = static_cast<uint8_t>(a);
    game().get_renderer().add_image_render_task(renderable);
}

void image_draw_pixel(float x, float y, float r, float g, float b, float a) {
    pixel_renderable renderable;
    renderable.x       = static_cast<int>(x);
    renderable.y       = static_cast<int>(y);
    renderable.color.r = static_cast<uint8_t>(r);
    renderable.color.g = static_cast<uint8_t>(g);
    renderable.color.b = static_cast<uint8_t>(b);
    renderable.color.a = static_cast<uint8_t>(a);
    game().get_renderer().add_pixel_render_task(renderable);
}

}  // namespace Engine
