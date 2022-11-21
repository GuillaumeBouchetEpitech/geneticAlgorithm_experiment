
#include "ShaderProgram.hpp"

#include "GlContext.hpp"

#include "framework/files/getFileContent.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/ErrorHandler.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept> // <= std::invalid_argument / runtime_error
#include <memory> // <= std::unique_ptr / make_unique
#include <regex>

namespace utilities
{
  bool getShaderSource(const std::string& filename, std::string& source)
  {
    std::string subSource;
    if (!FileUtils::getFileContent(filename, subSource))
      return false;

    std::vector<std::string> newLines;
    newLines.reserve(512);

    std::stringstream sstr;
    sstr << subSource;

    // D_MYLOG("##############################");
    // D_MYLOG("filename=" << filename);

    std::regex inlcudeRegexp(R"(^\s*#\s*include\s+\"(\S+)\")");

    std::string currentLine;
    while (std::getline(sstr, currentLine))
    {
      // D_MYLOG("currentLine=" << currentLine);

      std::smatch match;
      std::regex_search(currentLine, match, inlcudeRegexp);

      if (match.size() >= 2)
      {
        std::string includeFilename = match[1].str();

        std::string includeSource;
        if (!FileUtils::getFileContent(includeFilename, includeSource))
          return false;

        newLines.push_back(includeSource);
      }
      else
      {
        newLines.push_back(currentLine);
      }
    }

    source = sstr.str();

    std::stringstream sstrResult;
    for (auto& line : newLines)
      sstrResult << line << std::endl;
    source = sstrResult.str();

    return true;
  }

}

//

ShaderProgram::ShaderProgram(const Definition& def)
{
  std::string vertexSourceCode;
  std::string fragmentSourceCode;

  if (!utilities::getShaderSource(def.filenames.vertex, vertexSourceCode))
    D_THROW(std::invalid_argument, "fail to read a file"
            << ", source=" << def.filenames.vertex);

  if (!utilities::getShaderSource(def.filenames.fragment, fragmentSourceCode))
    D_THROW(std::invalid_argument, "fail to read a file"
            << ", source=" << def.filenames.fragment);

  //


  unsigned int vertexShader = GlContext::loadVertexShader(vertexSourceCode);
  unsigned int fragmentShader = GlContext::loadFragmentShader(fragmentSourceCode);

  _programId = GlContext::createProgram();

  if (!_programId)
    D_THROW(std::invalid_argument, "fail to create a shader program"
            << ", sources: vertex=" << def.filenames.vertex
            << ", fragment=" << def.filenames.fragment);

  if (!GlContext::linkProgram(_programId, vertexShader, fragmentShader))
  {
    GlContext::printProgramInfo(_programId);

    GlContext::deleteProgram(_programId);

    D_THROW(std::runtime_error, "fail to link a shader"
            << ", sources: vertex=" << def.filenames.vertex
            << ", fragment=" << def.filenames.fragment);
  }

  for (const auto& attribute : def.attributes)
  {
    if (_attributesMap.count(attribute) > 0)
      D_THROW(std::runtime_error, "duplicate attribute"
              << ", input=" << attribute
              << ", sources: vertex=" << def.filenames.vertex
              << ", fragment=" << def.filenames.fragment);

    int location = GlContext::getAttribLocation(_programId, attribute.c_str());

    if (location == -1)
      D_THROW(std::runtime_error, "fail to get an attribute location"
              << ", input=" << attribute
              << ", sources: vertex=" << def.filenames.vertex
              << ", fragment=" << def.filenames.fragment);

    _attributesMap[attribute] = location;
  }

  for (const auto& uniform : def.uniforms)
  {
    if (_uniformsMap.count(uniform) > 0)
      D_THROW(std::runtime_error, "duplicate uniform"
              << ", input=" << uniform
              << ", sources: vertex=" << def.filenames.vertex
              << ", fragment=" << def.filenames.fragment);

    int location = GlContext::getUniformLocation(_programId, uniform.c_str());

    if (location == -1)
      D_THROW(std::runtime_error, "fail to get an uniform location"
              << ", input=" << uniform
              << ", sources: vertex=" << def.filenames.vertex
              << ", fragment=" << def.filenames.fragment);

    _uniformsMap[uniform] = location;
  }
}

ShaderProgram::~ShaderProgram()
{
  if (_programId)
    GlContext::deleteProgram(_programId);
}

//

void ShaderProgram::bind() const
{
  if (!_programId)
    D_THROW(std::runtime_error, "shader not initialised");

  GlContext::useProgram(_programId);
}

void ShaderProgram::unbind()
{
  GlContext::useProgram(0);
}

//

int ShaderProgram::getAttribute(const char* name) const
{
  auto it = _attributesMap.find(name);

  if (it == _attributesMap.end())
    D_THROW(std::invalid_argument, "unknown attribute, input=" << name);

  return it->second;
}

int ShaderProgram::getUniform(const char* name) const
{
  auto it = _uniformsMap.find(name);

  if (it == _uniformsMap.end())
    D_THROW(std::invalid_argument, "unknown uniform, input=" << name);

  return it->second;
}

bool ShaderProgram::hasAttribute(const char* name) const
{
  return (_attributesMap.find(name) != _attributesMap.end());
}

bool ShaderProgram::hasUniform(const char* name) const
{
  return (_uniformsMap.find(name) != _uniformsMap.end());
}

//

void ShaderProgram::setUniform(const char* name, float value) const
{
  setUniform(getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, float x, float y, float z, float w) const
{
  setUniform(getUniform(name), x, y, z, w);
}

void ShaderProgram::setUniform(const char* name, const glm::vec4& vec4) const
{
  setUniform(getUniform(name), vec4.x, vec4.y, vec4.z, vec4.w);
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& mat4) const
{
  setUniform(getUniform(name), mat4);
}

//

void ShaderProgram::setUniform(int location, float value) const
{
  GlContext::setUniform(location, value);
}

void ShaderProgram::setUniform(int location, float x, float y, float z, float w) const
{
  GlContext::setUniform(location, x, y, z, w);
}

void ShaderProgram::setUniform(int location, const glm::vec4& vec4) const
{
  GlContext::setUniform(location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void ShaderProgram::setUniform(int location, const glm::mat4& mat4) const
{
  GlContext::setUniform(location, mat4);
}
