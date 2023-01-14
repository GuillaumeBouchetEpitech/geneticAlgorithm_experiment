
#include "headers.hpp"

TEST(static_heap_grid_array, can_create) {
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
}
