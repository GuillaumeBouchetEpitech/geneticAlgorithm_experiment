
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <cstdint>
#include <string>

struct ALCdevice_struct;
struct ALCcontext_struct;

class OpenAlContext {
public:
  struct Context {
    ALCdevice_struct* device;
    ALCcontext_struct* context;
  };

public:
  static Context initialise();
  static void close(Context& context);

public:
  static void setListenerPosition(float x, float y, float z);
  static void setListenerPosition(const glm::vec3& pos);

  static void setListenerOrientation(float frontX, float frontY, float frontZ,
                                     float upX, float upY, float upZ);
  static void setListenerOrientation(const glm::vec3& front,
                                     const glm::vec3& up);

  static void setListenerVolume(float volume);

public:
  static void genSources(uint32_t total, uint32_t* sources);
  static void deleteSources(uint32_t total, const uint32_t* sources);

  enum class SourceStates {
    initial,
    playing,
    paused,
    stopped,
  };

  static SourceStates getSourceState(uint32_t source);
  static void stopSource(uint32_t source);
  static void setSourcePitch(uint32_t source, float pitch);
  static void setSourcePosition(uint32_t source, float x, float y, float z);
  static void setSourceVolume(uint32_t source, float volume);
  static void setSourceMinVolume(uint32_t source, float volume);
  static void setSourceMaxVolume(uint32_t source, float volume);
  static void setSourceDirection(uint32_t source, float x, float y, float z);
  static void setSourceVelocity(uint32_t source, float x, float y, float z);
  static void setSourceLooping(uint32_t source, bool isLooping);
  static void setSourceBuffer(uint32_t source, uint32_t buffer);
  static void playSource(uint32_t source);

public:
  static void genBuffers(uint32_t total, uint32_t* buffers);
  static void deleteBuffers(uint32_t total, const uint32_t* buffers);

  enum class BufferFormat {
    mono8,
    mono16,
    stereo8,
    stereo16,
  };

  static void setBufferData(uint32_t buffer, BufferFormat format,
                            const char* soundData, uint32_t size,
                            std::int32_t sampleRate);
};
