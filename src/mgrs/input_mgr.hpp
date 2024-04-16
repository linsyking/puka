#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_scancode.h"
#include "utils/types.hpp"
namespace Engine {

struct MouseStatus {
    bool  left         = 0;
    bool  middle       = 0;
    bool  right        = 0;
    vec2  position     = {.0f, .0f};
    float scroll_delta = 0;
};

class InputManager {
private:
    std::unordered_set<int>                                    key_pressed;
    MouseStatus                                                mouse;
    std::vector<std::pair<unsigned int, SDL_KeyboardEvent>>    keyboard_events;
    std::vector<std::pair<unsigned int, SDL_MouseButtonEvent>> mouse_events;

public:
    void refresh();

    /// True if the key is down on this frame
    bool is_key_pressed(const int key) const;

    bool is_key_event(unsigned int type, SDL_Scancode scancode) const;
    bool is_mouse_event(unsigned int type, int mouse) const;
    void add_key(int key);
    void remove_key(int key);
    void add_mouse(int key);
    void remove_mouse(int key);
    void add_keyboard_event(unsigned int type, SDL_KeyboardEvent event);
    void add_mouse_event(unsigned int type, SDL_MouseButtonEvent event);
    // Convenient functions

    /// Mouse status
    void set_mouse_position(const vec2 &position);
    void set_mouse_scroll_delta(float);

    MouseStatus get_mouse() const;

    /// True if the keycode became pressed on this frame
    bool is_key_down(const SDL_Scancode scancode) const;
    bool is_mouse_down(int mouse) const;

    /// True if the keycode became released on this frame
    bool is_key_up(const SDL_Scancode scancode) const;
    bool is_mouse_up(int mouse) const;
};

// Lua bindings

/// True if the key is down on this frame
bool get_key(const std::string &);
bool get_key_down(const std::string &);
bool get_key_up(const std::string &);

// button num:
// 1: left
// 2: middle
// 3: right
bool  get_mouse(int);
bool  get_mouse_down(int);
bool  get_mouse_up(int);
vec2  get_mouse_position();
float get_mouse_scroll_delta();

}  // namespace Engine
