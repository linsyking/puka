#include "text_mgr.hpp"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include <cstdint>
#include <string>
#include "SDL2/SDL_ttf.h"
#include "consts.hpp"
#include "game.hpp"
#include "render.hpp"
#include "utils/types.hpp"

namespace Engine {

void TextManager::init() {
    TTF_Init();
}

void TextManager::quit() {
    // Free all textures
    for (auto &text : font_cache) {
        for (auto &font : text.second) {
            TTF_CloseFont(font.second);
        }
    }
    TTF_Quit();
}

void TextManager::draw_text(const std::string &text, const std::string &font_name, int font_size,
                            const vec2 &pos, SDL_Color color) {
    Game &g = Game::getInstance();
    // Check font cache
    TTF_Font *font = nullptr;
    if (font_cache.find(font_name) != font_cache.end() &&
        font_cache[font_name].find(font_size) != font_cache[font_name].end()) {
        font = font_cache[font_name][font_size];
    } else {
        // Not cached
        std::string font_path = font_folder + "/" + font_name + ".ttf";
        font                  = TTF_OpenFont(font_path.c_str(), font_size);
        if (!font) {
            std::cout << "error: font " << font_name << " missing";
            exit(0);
        }
        font_cache[font_name][font_size] = font;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(g.get_renderer().get_raw_renderer(), surface);
    SDL_Rect rect;
    rect.x = static_cast<int>(pos.x);
    rect.y = static_cast<int>(pos.y);
    SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
    rect_renderable r;
    r.texture = texture;
    r.rect    = rect;
    g.get_renderer().add_text_render_task(r);
}

void draw_text(const std::string &str, float x, float y, const std::string &font, int size,
               uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Game     &game = Game::getInstance();
    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    game.get_text_manager().draw_text(str, font, size, {x, y}, color);
}

}  // namespace Engine
