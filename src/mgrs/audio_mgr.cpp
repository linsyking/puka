#include "audio_mgr.hpp"
#include "SDL2/SDL_mixer.h"
#include <filesystem>
#include <iostream>
#include <string>
#include "game.hpp"
#include "consts.hpp"

namespace Engine {

std::string AudioManager::music_path(const std::string &name) {
    std::string path = audio_folder + "/" + name + ".wav";
    if (!std::filesystem::exists(path)) {
        path = audio_folder + "/" + name + ".ogg";
    }
    if (!std::filesystem::exists(path)) {
        std::cout << " error: failed to play audio clip " << name;
        exit(0);
    }
    return path;
}

void AudioManager::init() {
    Mix_Init(MIX_INIT_OGG);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    // Allocate 8 channels
    Mix_AllocateChannels(50);
}

void AudioManager::change_bgm(const std::string &name) {
    bgm = load_channel(name);
}

Mix_Chunk *AudioManager::load_channel(const std::string &name) {
    if (loaded_channels.find(name) != loaded_channels.end()) {
        return loaded_channels[name];
    }
    Mix_Chunk *chunk      = Mix_LoadWAV(music_path(name).c_str());
    loaded_channels[name] = chunk;
    return chunk;
}

void AudioManager::play_bgm(int loop) {
    Mix_PlayChannel(0, bgm, loop);
}

void AudioManager::halt_bgm() {
    halt_channel(0);
}

void AudioManager::quit() {
    Mix_Quit();
    // Free all loaded channels
    for (auto &pair : loaded_channels) {
        Mix_FreeChunk(pair.second);
    }
}

void AudioManager::play_channel(int channel, const std::string &name, int loop) {
    Mix_Chunk *chunk = load_channel(name);
    Mix_PlayChannel(channel, chunk, loop ? -1 : 0);
}

void AudioManager::halt_channel(int channel) {
    Mix_HaltChannel(channel);
}

void play_audio(int channel, const std::string &name, bool loop) {
    Game::getInstance().get_audio_manager().play_channel(channel, name, loop);
}

void halt_audio(int channel) {
    Mix_HaltChannel(channel);
}

void set_volume(int channel, float volume) {
    int vol = static_cast<int>(volume);
    Mix_Volume(channel, vol);
}

}  // namespace Engine
