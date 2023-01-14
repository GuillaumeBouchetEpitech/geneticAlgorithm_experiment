
#include "headers.hpp"

TEST(static_heap_grid_array, can_set_and_get_values) {
  constexpr std::size_t k_width = 6;
  constexpr std::size_t k_height = 4;

  static_heap_grid_array<common::TestStructure> staticGrid;

  EXPECT_EQ(staticGrid.width(), 0);
  EXPECT_EQ(staticGrid.height(), 0);
  EXPECT_EQ(staticGrid.size(), 0);
  EXPECT_EQ(staticGrid.is_empty(), true);

  staticGrid.allocate(k_height, k_width);

  EXPECT_EQ(staticGrid.width(), k_width);
  EXPECT_EQ(staticGrid.height(), k_height);
  EXPECT_EQ(staticGrid.size(), k_width * k_height);
  EXPECT_EQ(staticGrid.is_empty(), false);

  for (int yy = 0; yy < int(staticGrid.height()); ++yy)
    for (int xx = 0; xx < int(staticGrid.width()); ++xx)
      staticGrid(yy, xx).value = 1 + yy * int(k_width) + xx;

  for (int ii = 0; ii < int(staticGrid.size()); ++ii) {
    EXPECT_EQ(staticGrid[ii].value, ii + 1);
    EXPECT_EQ(staticGrid[ii + int(staticGrid.size())].value, ii + 1);
    EXPECT_EQ(staticGrid[ii - int(staticGrid.size())].value, ii + 1);
  }

  const auto& cstaticGrid = staticGrid;

  for (int ii = 0; ii < int(cstaticGrid.size()); ++ii) {
    EXPECT_EQ(cstaticGrid[ii].value, ii + 1);
    EXPECT_EQ(cstaticGrid[ii + int(cstaticGrid.size())].value, ii + 1);
    EXPECT_EQ(cstaticGrid[ii - int(cstaticGrid.size())].value, ii + 1);
  }
}

TEST(static_heap_grid_array, can_set_and_get_off_limits_values) {
  constexpr std::size_t k_width = 6;
  constexpr std::size_t k_height = 4;

  static_heap_grid_array<common::TestStructure> staticGrid;

  EXPECT_EQ(staticGrid.width(), 0);
  EXPECT_EQ(staticGrid.height(), 0);
  EXPECT_EQ(staticGrid.size(), 0);
  EXPECT_EQ(staticGrid.is_empty(), true);

  staticGrid.allocate(k_height, k_width);

  EXPECT_EQ(staticGrid.width(), k_width);
  EXPECT_EQ(staticGrid.height(), k_height);
  EXPECT_EQ(staticGrid.size(), k_width * k_height);
  EXPECT_EQ(staticGrid.is_empty(), false);

  for (int yy = 0; yy < int(staticGrid.height()); ++yy)
    for (int xx = 0; xx < int(staticGrid.width()); ++xx)
      staticGrid(yy, xx).value = 1 + yy * int(k_width) + xx;

  {
    int totalItertions = 0;
    for (int yy = 0; yy < int(staticGrid.height()); ++yy) {
      for (int xx = 0; xx < int(staticGrid.width()); ++xx) {
        const int expectedVal = 1 + yy * int(k_width) + xx;

        EXPECT_EQ(staticGrid(yy, xx).value, expectedVal);

        for (int stepY = -1; stepY <= +1; ++stepY) {
          for (int stepX = -1; stepX <= +1; ++stepX) {
            const int indexX = xx + stepX * int(staticGrid.width());
            const int indexY = yy + stepY * int(staticGrid.height());
            EXPECT_EQ(staticGrid(indexY, indexX).value, expectedVal);

            ++totalItertions;
          }
        }
      }
    }

    EXPECT_EQ(totalItertions, k_width * k_height * 9);
  }

  {
    const auto& cstaticGrid = staticGrid;

    int totalItertions = 0;
    for (int yy = 0; yy < int(cstaticGrid.height()); ++yy) {
      for (int xx = 0; xx < int(cstaticGrid.width()); ++xx) {
        const int expectedVal = 1 + yy * int(k_width) + xx;

        EXPECT_EQ(cstaticGrid(yy, xx).value, expectedVal);

        for (int stepY = -1; stepY <= +1; ++stepY) {
          for (int stepX = -1; stepX <= +1; ++stepX) {
            const int indexX = xx + stepX * int(cstaticGrid.width());
            const int indexY = yy + stepY * int(cstaticGrid.height());
            EXPECT_EQ(cstaticGrid(indexY, indexX).value, expectedVal);

            ++totalItertions;
          }
        }
      }
    }

    EXPECT_EQ(totalItertions, k_width * k_height * 9);
  }
}
