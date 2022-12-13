
#include "OpenALSoundManager.hpp"

#include "./decoders/ogg.hpp"

#include "framework/files/getFileContent.hpp"

#include "framework/ErrorHandler.hpp"

#include "framework/math/RandomNumberGenerator.hpp"
#include "framework/math/clamp.hpp"

#include <algorithm>
#include <iostream>

OpenALSoundManager::OpenALSoundManager() {

  _context = OpenAlContext::initialise();

  { // listener

    setListener(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));
    OpenAlContext::setListenerVolume(1.0f);

  } // listener

  { // sources

    constexpr uint32_t totalSources = 30;
    std::array<uint32_t, totalSources> allSources;

    OpenAlContext::genSources(allSources.size(), allSources.data());

    _sources.reserve(totalSources); // pre-allocate
    for (unsigned int ii = 0; ii < allSources.size(); ++ii) {

      const uint32_t currSource = allSources.at(ii);

      OpenAlContext::setSourcePitch(currSource, 1.0f);
      // OpenAlContext::setSourcePosition(currSource, 0.0f, 0.0f, 0.0f);
      OpenAlContext::setSourceVolume(currSource, 1.0f);
      OpenAlContext::setSourceMinVolume(currSource, 0.0f);
      OpenAlContext::setSourceMaxVolume(currSource, 1.0f);
      OpenAlContext::setSourceDirection(currSource, 0.0f, 0.0f, 0.0f);
      OpenAlContext::setSourceVelocity(currSource, 0.0f, 0.0f, 0.0f);
      OpenAlContext::setSourceLooping(currSource, false);

      _sources.push_back(currSource);
    }

  } // sources
}

OpenALSoundManager::~OpenALSoundManager() { OpenAlContext::close(_context); }

void OpenALSoundManager::setEnabled(bool enabled) {
  if (_enabled == enabled)
    return;

  _enabled = enabled;

  OpenAlContext::setListenerVolume(0.0f);
}

bool OpenALSoundManager::isEnabled() const { return _enabled; }

void OpenALSoundManager::loadOggFromFile(uint32_t alias,
                                         const std::string& filename) {
  loadOggFromFile(alias, filename, FileUtils::getDefaulCallback());
}

void OpenALSoundManager::loadOggFromFile(uint32_t alias,
                                         const std::string& filename,
                                         FileManager& fileManager) {
  loadOggFromFile(alias, filename,
                  FileUtils::getFileManagerCallback(fileManager));
}

void OpenALSoundManager::loadOggFromFile(
  uint32_t alias, const std::string& filename,
  FileUtils::LoadCallback loadFileCallback) {
  std::string content;
  loadFileCallback(filename, content);

  loadOggFromMemory(alias, content);
}

void OpenALSoundManager::loadOggFromMemory(uint32_t alias,
                                           const std::string& content) {
  if (_bufferSoundsMap.count(alias) != 0)
    return;

  uint32_t newBuffer;
  OpenAlContext::genBuffers(1, &newBuffer);

  std::int32_t channels;
  std::int32_t sampleRate;
  std::int32_t bitsPerSample;
  uint32_t size;
  char* soundData =
    decoders::ogg::decode(content, channels, sampleRate, bitsPerSample, size);

  OpenAlContext::BufferFormat format;
  if (channels == 1 && bitsPerSample == 8)
    format = OpenAlContext::BufferFormat::mono8;
  else if (channels == 1 && bitsPerSample == 16)
    format = OpenAlContext::BufferFormat::mono16;
  else if (channels == 2 && bitsPerSample == 8)
    format = OpenAlContext::BufferFormat::stereo8;
  else if (channels == 2 && bitsPerSample == 16)
    format = OpenAlContext::BufferFormat::stereo16;
  else
    D_THROW(std::runtime_error, "ERROR: unrecognised ogg format"
                                  << ", channels=" << channels
                                  << ", bitsPerSample=" << bitsPerSample);

  // D_MYLOG("channels " << channels);
  // D_MYLOG("bitsPerSample " << bitsPerSample);
  // D_MYLOG("size " << size);

  OpenAlContext::setBufferData(newBuffer, format, soundData, size, sampleRate);

  decoders::ogg::deallocate(&soundData);

  _bufferSoundsMap[alias] = newBuffer;
}

void OpenALSoundManager::playSound(uint32_t alias, const glm::vec3& pos,
                                   float volume, float pitch) {

  if (!_enabled)
    return;

  auto it = _bufferSoundsMap.find(alias);
  if (it == _bufferSoundsMap.end())
    D_THROW(std::runtime_error, "Buffer sound not found, alias: " << alias);

  // D_MYLOG("playing \"" << filename << "\"");

  const uint32_t currBuffer = it->second;

  const uint32_t currSource = _sources[_currentSource];

  _currentSource = (_currentSource + 1) % _sources.size();

  OpenAlContext::SourceStates state =
    OpenAlContext::getSourceState(_currentSource);
  if (state != OpenAlContext::SourceStates::stopped) {
    OpenAlContext::stopSource(currSource);
  }

  OpenAlContext::setSourcePitch(currSource, clamp(pitch, 0.0f, 100.0f));
  OpenAlContext::setSourcePosition(currSource, pos.x, pos.y, pos.z);
  OpenAlContext::setSourceVolume(currSource, clamp(volume, 0.0f, 1.0f));
  // OpenAlContext::setSourceMinVolume(currSource, float volume);
  // OpenAlContext::setSourceMaxVolume(currSource, float volume);
  // OpenAlContext::setSourceDirection(currSource, float x, float y, float z);
  // OpenAlContext::setSourceVelocity(currSource, float x, float y, float z);
  OpenAlContext::setSourceLooping(currSource, false);

  OpenAlContext::setSourceBuffer(currSource, currBuffer);
  OpenAlContext::playSource(currSource);
}

//
//
//

void OpenALSoundManager::setVolume(float level) {
  OpenAlContext::setListenerVolume(clamp(level, 0.0f, 1.0f));
}

void OpenALSoundManager::setListener(const glm::vec3& pos,
                                     const glm::vec3& front,
                                     const glm::vec3& up) {
  OpenAlContext::setListenerPosition(pos.x, pos.y, pos.z);
  OpenAlContext::setListenerOrientation(front, up);
}
