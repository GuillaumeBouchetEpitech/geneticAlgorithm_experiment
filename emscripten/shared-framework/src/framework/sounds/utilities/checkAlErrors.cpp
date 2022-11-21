
#include "checkAlErrors.hpp"

#include "framework/ErrorHandler.hpp"

#include <AL/al.h>
#include <AL/alc.h>

void checkAlErrors(const char* filename, const char* funcname, int line)
{
  ALenum error = alGetError();

  if (error == AL_NO_ERROR)
    return;

  D_MYLOG("***ERROR*** (" << filename << ": " << funcname << ": " << line << ")");
  switch (error)
  {
  case AL_INVALID_NAME:
    D_THROW(std::runtime_error, "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
    break;
  case AL_INVALID_ENUM:
    D_THROW(std::runtime_error, "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
    break;
  case AL_INVALID_VALUE:
    D_THROW(std::runtime_error, "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
    break;
  case AL_INVALID_OPERATION:
    D_THROW(std::runtime_error, "AL_INVALID_OPERATION: the requested operation is not valid");
    break;
  case AL_OUT_OF_MEMORY:
    D_THROW(std::runtime_error, "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
    break;
  default:
    D_THROW(std::runtime_error, "UNKNOWN AL ERROR: " << error);
  }
}
