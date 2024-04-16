#pragma once

#include "SDL2/SDL_mixer.h"
#include <string>
#include <unordered_map>
namespace Engine {
class AudioManager {
private:
    Mix_Chunk                                   *bgm = nullptr;  // BGM on channel 0
    std::unordered_map<std::string, Mix_Chunk *> loaded_channels;
    std::string                                  music_path(const std::string &name);
    Mix_Chunk                                   *load_channel(const std::string &name);

public:
    void init();
    void quit();
    void halt_bgm();
    void change_bgm(const std::string &name);
    void play_bgm(int loop = -1);
    void play_channel(int channel, const std::string &name, int loop = 0);
    void halt_channel(int channel);
};

// Lua bindings

void play_audio(int channel, const std::string &name, bool loop);
void halt_audio(int channel);
void set_volume(int channel, float volume);

}  // namespace Engine
