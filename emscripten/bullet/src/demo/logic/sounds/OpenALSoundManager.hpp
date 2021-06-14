
#pragma once

#include "demo/helpers/GLMath.hpp"

#include <unordered_map>
#include <vector>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

class OpenALSoundManager
{
private:
    bool _enabled = true;

    ALCdevice* _device;
    ALCcontext* _context;

    std::vector<ALuint> _sources;
    unsigned int _currentSource = 0;
    unsigned int _currentSource_2 = 0; // hacky

    std::unordered_map<std::string, ALuint> _bufferExplosionMap;
    std::vector<std::string>                _buffersExplosionNames;

    ALuint _bufferCarEngine;

public:
    OpenALSoundManager();
    ~OpenALSoundManager();

public:
    void setEnabled(bool enabled);
    bool isEnabled() const;

public:
    void loadExplosion(const std::string& filename);
    void playExplosion(const std::string& filename, const glm::vec3& pos);
    void playRandomExplosion(const glm::vec3& pos);

public:
    void loadCarEngine(const std::string& filename);
    void playCarEngine(const glm::vec3& pos, float pitch);

public:
    void setListener(const glm::vec3& pos,
                     const glm::vec3& front,
                     const glm::vec3& up);


};
