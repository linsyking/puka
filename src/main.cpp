#include "game.hpp"

int main(int, char **) {
    using namespace Engine;

    Game &game = Game::getInstance();
    game.init();
    game.main_loop();
    return 0;
}
