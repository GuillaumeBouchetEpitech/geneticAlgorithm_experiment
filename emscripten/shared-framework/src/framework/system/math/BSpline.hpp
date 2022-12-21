
#pragma once

#include <cstdint>
#include <functional>
#include <vector>

class BSpline {
public:
  struct Definition {
    const float* knotsData;
    std::size_t knotsLength;
    uint32_t dimensions;
    uint32_t degree;
  };

public:
  void initialise(const Definition& def);
  float calcAt(float coef, uint32_t dimension) const;

private:
  using BaseFunc = float (*)(float);

  Definition _def;
  BaseFunc _baseFunc = nullptr;
  int32_t _rangeInterval;
};
