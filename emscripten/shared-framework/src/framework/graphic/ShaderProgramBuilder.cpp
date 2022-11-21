
#include "ShaderProgramBuilder.hpp"

#include "framework/ErrorHandler.hpp"

ShaderProgramBuilder::ShaderProgramBuilder()
{
  reset();
}

//

ShaderProgramBuilder& ShaderProgramBuilder::reset()
{
  _def.filenames.vertex.clear();
  _def.filenames.fragment.clear();
  _def.attributes.clear();
  _def.attributes.reserve(32);
  _def.uniforms.clear();
  _def.uniforms.reserve(32);

  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::setVertexFilename(const std::string& filename)
{
  _def.filenames.vertex = filename;
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::setFragmentFilename(const std::string& filename)
{
  _def.filenames.fragment = filename;
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::addAttribute(const std::string& name)
{
  _def.attributes.push_back(name);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::addUniform(const std::string& name)
{
  _def.uniforms.push_back(name);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::validate()
{
  if (_def.filenames.vertex.empty())
    D_THROW(std::runtime_error, "cannot build without a vertex filename");

  if (_def.filenames.fragment.empty())
    D_THROW(std::runtime_error, "cannot build without a fragment filename");

  if (_def.attributes.empty())
    D_THROW(std::runtime_error, "cannot build without a attributes defined");

  return *this;
}

std::shared_ptr<ShaderProgram> ShaderProgramBuilder::build()
{
  validate();
  return std::make_shared<ShaderProgram>(_def);
}

ShaderProgram::Definition ShaderProgramBuilder::getDefinition()
{
  validate();
  return _def;
}
