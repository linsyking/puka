#include "end_scene.hpp"
#include "game.hpp"

namespace Engine {
std::shared_ptr<GeneralScene> EndScene::pre_check() {
    game().end_game();
    return nullptr;
}

void EndScene::init() {
    game().get_audio_manager().halt_bgm();
}

}  // namespace Engine
