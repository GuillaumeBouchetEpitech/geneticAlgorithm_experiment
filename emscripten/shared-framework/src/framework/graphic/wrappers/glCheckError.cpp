
#include "framework/helpers/internals/OpenGLES.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

namespace debug {

void glCheckError(const char* file, unsigned int line, const char* expression) {
  // Get the last error
  GLenum errorCode = glGetError();

  if (errorCode == GL_NO_ERROR)
    return;

  std::string error = "Unknown error";
  std::string description = "No description";

  // Decode the error code
  switch (errorCode) {
  case GL_INVALID_ENUM: {
    error = "GL_INVALID_ENUM";
    description =
      "An unacceptable value has been specified for an enumerated argument.";
    break;
  }

  case GL_INVALID_VALUE: {
    error = "GL_INVALID_VALUE";
    description = "A numeric argument is out of range.";
    break;
  }

  case GL_INVALID_OPERATION: {
    error = "GL_INVALID_OPERATION";
    description =
      "The specified operation is not allowed in the current state.";
    break;
  }

  case GL_STACK_OVERFLOW: {
    error = "GL_STACK_OVERFLOW";
    description = "This command would cause a stack overflow.";
    break;
  }

  case GL_STACK_UNDERFLOW: {
    error = "GL_STACK_UNDERFLOW";
    description = "This command would cause a stack underflow.";
    break;
  }

  case GL_OUT_OF_MEMORY: {
    error = "GL_OUT_OF_MEMORY";
    description = "There is not enough memory left to execute the command.";
    break;
  }

    // case GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION:
    // {
    //   error       = "GL_INVALID_FRAMEBUFFER_OPERATION";
    //   description = "The object bound to FRAMEBUFFER_BINDING is not
    //   \"framebuffer complete\"."; break;
    // }
  }

  D_MYLOG("An internal OpenGL call failed in " << file << "(" << line << ").");
  D_MYLOG("Expression:")
  D_MYLOG(expression);
  D_MYLOG("Error description:");
  D_MYLOG(error);
  D_MYLOG("     " << description);
  // D_THROW(std::runtime_error, "<3");
}

} // namespace debug
