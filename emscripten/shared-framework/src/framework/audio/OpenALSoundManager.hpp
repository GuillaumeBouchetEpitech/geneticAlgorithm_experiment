
#pragma once

#include "OpenAlContext.hpp"

#include "framework/files/FileLoadFileCallback.hpp"
#include "framework/files/FileManager.hpp"

#include "framework/helpers/GLMath.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class OpenALSoundManager {
private:
  bool _enabled = true;

  OpenAlContext::Context _context;

  std::vector<uint32_t> _sources;
  unsigned int _currentSource = 0;

  std::unordered_map<uint32_t, uint32_t> _bufferSoundsMap;

public:
  OpenALSoundManager();
  ~OpenALSoundManager();

public:
  void setEnabled(bool enabled);
  bool isEnabled() const;

public:
  void loadOggFromFile(uint32_t alias, const std::string& filename);
  void loadOggFromFile(uint32_t alias, const std::string& filename,
                       FileManager& fileManager);
  void loadOggFromFile(uint32_t alias, const std::string& filename,
                       FileUtils::LoadCallback loadFileCallback);
  void loadOggFromMemory(uint32_t alias, const std::string& content);

public:
  void playSound(uint32_t alias, const glm::vec3& pos, float volume,
                 float pitch);

public:
  void setVolume(float level);
  void setListener(const glm::vec3& pos, const glm::vec3& front,
                   const glm::vec3& up);
};
