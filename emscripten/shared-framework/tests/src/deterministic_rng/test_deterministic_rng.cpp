
#include "headers.hpp"

TEST(deterministic_rng, can_generate_10000_unique_random_values) {
  DeterministicRng rng;

  constexpr int testSize = 10000;

  std::unordered_set<int64_t> allValues;
  for (int index = 0; index < testSize; ++index) {
    const int64_t value = rng.getValue();

    EXPECT_EQ(allValues.count(value), 0);
    EXPECT_LT(value, rng.getMaxValue());

    allValues.insert(value);
  }

  EXPECT_EQ(allValues.size(), testSize);
}

TEST(deterministic_rng, can_generate_10000_uniformed_random_values) {
  DeterministicRng rng;

  constexpr int testSize = 10000;

  for (int index = 0; index < testSize; ++index) {
    const float value = rng.getNormalisedValue();

    EXPECT_GE(value, 0.0f);
    EXPECT_LT(value, 1.0f);
  }
}

TEST(deterministic_rng, can_generate_10000_random_but_deterministic_values) {
  constexpr int32_t k_seed = 666;

  DeterministicRng rng;
  rng.setSeed(k_seed);

  constexpr int testSize = 10000;

  std::vector<int64_t> allValues;
  allValues.reserve(testSize);
  for (int index = 0; index < testSize; ++index)
    allValues.push_back(rng.getValue());

  EXPECT_EQ(allValues.size(), testSize);

  rng.setSeed(k_seed);
  for (int index = 0; index < testSize; ++index) {
    EXPECT_EQ(allValues[index], rng.getValue());
  }
}
