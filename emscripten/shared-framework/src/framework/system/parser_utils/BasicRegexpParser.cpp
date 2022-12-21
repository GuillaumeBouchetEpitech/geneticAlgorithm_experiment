
#include "BasicRegexpParser.hpp"

#include "framework/system/ErrorHandler.hpp"

BasicRegexpParser::BasicRegexpParser() {
  _regexps.regexpMain = _matchers.matchMain;
  _regexps.regexpName = _matchers.matchStrName;
  _regexps.regexpFileName = _matchers.matchStrFileName;
  _regexps.regexpS1UI = _matchers.matchS1UI;
  _regexps.regexpS2UI = _matchers.matchS2UI;
  _regexps.regexpS3UI = _matchers.matchS3UI;
  _regexps.regexpS1F = _matchers.match1F;
  _regexps.regexpS2F = _matchers.matchS2F;
  _regexps.regexpS3F = _matchers.matchS3F;
  _regexps.regexpS4F = _matchers.matchS4F;
}

void BasicRegexpParser::setErrorHint(const std::string& errorHint) {
  _errorHint = errorHint;
}

std::string BasicRegexpParser::getName(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpName);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse name, type=" << _errorHint);

  return subMatch[1].str();
}

std::string BasicRegexpParser::getFileName(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpFileName);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse file name, type=" << _errorHint);

  return subMatch[1].str();
}

uint32_t BasicRegexpParser::get1UI(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS1UI);
  if (subMatch.empty())
    D_THROW(std::runtime_error,
            "cannot parse 1UI, type=" << _errorHint << ", toMatch=" << toMatch);

  return uint32_t(std::atoi(subMatch[1].str().c_str()));
}

uint32_t BasicRegexpParser::get1UI(const std::string& toMatch,
                                   uint32_t maxValue) {
  const uint32_t value = get1UI(toMatch);
  if (value > maxValue)
    D_THROW(std::runtime_error, "value of 1UI is too high, type="
                                  << _errorHint << ", value=" << value
                                  << ", maxValue=" << maxValue);
  return value;
}

glm::uvec2 BasicRegexpParser::get2UI(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS2UI);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 2UI, type=" << _errorHint);

  return {uint32_t(std::atoi(subMatch[1].str().c_str())),
          uint32_t(std::atoi(subMatch[2].str().c_str()))};
}

glm::uvec2 BasicRegexpParser::get2UI(const std::string& toMatch,
                                     uint32_t maxValue) {
  const glm::uvec2 value = get2UI(toMatch);
  if (value.x > maxValue)
    D_THROW(std::runtime_error, "value.x of 2UI is too high, type="
                                  << _errorHint << ", value=" << value.x
                                  << ", maxValue=" << maxValue);
  if (value.y > maxValue)
    D_THROW(std::runtime_error, "value.y of 2UI is too high, type="
                                  << _errorHint << ", value=" << value.y
                                  << ", maxValue=" << maxValue);
  return value;
}

glm::uvec3 BasicRegexpParser::get3UI(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS3UI);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 3UI, type=" << _errorHint);

  return {uint32_t(std::atoi(subMatch[1].str().c_str())),
          uint32_t(std::atoi(subMatch[2].str().c_str())),
          uint32_t(std::atoi(subMatch[3].str().c_str()))};
}

glm::uvec3 BasicRegexpParser::get3UI(const std::string& toMatch,
                                     uint32_t maxValue) {
  const glm::uvec3 value = get3UI(toMatch);
  if (value.x > maxValue)
    D_THROW(std::runtime_error, "value.x of 2UI is too high, type="
                                  << _errorHint << ", value=" << value.x
                                  << ", maxValue=" << maxValue);
  if (value.y > maxValue)
    D_THROW(std::runtime_error, "value.y of 2UI is too high, type="
                                  << _errorHint << ", value=" << value.y
                                  << ", maxValue=" << maxValue);
  if (value.z > maxValue)
    D_THROW(std::runtime_error, "value.z of 2UI is too high, type="
                                  << _errorHint << ", value=" << value.z
                                  << ", maxValue=" << maxValue);
  return value;
}

float BasicRegexpParser::get1F(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS1F);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 1F, type=" << _errorHint);

  return _getValidFloat(subMatch[1].str());
}

float BasicRegexpParser::get1F(const std::string& toMatch, float minValue,
                               float maxValue) {
  float value = get1F(toMatch);

  if (value < minValue)
    D_THROW(std::runtime_error,
            "value of 1F is too low, type=" << _errorHint << ", value=" << value
                                            << ", minValue=" << minValue);
  if (value > maxValue)
    D_THROW(std::runtime_error, "value of 1F is too high, type="
                                  << _errorHint << ", value=" << value
                                  << ", maxValue=" << maxValue);

  return value;
}

glm::vec2 BasicRegexpParser::get2F(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS2F);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 2F, type=" << _errorHint);

  return {_getValidFloat(subMatch[1].str()), _getValidFloat(subMatch[2].str())};
}

glm::vec2 BasicRegexpParser::get2F(const std::string& toMatch, float minValue,
                                   float maxValue) {
  glm::vec2 value = get2F(toMatch);

  if (value.x < minValue)
    D_THROW(std::runtime_error, "value.x of 2F is too low, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", minValue=" << minValue);
  if (value.x > maxValue)
    D_THROW(std::runtime_error, "value.x of 2F is too high, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", maxValue=" << maxValue);

  if (value.y < minValue)
    D_THROW(std::runtime_error, "value.y of 2F is too low, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", minValue=" << minValue);
  if (value.y > maxValue)
    D_THROW(std::runtime_error, "value.y of 2F is too high, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", maxValue=" << maxValue);

  return value;
}

glm::vec3 BasicRegexpParser::get3F(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS3F);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 3F, type=" << _errorHint);

  return {_getValidFloat(subMatch[1].str()), _getValidFloat(subMatch[2].str()),
          _getValidFloat(subMatch[3].str())};
}

glm::vec3 BasicRegexpParser::get3F(const std::string& toMatch, float minValue,
                                   float maxValue) {
  glm::vec3 value = get3F(toMatch);

  if (value.x < minValue)
    D_THROW(std::runtime_error, "value.x of 3F is too low, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", minValue=" << minValue);
  if (value.x > maxValue)
    D_THROW(std::runtime_error, "value.x of 3F is too high, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", maxValue=" << maxValue);

  if (value.y < minValue)
    D_THROW(std::runtime_error, "value.y of 3F is too low, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", minValue=" << minValue);
  if (value.y > maxValue)
    D_THROW(std::runtime_error, "value.y of 3F is too high, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", maxValue=" << maxValue);

  if (value.z < minValue)
    D_THROW(std::runtime_error, "value.z of 3F is too low, type="
                                  << _errorHint << ", value.z=" << value.z
                                  << ", minValue=" << minValue);
  if (value.z > maxValue)
    D_THROW(std::runtime_error, "value.z of 3F is too high, type="
                                  << _errorHint << ", value.z=" << value.z
                                  << ", maxValue=" << maxValue);

  return value;
}

glm::vec4 BasicRegexpParser::get4F(const std::string& toMatch) {
  std::smatch subMatch;
  std::regex_search(toMatch, subMatch, _regexps.regexpS4F);
  if (subMatch.empty())
    D_THROW(std::runtime_error, "cannot parse 4F, type=" << _errorHint);

  return {_getValidFloat(subMatch[1].str()), _getValidFloat(subMatch[2].str()),
          _getValidFloat(subMatch[3].str()), _getValidFloat(subMatch[4].str())};
}

glm::vec4 BasicRegexpParser::get4F(const std::string& toMatch, float minValue,
                                   float maxValue) {
  glm::vec4 value = get4F(toMatch);

  if (value.x < minValue)
    D_THROW(std::runtime_error, "value.x of 4F is too low, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", minValue=" << minValue);
  if (value.x > maxValue)
    D_THROW(std::runtime_error, "value.x of 4F is too high, type="
                                  << _errorHint << ", value.x=" << value.x
                                  << ", maxValue=" << maxValue);

  if (value.y < minValue)
    D_THROW(std::runtime_error, "value.y of 4F is too low, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", minValue=" << minValue);
  if (value.y > maxValue)
    D_THROW(std::runtime_error, "value.y of 4F is too high, type="
                                  << _errorHint << ", value.y=" << value.y
                                  << ", maxValue=" << maxValue);

  if (value.z < minValue)
    D_THROW(std::runtime_error, "value.z of 4F is too low, type="
                                  << _errorHint << ", value.z=" << value.z
                                  << ", minValue=" << minValue);
  if (value.z > maxValue)
    D_THROW(std::runtime_error, "value.z of 4F is too high, type="
                                  << _errorHint << ", value.z=" << value.z
                                  << ", maxValue=" << maxValue);

  if (value.w < minValue)
    D_THROW(std::runtime_error, "value.w of 4F is too low, type="
                                  << _errorHint << ", value.w=" << value.w
                                  << ", minValue=" << minValue);
  if (value.w > maxValue)
    D_THROW(std::runtime_error, "value.w of 4F is too high, type="
                                  << _errorHint << ", value.w=" << value.w
                                  << ", maxValue=" << maxValue);

  return value;
}

void BasicRegexpParser::forEachArgs(
  const std::string& content,
  const std::function<void(const std::string&, const std::string&)>& callback) {
  std::smatch match;
  std::string::const_iterator searchStart(content.cbegin());
  while (std::regex_search(searchStart, content.cend(), match,
                           _regexps.regexpMain)) {
    std::string key = match[1].str();
    std::string value = match[2].str();

    callback(key, value);

    searchStart = match.suffix().first;
  }
}

void BasicRegexpParser::_validateFloat(float value) {
  if (glm::isnan(value))
    D_THROW(std::runtime_error, "invalid value (NaN), type=" << _errorHint);

  if (glm::isinf(value))
    D_THROW(std::runtime_error, "invalid value (inf), type=" << _errorHint);
}

float BasicRegexpParser::_getValidFloat(const std::string str) {
  const float value = float(std::atof(str.c_str()));
  _validateFloat(value);
  return value;
}
