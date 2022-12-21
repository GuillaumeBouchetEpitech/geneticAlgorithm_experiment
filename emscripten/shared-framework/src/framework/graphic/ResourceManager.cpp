
#include "ResourceManager.hpp"

#include "Image.hpp"

#include "framework/system/asValue.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

#include <sstream>

std::shared_ptr<ShaderProgram>
ResourceManager::createShader(int aliasCode,
                              const ShaderProgram::Definition def) {
  std::stringstream sstr;
  sstr << def.filenames.vertex << "-" << def.filenames.fragment;
  std::string shaderUniqueName = sstr.str();

  auto itDef = _shaderDefsMap.find(shaderUniqueName);
  if (itDef != _shaderDefsMap.end()) {
    auto itShader = _shadersMap.find(itDef->second);
    if (itShader == _shadersMap.end())
      D_THROW(std::runtime_error, "resource manager shader map corrupted");

    _shadersMap[aliasCode] = itShader->second; // save same under new alias
    return itShader->second;
  }

  if (_shadersMap.count(aliasCode) > 0)
    D_THROW(std::runtime_error,
            "resource manager new shader alias is duplicated, aliasCode="
              << aliasCode);

  auto newShader = std::make_shared<ShaderProgram>(def, _fileManager);
  _shadersMap[aliasCode] = newShader;
  _shaderDefsMap[shaderUniqueName] = aliasCode;
  return newShader;
}

std::shared_ptr<ShaderProgram> ResourceManager::getShader(int aliasCode) {
  auto it = _shadersMap.find(aliasCode);
  if (it == _shadersMap.end())
    D_THROW(std::runtime_error,
            "resource manager shader does not exist, aliasCode=" << aliasCode);

  return it->second;
}

//
//
//

std::shared_ptr<Texture> ResourceManager::createTexture(
  int aliasCode, const std::string& filename,
  Texture::Quality quality /*= Texture::Quality::pixelated*/,
  Texture::Pattern pattern /*= Texture::Pattern::clamped*/) {
  std::stringstream sstr;
  sstr << filename << "-quality=" << asValue(quality)
       << "-repeat=" << asValue(pattern);
  std::string textureUniqueName = sstr.str();

  auto itDef = _textureDefsMap.find(textureUniqueName);
  if (itDef != _textureDefsMap.end()) {
    auto itTexture = _texturesMap.find(itDef->second);
    if (itTexture == _texturesMap.end())
      D_THROW(std::runtime_error, "resource manager texture map corrupted");

    _texturesMap[aliasCode] = itTexture->second; // save same under new alias
    return itTexture->second;
  }

  if (_texturesMap.count(aliasCode) > 0)
    D_THROW(std::runtime_error,
            "resource manager new texture alias is duplicated, aliasCode="
              << aliasCode);

  auto newTexture = std::make_shared<Texture>();

  Image tmpImg;
  tmpImg.loadFromFile(_fileManager, filename);
  newTexture->setFromImage(tmpImg, quality, pattern);

  _texturesMap[aliasCode] = newTexture;
  _textureDefsMap[textureUniqueName] = aliasCode;
  return newTexture;
}

std::shared_ptr<Texture> ResourceManager::getTexture(int aliasCode) {
  auto it = _texturesMap.find(aliasCode);
  if (it == _texturesMap.end())
    D_THROW(std::runtime_error,
            "resource manager texture does not exist, aliasCode=" << aliasCode);

  return it->second;
}

//
//
//

const Geometry::Definition&
ResourceManager::createGeometryDefinition(int aliasCode,
                                          const Geometry::Definition& def) {
  std::stringstream sstr;

  sstr << asValue(def.primitiveType) << "-" << def.vbos.size();
  for (const auto& vbo : def.vbos) {
    sstr << "-" << vbo.instanced;
    sstr << "-" << vbo.stride;
    sstr << "-" << vbo.attrs.size();
    for (const auto& attr : vbo.attrs) {
      sstr << "-" << attr.index;
      sstr << "-" << attr.name;
      sstr << "-" << asValue(attr.type);
    }
  }

  std::string geoDefUniqueName = sstr.str();

  auto itDef = _geometryDefsMap.find(geoDefUniqueName);
  if (itDef != _geometryDefsMap.end()) {
    auto itGeoDef = _geometriesMap.find(itDef->second);
    if (itGeoDef == _geometriesMap.end())
      D_THROW(std::runtime_error,
              "resource manager geometry definition map corrupted");

    _geometriesMap[aliasCode] = itGeoDef->second; // save same under new alias
    return itGeoDef->second;
  }

  if (_geometriesMap.count(aliasCode) > 0)
    D_THROW(
      std::runtime_error,
      "resource manager new geometry definition alias is duplicated, aliasCode="
        << aliasCode);

  _geometriesMap[aliasCode] = def; // hard copy
  _geometryDefsMap[geoDefUniqueName] = aliasCode;
  return def;
}

const Geometry::Definition&
ResourceManager::getGeometryDefinition(int aliasCode) {
  auto it = _geometriesMap.find(aliasCode);
  if (it == _geometriesMap.end())
    D_THROW(std::runtime_error,
            "resource manager geometry definition does not exist, aliasCode="
              << aliasCode);

  return it->second;
}
