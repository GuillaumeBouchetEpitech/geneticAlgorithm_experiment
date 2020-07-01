
#include "SoundManager.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

#include <SDL2/SDL_mixer.h> // <= Mix_LoadWAV/PlayChannel/FreeChunk

#include <algorithm>

SoundManager::SoundManager()
{
    const int totalChannels = 16;

    for (int ii = 0; ii < totalChannels; ++ii)
        _freeChannels.push_back(ii);

    Mix_AllocateChannels(totalChannels);
}

//

void SoundManager::load(const std::string& filename, int volume)
{
    if (_soundMap.count(filename) > 0)
        D_THROW(std::runtime_error, "Already loaded sound"
                << ", filename: \"" << filename << "\"");

    Mix_Chunk* newChunk;

    if (!(newChunk = Mix_LoadWAV(filename.c_str())))
        D_THROW(std::runtime_error, "Could not load a sound"
                << ", filename: \"" << filename << "\""
                << ", error: " << Mix_GetError());

    _soundMap[filename] = { newChunk, volume };
    _soundNames.push_back(filename);
}

void SoundManager::play(const std::string& filename)
{
    auto it = _soundMap.find(filename);
    if (it == _soundMap.end())
        D_THROW(std::runtime_error, "Sound not found"
                << ", filename: \"" << filename << "\"");

    auto soundData = it->second;
    auto* chunk = soundData.chunk;

    // find free channel
    if (_freeChannels.empty())
    {
        // find oldest sound

        auto cmpFunc = [](const t_playingSoundData& dataA, const t_playingSoundData& dataB) { return dataA.age > dataB.age; };
        std::sort(_playingSounds.begin(), _playingSounds.end(), cmpFunc);

        _freeChannels.push_back(_playingSounds.back().channel);
        _playingSounds.pop_back();
    }

    int channel = _freeChannels.back();
    _freeChannels.pop_back();

    _playingSounds.push_back({ ++_currentAge, channel, chunk });

    // D_MYLOG("channel=" << channel);

    if (Mix_Playing(channel))
        Mix_HaltChannel(channel);

    if (Mix_PlayChannel(channel, chunk, 0) == -1)
        D_THROW(std::runtime_error, "Could not play a sound"
            << ", filename: \"" << filename << "\""
            << ", error: " << Mix_GetError());

    Mix_Volume(channel, soundData.volume);
    // // Mix_SetDistance
    // Mix_SetPanning(channel, 255, 255 - 255);

}

void SoundManager::playRandom()
{
    int soundIndex = t_RNG::getRangedValue(0, _soundNames.size());

    play(_soundNames.at(soundIndex));
}

