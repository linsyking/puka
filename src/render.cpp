#include "render.hpp"
#include <mutex>
#include "SDL2/SDL.h"
#include "SDL2/SDL_blendmode.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_rect.h"
#include "config.hpp"
#include "game.hpp"
#include "utils/dbg.hpp"

namespace Engine {

void Renderer::init() {
    Config config = game().get_config();
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
    window   = SDL_CreateWindow(config.game_title.c_str(), 0, 0, config.x_resolution,
                                config.y_resolution, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    // Clear the color
    SDL_SetRenderDrawColor(renderer, config.clear_color.r, config.clear_color.g,
                           config.clear_color.b, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);
}

void Renderer::clear() {
    Config config = game().get_config();
    SDL_SetRenderDrawColor(renderer, config.clear_color.r, config.clear_color.g,
                           config.clear_color.b, 255);
    SDL_RenderClear(renderer);
}

void Renderer::quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::render_copy(SDL_Texture *texture, const SDL_Rect &dstrect) {
    SDL_RenderCopyEx(renderer, texture, nullptr, &dstrect, 0, nullptr, SDL_FLIP_NONE);
}

/* Render queue related */

void Renderer::render_rect_renderable(rect_renderable r) {
    render_copy(r.texture, r.rect);
}

void Renderer::render_ui_renderable(rect_renderable_with_order r) {
    SDL_SetTextureColorMod(r.texture, r.color.r, r.color.g, r.color.b);
    SDL_SetTextureAlphaMod(r.texture, r.color.a);
    render_copy(r.texture, r.rect);
}

void Renderer::render_image_renderable(rect_renderable_full r) {
    float scale_factor = game().get_config().zoom_factor;
    SDL_SetTextureColorMod(r.texture, r.color.r, r.color.g, r.color.b);
    SDL_SetTextureAlphaMod(r.texture, r.color.a);
    SDL_RenderSetScale(renderer, scale_factor, scale_factor);
    SDL_RenderCopyEx(renderer, r.texture, nullptr, &r.rect, static_cast<int>(r.angle), &r.center,
                     r.flip);
    SDL_RenderSetScale(renderer, 1.0f, 1.0f);
}

void Renderer::render_pixel_renderable(pixel_renderable r) {
    SDL_SetRenderDrawColor(renderer, r.color.r, r.color.g, r.color.b, r.color.a);
    SDL_RenderDrawPoint(renderer, static_cast<int>(r.x), static_cast<int>(r.y));
}

void Renderer::add_text_render_task(rect_renderable r) {
    std::unique_lock<std::mutex> lock(mtx.get());
    text_renderables.push_back(r);
}

void Renderer::add_ui_render_task(rect_renderable_with_order r) {
    std::unique_lock<std::mutex> lock(mtx.get());
    ui_renderables.push_back(r);
}

void Renderer::add_image_render_task(rect_renderable_full r) {
    std::unique_lock<std::mutex> lock(mtx.get());
    scene_renderables.push_back(r);
}

void Renderer::add_pixel_render_task(pixel_renderable r) {
    std::unique_lock<std::mutex> lock(mtx.get());
    pixel_renderables.push_back(r);
}

void Renderer::reset_reder_queue() {
    text_renderables.clear();
    scene_renderables.clear();
    ui_renderables.clear();
    pixel_renderables.clear();
}

void Renderer::render_texts() {
    for (auto &r : text_renderables) {
        render_rect_renderable(r);
    }
}

void Renderer::render_ui() {
    // Stable sort the ui_renderables based on order
    std::stable_sort(ui_renderables.begin(), ui_renderables.end(),
                     [](const rect_renderable_with_order &a, const rect_renderable_with_order &b) {
                         return a.order < b.order;
                     });
    for (auto &r : ui_renderables) {
        render_ui_renderable(r);
    }
}

void Renderer::render_scene_images() {
    // Stable sort the scene_renderables based on order
    std::stable_sort(scene_renderables.begin(), scene_renderables.end(),
                     [](const rect_renderable_full &a, const rect_renderable_full &b) {
                         return a.order < b.order;
                     });
    for (auto &r : scene_renderables) {
        render_image_renderable(r);
    }
}

void Renderer::render_pixels() {
    for (auto &r : pixel_renderables) {
        render_pixel_renderable(r);
    }
}

void Renderer::render_all() {
    render_scene_images();
    render_ui();
    render_texts();
    render_pixels();
}

}  // namespace Engine
