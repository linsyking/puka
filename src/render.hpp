#pragma once

// Renderer module

#include <vector>
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "utils/mutex_wrapper.hpp"

namespace Engine {

struct rect_renderable {
    SDL_Texture *texture = nullptr;
    SDL_Rect     rect;
};

struct rect_renderable_with_order {
    SDL_Texture *texture = nullptr;
    SDL_Rect     rect;
    SDL_Color    color;
    int          order;
};

struct rect_renderable_full {
    SDL_Texture     *texture = nullptr;
    SDL_Rect         rect;
    float            angle;
    SDL_Point        center;
    SDL_RendererFlip flip;
    SDL_Color        color;
    int              order;
};

struct pixel_renderable {
    int       x;
    int       y;
    SDL_Color color;
};

class Renderer {
private:
    SDL_Window                             *window   = nullptr;
    SDL_Renderer                           *renderer = nullptr;
    std::vector<rect_renderable_full>       scene_renderables;
    std::vector<rect_renderable_with_order> ui_renderables;
    std::vector<rect_renderable>            text_renderables;
    std::vector<pixel_renderable>           pixel_renderables;
    void                                    render_texts();
    void                                    render_ui();
    void                                    render_scene_images();
    void                                    render_pixels();

public:
    unique_mutex  mtx;
    void          init();
    void          render_present() { SDL_RenderPresent(renderer); }
    SDL_Renderer *get_raw_renderer() { return renderer; }
    SDL_Window   *get_raw_window() { return window; }
    void          quit();

    /// Clear screen color to deafult color
    void clear();

    /// Reset the render queue, should be called before rendering on every frame
    void reset_reder_queue();

    /// Render a rect_renderable
    void render_rect_renderable(rect_renderable);
    void render_ui_renderable(rect_renderable_with_order);
    void render_image_renderable(rect_renderable_full);
    void render_pixel_renderable(pixel_renderable);

    /// Add a rect_renderable to the render queue
    void add_text_render_task(rect_renderable);
    void add_ui_render_task(rect_renderable_with_order);
    void add_image_render_task(rect_renderable_full);
    void add_pixel_render_task(pixel_renderable);

    void render_copy(SDL_Texture *texture, const SDL_Rect &dstrect);

    /// Render all queues
    void render_all();
};
}  // namespace Engine
