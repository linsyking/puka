#pragma once

#include <string>
#include <thread>

namespace Engine {

struct RGB {
    int r = 255;
    int g = 255;
    int b = 255;
};

struct Config {
    // Game config

    // Game initial scene
    std::string initial_scene;

    std::string game_title;

    // Rendering config
    // Clear color
    RGB clear_color;

    // Rendering resolution
    int x_resolution = 640;
    int y_resolution = 360;

    // Camera zoom factor
    float zoom_factor = 1.0f;

    // Camera smooth ease factor
    float cam_ease_factor = 1.0f;

    // Number of threads to use
    int num_threads = std::thread::hardware_concurrency();
};

Config load_config();

}  // namespace Engine
