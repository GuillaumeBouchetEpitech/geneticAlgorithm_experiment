
#pragma once

#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "Geometry.hpp"

#include <memory>
#include <unordered_map>

class ResourceManager
{
public:
  ResourceManager() = default;
  ~ResourceManager() = default;

private:
  std::unordered_map<std::string, int> _shaderDefsMap;
  std::unordered_map<int, std::shared_ptr<ShaderProgram>> _shadersMap;
public:
  std::shared_ptr<ShaderProgram> createShader(int aliasCode, const ShaderProgram::Definition def);
  std::shared_ptr<ShaderProgram> getShader(int aliasCode);

private:
  std::unordered_map<std::string, int> _textureDefsMap;
  std::unordered_map<int, std::shared_ptr<Texture>> _texturesMap;
public:
  std::shared_ptr<Texture> createTexture(int aliasCode, const std::string& filename, bool pixelated = false, bool repeat = false);
  std::shared_ptr<Texture> getTexture(int aliasCode);

private:
  std::unordered_map<std::string, int> _geometryDefsMap;
  std::unordered_map<int, Geometry::Definition> _geometriesMap;
public:
  const Geometry::Definition& createGeometryDefinition(int aliasCode, const Geometry::Definition& def);
  const Geometry::Definition& getGeometryDefinition(int aliasCode);

};
