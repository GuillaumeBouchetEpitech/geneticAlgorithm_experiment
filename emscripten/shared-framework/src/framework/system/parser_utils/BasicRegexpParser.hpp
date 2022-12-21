
#pragma once

#include "framework/system/getStreamStr.hpp"

#include "framework/helpers/GLMath.hpp"

#include <regex>

class BasicRegexpParser {
private:
  struct Matchers {
    const std::string matchStrName = R"(([\w\-]+))";
    const std::string matchStrFileName = R"(([\w\-\.\/]+))";
    const std::string matchStrValue = R"(\"(.+?)\")";
    const std::string matchMaybeSpace = R"(\s*?)";
    const std::string matchMain = D_SSTR(matchStrName << "=" << matchStrValue);
    const std::string match1UI = R"(([+-]?\d+))";
    const std::string match1F = R"(([+-]?\d+(?:\.\d+)?))";
    const std::string matchS1UI =
      D_SSTR(matchMaybeSpace << match1UI << matchMaybeSpace);
    const std::string matchS2UI = D_SSTR(matchS1UI << "," << matchS1UI);
    const std::string matchS3UI =
      D_SSTR(matchS1UI << "," << matchS1UI << "," << matchS1UI);
    const std::string matchS1F =
      D_SSTR(matchMaybeSpace << match1F << matchMaybeSpace);
    const std::string matchS2F = D_SSTR(matchS1F << "," << matchS1F);
    const std::string matchS3F =
      D_SSTR(matchS1F << "," << matchS1F << "," << matchS1F);
    const std::string matchS4F =
      D_SSTR(matchS1F << "," << matchS1F << "," << matchS1F << "," << matchS1F);
  } _matchers;

  struct Regexps {
    std::regex regexpMain;
    std::regex regexpName;
    std::regex regexpFileName;
    std::regex regexpS1UI;
    std::regex regexpS2UI;
    std::regex regexpS3UI;
    std::regex regexpS1F;
    std::regex regexpS2F;
    std::regex regexpS3F;
    std::regex regexpS4F;
  } _regexps;

  std::string _errorHint;

public:
  BasicRegexpParser();

  void setErrorHint(const std::string& errorHint);

  std::string getName(const std::string& toMatch);
  std::string getFileName(const std::string& toMatch);

  uint32_t get1UI(const std::string& toMatch);
  uint32_t get1UI(const std::string& toMatch, uint32_t maxValue);

  glm::uvec2 get2UI(const std::string& toMatch);
  glm::uvec2 get2UI(const std::string& toMatch, uint32_t maxValue);

  glm::uvec3 get3UI(const std::string& toMatch);
  glm::uvec3 get3UI(const std::string& toMatch, uint32_t maxValue);

  float get1F(const std::string& toMatch);
  float get1F(const std::string& toMatch, float minValue, float maxValue);

  glm::vec2 get2F(const std::string& toMatch);
  glm::vec2 get2F(const std::string& toMatch, float minValue, float maxValue);

  glm::vec3 get3F(const std::string& toMatch);
  glm::vec3 get3F(const std::string& toMatch, float minValue, float maxValue);

  glm::vec4 get4F(const std::string& toMatch);
  glm::vec4 get4F(const std::string& toMatch, float minValue, float maxValue);

  void forEachArgs(const std::string& content,
                   const std::function<void(const std::string&,
                                            const std::string&)>& callback);

private:
  void _validateFloat(float value);
  float _getValidFloat(const std::string str);
};
