
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>

class PhysicWorld;
class PhysicBody;

class QueryRadius {
  friend PhysicWorld;

private:
  PhysicWorld& _physicWorld;

private:
  QueryRadius(PhysicWorld& physicWorld);
  ~QueryRadius() = default;

public:
  struct QueryRadiusParams {
    glm::vec3 position;
    float radius;

    // TODO: "to ignore" parameter

    short collisionGroup;
    short collisionMask;

    struct ResultRaw {
      bool hasHit = false;

      PhysicBody** allBodiesData;
      std::size_t allBodiesMaxSize;
      std::size_t allBodiesTotal;
    };

    template <std::size_t N> struct ResultArray {
      bool hasHit = false;

      std::array<PhysicBody*, N> allBodiesData;
      std::size_t allBodiesTotal;
    };

    QueryRadiusParams(const glm::vec3& inPosition, float inRadius = 0.0f,
                      short inGroup = -1, short inMask = -1)
      : position(inPosition), radius(inRadius), collisionGroup(inGroup),
        collisionMask(inMask) {}
  };

private:
  bool _queryRadius(QueryRadiusParams& params,
                    QueryRadiusParams::ResultRaw& result);

public:
  template <std::size_t N>
  bool queryRadius(QueryRadiusParams& params,
                   QueryRadiusParams::ResultArray<N>& resultArray) {
    QueryRadiusParams::ResultRaw resultRaw;
    resultRaw.allBodiesData = resultArray.allBodiesData.data();
    resultRaw.allBodiesMaxSize = resultArray.allBodiesData.size();
    resultRaw.allBodiesTotal = 0;
    const bool hasHit = _queryRadius(params, resultRaw);

    resultArray.hasHit = resultRaw.hasHit;
    resultArray.allBodiesTotal = resultRaw.allBodiesTotal;

    return hasHit;
  }
};
