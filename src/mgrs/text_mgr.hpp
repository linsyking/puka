#pragma once

#include <string>
#include <unordered_map>
#include "SDL2/SDL_ttf.h"
#include "utils/types.hpp"
// Manage text

namespace Engine {
class TextManager {
private:
    std::unordered_map<std::string, std::unordered_map<int, TTF_Font *>> font_cache;

public:
    void init();
    void quit();
    void draw_text(const std::string &text, const std::string &font_name, int font_size,
                   const vec2 &pos, SDL_Color color);
};

/// Lua binding
void draw_text(const std::string &str, float x, float y, const std::string &font, int size,
               uint8_t r, uint8_t g, uint8_t b, uint8_t a);

}  // namespace Engine
