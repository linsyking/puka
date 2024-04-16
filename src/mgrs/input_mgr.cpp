#include "input_mgr.hpp"
#include <string>
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_scancode.h"
#include "game.hpp"

namespace Engine {
bool InputManager::is_key_event(unsigned int type, SDL_Scancode scancode) const {
    for (auto &e : keyboard_events) {
        if (e.first == type && e.second.keysym.scancode == scancode) {
            return true;
        }
    }
    return false;
}

void InputManager::refresh() {
    keyboard_events.clear();
    mouse_events.clear();
    mouse.scroll_delta = 0;
}

bool InputManager::is_key_pressed(const int key) const {
    return key_pressed.find(key) != key_pressed.end();
}

bool InputManager::is_mouse_event(unsigned int type, int mouse) const {
    for (auto &e : mouse_events) {
        if (e.first == type && e.second.button == mouse) {
            return true;
        }
    }
    return false;
}

void InputManager::add_key(int key) {
    key_pressed.insert(key);
}

void InputManager::remove_key(int key) {
    key_pressed.erase(key);
}

void InputManager::add_mouse(int key) {
    if (key == SDL_BUTTON_LEFT) {
        mouse.left = true;
    } else if (key == SDL_BUTTON_MIDDLE) {
        mouse.middle = true;
    } else if (key == SDL_BUTTON_RIGHT) {
        mouse.right = true;
    }
}

void InputManager::remove_mouse(int key) {
    if (key == SDL_BUTTON_LEFT) {
        mouse.left = false;
    } else if (key == SDL_BUTTON_MIDDLE) {
        mouse.middle = false;
    } else if (key == SDL_BUTTON_RIGHT) {
        mouse.right = false;
    }
}

void InputManager::add_keyboard_event(unsigned int type, SDL_KeyboardEvent event) {
    // Remove old events that match the new event scancode
    keyboard_events.erase(
        std::remove_if(
            keyboard_events.begin(), keyboard_events.end(),
            [event](const auto &e) { return e.second.keysym.scancode == event.keysym.scancode; }),
        keyboard_events.end());
    keyboard_events.push_back({type, event});
}

void InputManager::add_mouse_event(unsigned int type, SDL_MouseButtonEvent event) {
    // Remove old events that match the new event button
    mouse_events.erase(
        std::remove_if(mouse_events.begin(), mouse_events.end(),
                       [event](const auto &e) { return e.second.button == event.button; }),
        mouse_events.end());
    mouse_events.push_back({type, event});
}

bool InputManager::is_key_down(const SDL_Scancode scancode) const {
    return is_key_event(SDL_KEYDOWN, scancode);
}

bool InputManager::is_key_up(const SDL_Scancode scancode) const {
    return is_key_event(SDL_KEYUP, scancode);
}

bool InputManager::is_mouse_down(int mouse) const {
    return is_mouse_event(SDL_MOUSEBUTTONDOWN, mouse);
}

bool InputManager::is_mouse_up(int mouse) const {
    return is_mouse_event(SDL_MOUSEBUTTONUP, mouse);
}

void InputManager::set_mouse_position(const vec2 &position) {
    mouse.position = position;
}

void InputManager::set_mouse_scroll_delta(float delta) {
    mouse.scroll_delta = delta;
}

MouseStatus InputManager::get_mouse() const {
    return mouse;
}

bool get_mouse(int key) {
    MouseStatus mouse = Game::getInstance().get_input_manager().get_mouse();
    if (key == SDL_BUTTON_LEFT) {
        return mouse.left;
    } else if (key == SDL_BUTTON_MIDDLE) {
        return mouse.middle;
    } else if (key == SDL_BUTTON_RIGHT) {
        return mouse.right;
    }
    return false;
}

bool get_mouse_down(int key) {
    return Game::getInstance().get_input_manager().is_mouse_down(key);
}

bool get_mouse_up(int key) {
    return Game::getInstance().get_input_manager().is_mouse_up(key);
}

vec2 get_mouse_position() {
    return Game::getInstance().get_input_manager().get_mouse().position;
}

float get_mouse_scroll_delta() {
    return Game::getInstance().get_input_manager().get_mouse().scroll_delta;
}

std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
  // Directional (arrow) Keys
    {       "up",           SDL_SCANCODE_UP},
    {     "down",         SDL_SCANCODE_DOWN},
    {    "right",        SDL_SCANCODE_RIGHT},
    {     "left",         SDL_SCANCODE_LEFT},

 // Misc Keys
    {   "escape",       SDL_SCANCODE_ESCAPE},

 // Modifier Keys
    {   "lshift",       SDL_SCANCODE_LSHIFT},
    {   "rshift",       SDL_SCANCODE_RSHIFT},
    {    "lctrl",        SDL_SCANCODE_LCTRL},
    {    "rctrl",        SDL_SCANCODE_RCTRL},
    {     "lalt",         SDL_SCANCODE_LALT},
    {     "ralt",         SDL_SCANCODE_RALT},

 // Editing Keys
    {      "tab",          SDL_SCANCODE_TAB},
    {   "return",       SDL_SCANCODE_RETURN},
    {    "enter",       SDL_SCANCODE_RETURN},
    {"backspace",    SDL_SCANCODE_BACKSPACE},
    {   "delete",       SDL_SCANCODE_DELETE},
    {   "insert",       SDL_SCANCODE_INSERT},

 // Character Keys
    {    "space",        SDL_SCANCODE_SPACE},
    {        "a",            SDL_SCANCODE_A},
    {        "b",            SDL_SCANCODE_B},
    {        "c",            SDL_SCANCODE_C},
    {        "d",            SDL_SCANCODE_D},
    {        "e",            SDL_SCANCODE_E},
    {        "f",            SDL_SCANCODE_F},
    {        "g",            SDL_SCANCODE_G},
    {        "h",            SDL_SCANCODE_H},
    {        "i",            SDL_SCANCODE_I},
    {        "j",            SDL_SCANCODE_J},
    {        "k",            SDL_SCANCODE_K},
    {        "l",            SDL_SCANCODE_L},
    {        "m",            SDL_SCANCODE_M},
    {        "n",            SDL_SCANCODE_N},
    {        "o",            SDL_SCANCODE_O},
    {        "p",            SDL_SCANCODE_P},
    {        "q",            SDL_SCANCODE_Q},
    {        "r",            SDL_SCANCODE_R},
    {        "s",            SDL_SCANCODE_S},
    {        "t",            SDL_SCANCODE_T},
    {        "u",            SDL_SCANCODE_U},
    {        "v",            SDL_SCANCODE_V},
    {        "w",            SDL_SCANCODE_W},
    {        "x",            SDL_SCANCODE_X},
    {        "y",            SDL_SCANCODE_Y},
    {        "z",            SDL_SCANCODE_Z},
    {        "0",            SDL_SCANCODE_0},
    {        "1",            SDL_SCANCODE_1},
    {        "2",            SDL_SCANCODE_2},
    {        "3",            SDL_SCANCODE_3},
    {        "4",            SDL_SCANCODE_4},
    {        "5",            SDL_SCANCODE_5},
    {        "6",            SDL_SCANCODE_6},
    {        "7",            SDL_SCANCODE_7},
    {        "8",            SDL_SCANCODE_8},
    {        "9",            SDL_SCANCODE_9},
    {        "/",        SDL_SCANCODE_SLASH},
    {        ";",    SDL_SCANCODE_SEMICOLON},
    {        "=",       SDL_SCANCODE_EQUALS},
    {        "-",        SDL_SCANCODE_MINUS},
    {        ".",       SDL_SCANCODE_PERIOD},
    {        ",",        SDL_SCANCODE_COMMA},
    {        "[",  SDL_SCANCODE_LEFTBRACKET},
    {        "]", SDL_SCANCODE_RIGHTBRACKET},
    {       "\\",    SDL_SCANCODE_BACKSLASH},
    {        "'",   SDL_SCANCODE_APOSTROPHE}
};

bool get_key(const std::string &key) {
    return Game::getInstance().get_input_manager().is_key_pressed(__keycode_to_scancode[key]);
}

bool get_key_down(const std::string &key) {
    return Game::getInstance().get_input_manager().is_key_down(__keycode_to_scancode[key]);
}

bool get_key_up(const std::string &key) {
    return Game::getInstance().get_input_manager().is_key_up(__keycode_to_scancode[key]);
}

}  // namespace Engine
