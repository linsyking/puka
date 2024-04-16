// Chore things
#include <filesystem>
#include <iostream>
#include "consts.hpp"
#include "game.hpp"

namespace Engine {

void check_resource_folder() {
    if (!std::filesystem::exists(resource_folder)) {
        std::cout << "error: resources/ missing";
        exit(0);
    }
}

void check_config() {
    if (!std::filesystem::exists(conf_game)) {
        std::cout << "error: resources/game.config missing";
        exit(0);
    }
}

}  // namespace Engine
