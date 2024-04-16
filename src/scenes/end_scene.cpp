#include "end_scene.hpp"
#include "game.hpp"

namespace Engine {
std::shared_ptr<GeneralScene> EndScene::pre_check() {
    Game &g = Game::getInstance();
    g.end_game();
    return nullptr;
}

void EndScene::init() {
    Game &g = Game::getInstance();
    g.get_audio_manager().halt_bgm();
}

}  // namespace Engine
