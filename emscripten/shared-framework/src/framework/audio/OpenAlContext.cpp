
#include "OpenAlContext.hpp"

#include "framework/system/ErrorHandler.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <array>

namespace name {

void checkAlErrors(const char* filename, const char* funcname, int line) {
  ALenum error = alGetError();

  if (error == AL_NO_ERROR)
    return;

  D_MYLOG("***ERROR*** (" << filename << ": " << funcname << ": " << line
                          << ")");
  switch (error) {
  case AL_INVALID_NAME:
    D_THROW(
      std::runtime_error,
      "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
    break;
  case AL_INVALID_ENUM:
    D_THROW(std::runtime_error, "AL_INVALID_ENUM: an invalid enum value was "
                                "passed to an OpenAL function");
    break;
  case AL_INVALID_VALUE:
    D_THROW(
      std::runtime_error,
      "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
    break;
  case AL_INVALID_OPERATION:
    D_THROW(std::runtime_error,
            "AL_INVALID_OPERATION: the requested operation is not valid");
    break;
  case AL_OUT_OF_MEMORY:
    D_THROW(std::runtime_error, "AL_OUT_OF_MEMORY: the requested operation "
                                "resulted in OpenAL running out of memory");
    break;
  default:
    D_THROW(std::runtime_error, "UNKNOWN AL ERROR: " << error);
  }
}

} // namespace name

// #define D_CHECK_AL_ERRORS() checkAlErrors(__FILE__, __func__, __LINE__)
#define D_CHECK_AL_ERRORS()

OpenAlContext::Context OpenAlContext::initialise() {
  Context newContext;
  newContext.device = alcOpenDevice(NULL);
  if (!newContext.device) {
    D_CHECK_AL_ERRORS();
    D_THROW(std::runtime_error, "Fail to open the OpenAL device");
  }

  newContext.context = alcCreateContext(newContext.device, NULL);
  if (!alcMakeContextCurrent(newContext.context)) {
    D_CHECK_AL_ERRORS();
    D_THROW(std::runtime_error, "Fail to create the OpenAL context");
  }

  return newContext;
}

void OpenAlContext::close(Context& context) {
  alcDestroyContext(context.context);
  alcCloseDevice(context.device);
  context.device = nullptr;
  context.context = nullptr;
}

void OpenAlContext::setListenerPosition(float x, float y, float z) {
  alListener3f(AL_POSITION, x, y, z);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setListenerPosition(const glm::vec3& pos) {
  setListenerPosition(pos.x, pos.y, pos.z);
}

void OpenAlContext::setListenerOrientation(float frontX, float frontY,
                                           float frontZ, float upX, float upY,
                                           float upZ) {
  std::array<ALfloat, 6> listenerOri = {
    {frontX, frontY, frontZ, upX, upY, upZ}};
  alListenerfv(AL_ORIENTATION, listenerOri.data());
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setListenerOrientation(const glm::vec3& front,
                                           const glm::vec3& up) {
  setListenerOrientation(front.x, front.y, front.z, up.x, up.y, up.z);
}

void OpenAlContext::setListenerVolume(float volume) {
  alListenerf(AL_GAIN, volume); // maximum global volume
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::genSources(uint32_t total, uint32_t* sources) {
  alGenSources((ALsizei)total, sources);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::deleteSources(uint32_t total, const uint32_t* sources) {
  alDeleteSources((ALsizei)total, sources);
  D_CHECK_AL_ERRORS();
}

OpenAlContext::SourceStates OpenAlContext::getSourceState(uint32_t source) {
  ALint state = AL_PLAYING;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  if (state == AL_PLAYING)
    return SourceStates::playing;
  if (state == AL_PAUSED)
    return SourceStates::paused;
  if (state == AL_STOPPED)
    return SourceStates::stopped;
  if (state == AL_INITIAL)
    return SourceStates::initial;
  return SourceStates::initial;
}

void OpenAlContext::stopSource(uint32_t source) {
  alSourceStop(source);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourcePitch(uint32_t source, float pitch) {
  alSourcef(source, AL_PITCH, pitch);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourcePosition(uint32_t source, float x, float y,
                                      float z) {
  alSource3f(source, AL_POSITION, x, y, z);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceVolume(uint32_t source, float volume) {
  alSourcef(source, AL_GAIN, volume);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceMinVolume(uint32_t source, float volume) {
  alSourcef(source, AL_MIN_GAIN, volume); // minimum volume
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceMaxVolume(uint32_t source, float volume) {
  alSourcef(source, AL_MAX_GAIN, volume); // maximum volume
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceDirection(uint32_t source, float x, float y,
                                       float z) {
  alSource3f(source, AL_DIRECTION, x, y, z);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceVelocity(uint32_t source, float x, float y,
                                      float z) {
  alSource3f(source, AL_VELOCITY, x, y, z);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceLooping(uint32_t source, bool isLooping) {
  alSourcei(source, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setSourceBuffer(uint32_t source, uint32_t buffer) {
  alSourcei(source, AL_BUFFER, ALint(buffer));
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::playSource(uint32_t source) {
  alSourcePlay(source);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::genBuffers(uint32_t total, uint32_t* buffers) {
  alGenBuffers((ALsizei)total, buffers);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::deleteBuffers(uint32_t total, const uint32_t* buffers) {
  alDeleteBuffers((ALsizei)total, buffers);
  D_CHECK_AL_ERRORS();
}

void OpenAlContext::setBufferData(uint32_t buffer, BufferFormat format,
                                  const char* soundData, uint32_t size,
                                  std::int32_t sampleRate) {
  switch (format) {
  case BufferFormat::mono8: {
    alBufferData(buffer, AL_FORMAT_MONO8, soundData, (ALsizei)size, sampleRate);
    break;
  }
  case BufferFormat::mono16: {
    alBufferData(buffer, AL_FORMAT_MONO16, soundData, (ALsizei)size,
                 sampleRate);
    break;
  }
  case BufferFormat::stereo8: {
    alBufferData(buffer, AL_FORMAT_STEREO8, soundData, (ALsizei)size,
                 sampleRate);
    break;
  }
  case BufferFormat::stereo16: {
    alBufferData(buffer, AL_FORMAT_STEREO16, soundData, (ALsizei)size,
                 sampleRate);
    break;
  }
  }

  D_CHECK_AL_ERRORS();
}
