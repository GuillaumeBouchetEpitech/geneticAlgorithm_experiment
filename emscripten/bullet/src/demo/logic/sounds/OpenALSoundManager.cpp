
#include "OpenALSoundManager.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

#include "./utilities/loadWavFile.hpp"
#include "./utilities/checkAlErrors.hpp"

#include <algorithm>
#include <iostream>

#define D_CHECK_AL_ERRORS() checkAlErrors(__FILE__, __func__, __LINE__)
// #define D_CHECK_AL_ERRORS()

OpenALSoundManager::OpenALSoundManager()
{
    _device = alcOpenDevice(NULL);
    if (!_device)
    {
        D_CHECK_AL_ERRORS();
        D_THROW(std::runtime_error, "Fail to open the OpenAL device");
    }

    _context = alcCreateContext(_device, NULL);
    if (!alcMakeContextCurrent(_context))
    {
        D_CHECK_AL_ERRORS();
        D_THROW(std::runtime_error, "Fail to create the OpenAL context");
    }

    {
        alListener3f(AL_POSITION, 0, 0, 0);
        D_CHECK_AL_ERRORS();

        alListener3f(AL_VELOCITY, 0, 0, 0);
        D_CHECK_AL_ERRORS();

        ALfloat listenerOri[] = {
            1.0f, 0.0f, 0.0f, // front
            0.0f, 0.0f, 1.0f, // up
        };
        alListenerfv(AL_ORIENTATION, listenerOri);
        D_CHECK_AL_ERRORS();

        alListenerf(AL_GAIN, 0.5f); // maximum global volume
        D_CHECK_AL_ERRORS();
    }

    {
        const unsigned int totalSources = 30;

        _sources.reserve(totalSources); // pre-allocate
        for (unsigned int ii = 0; ii < totalSources; ++ii)
        {
            ALuint currSource;

            alGenSources((ALuint)1, &currSource);
            D_CHECK_AL_ERRORS();

            alSourcef(currSource, AL_PITCH, 1.0f);
            D_CHECK_AL_ERRORS();

            alSourcef(currSource, AL_GAIN, 1.0f);
            D_CHECK_AL_ERRORS();

            alSourcef(currSource, AL_MIN_GAIN, 0.05f); // minimum volume
            D_CHECK_AL_ERRORS();

            alSourcef(currSource, AL_MAX_GAIN, 1.0f); // maximum volume
            D_CHECK_AL_ERRORS();

            // alSource3f(currSource, AL_POSITION, 30.0f, 0.0f, 0.0f);
            // D_CHECK_AL_ERRORS();

            alSource3f(currSource, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
            D_CHECK_AL_ERRORS();

            alSource3f(currSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            D_CHECK_AL_ERRORS();

            alSourcei(currSource, AL_LOOPING, AL_FALSE);
            D_CHECK_AL_ERRORS();

            // alSourcei(source, AL_BUFFER, buffer);
            // D_CHECK_AL_ERRORS();

            // alSourcePlay(source);
            // D_CHECK_AL_ERRORS();

            _sources.push_back(currSource);
        }
    }
}

OpenALSoundManager::~OpenALSoundManager()
{
    alcCloseDevice(_device);
}

void OpenALSoundManager::setEnabled(bool enabled)
{
    if (_enabled == enabled)
        return;

    _enabled = enabled;

    alListenerf(AL_GAIN, enabled ? 0.5f : 0.0f); // maximum global volume
    D_CHECK_AL_ERRORS();
}

bool OpenALSoundManager::isEnabled() const
{
    return _enabled;
}

void OpenALSoundManager::load(const std::string& filename)
{
    ALuint newBuffer;
    alGenBuffers((ALuint)1, &newBuffer);
    D_CHECK_AL_ERRORS();

    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;
    ALsizei size;
    char* soundData = load_wav(filename, channels, sampleRate, bitsPerSample, size);

    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
        D_THROW(std::runtime_error, "ERROR: unrecognised wave format"
                << ", channels=" << channels
                << ", bitsPerSample=" << bitsPerSample);

    alBufferData(newBuffer, format, soundData, size, sampleRate);
    D_CHECK_AL_ERRORS();

    _bufferMap[filename] = newBuffer;
    _buffersNames.push_back(filename);
}

void OpenALSoundManager::play(const std::string& filename, const glm::vec3& pos)
{
    if (!_enabled)
        return;

    auto it = _bufferMap.find(filename);
    if (it == _bufferMap.end())
        D_THROW(std::runtime_error, "Buffer sound not found"
                << ", flename =\"" << filename << "\"");

    // D_MYLOG("playing \"" << filename << "\"");

    const ALuint currBuffer = it->second;

    const ALuint currSource = _sources[currentSource];

    currentSource = (currentSource + 1) % _sources.size();

    ALint state = AL_PLAYING;
    alGetSourcei(currSource, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING)
    {
        alSourceStop(currSource);
        D_CHECK_AL_ERRORS();
    }

    // alSourcef(currSource, AL_PITCH, 1.0f);
    // D_CHECK_AL_ERRORS();

    // alSourcef(currSource, AL_GAIN, 2.0f); // twice louder
    // D_CHECK_AL_ERRORS();

    alSource3f(currSource, AL_POSITION, pos.x, pos.y, pos.z);
    D_CHECK_AL_ERRORS();

    // alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    // D_CHECK_AL_ERRORS();

    // alSourcei(source, AL_LOOPING, AL_FALSE);
    // D_CHECK_AL_ERRORS();

    alSourcei(currSource, AL_BUFFER, currBuffer);
    D_CHECK_AL_ERRORS();

    alSourcePlay(currSource);
    D_CHECK_AL_ERRORS();
}

void OpenALSoundManager::playRandom(const glm::vec3& pos)
{
    if (!_enabled)
        return;

    int soundIndex = t_RNG::getRangedValue(0, _buffersNames.size());

    play(_buffersNames.at(soundIndex), pos);
}

void OpenALSoundManager::setListener(const glm::vec3& pos,
                                     const glm::vec3& front,
                                     const glm::vec3& up)
{
    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    D_CHECK_AL_ERRORS();

    // alListener3f(AL_VELOCITY, 0, 0, 0);
    // D_CHECK_AL_ERRORS();

    ALfloat listenerOri[] = {
        front.x, front.y, front.z,
        up.x, up.y, up.z
    };
    alListenerfv(AL_ORIENTATION, listenerOri);
    D_CHECK_AL_ERRORS();
}

