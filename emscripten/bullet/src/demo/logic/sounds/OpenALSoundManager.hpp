
#pragma once

#include "thirdparty/GLMath.hpp"

#include <unordered_map>
#include <vector>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

class OpenALSoundManager
{
private:
    bool _enabled = false;

    ALCdevice* _device;
    ALCcontext* _context;

    std::vector<ALuint> _sources;
    unsigned int currentSource = 0;

    std::unordered_map<std::string, ALuint> _bufferMap;
    std::vector<std::string>                _buffersNames;

public:
    OpenALSoundManager();
    ~OpenALSoundManager();

public:
    void setEnabled(bool enabled);
    bool isEnabled() const;

public:
    void load(const std::string& filename);
    void play(const std::string& filename, const glm::vec3& pos);
    void playRandom(const glm::vec3& pos);
    void setListener(const glm::vec3& pos,
                     const glm::vec3& front,
                     const glm::vec3& up);


};
