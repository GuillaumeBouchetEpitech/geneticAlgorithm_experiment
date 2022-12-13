
#include "framework/math/DeterministicRng.hpp"

#include "framework/TraceLogger.hpp"

#include <unordered_set>

void test_deterministic_rng() {
  D_MYLOG("test_deterministic_rng()");

  {
    DeterministicRng rng;

    constexpr int testSize = 10000;

    std::unordered_set<int64_t> allValues;
    for (int index = 0; index < testSize; ++index) {
      const int64_t value = rng.getValue();

      assert(allValues.count(value) == 0);
      assert(value < rng.getMaxValue());

      allValues.insert(value);
    }

    assert(allValues.size() == testSize);
  }

  {
    DeterministicRng rng;

    constexpr int testSize = 10000;

    for (int index = 0; index < testSize; ++index) {
      const float value = rng.getNormalisedValue();
      assert(value >= 0.0f && value < 1.0f);
    }
  }

  {
    DeterministicRng rng;

    constexpr int testSize = 10000;

    for (int index = 0; index < testSize; ++index) {
      const int valuei = rng.getRangedValue(150, 950);
      assert(valuei >= 150 && valuei <= 950);

      const float valuef = rng.getRangedValue(150.0f, 950.0f);
      assert(valuef >= 150.0f && valuef <= 950.0f);
    }
  }

  D_MYLOG(" => DONE");
}
