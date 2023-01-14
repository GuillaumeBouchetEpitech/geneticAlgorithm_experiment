
#include "headers.hpp"

TEST(static_heap_grid_array, iterators_features) {
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

    int tmpRowIndex = 0;
    for (auto itRow = staticGrid.beginRows(); itRow != staticGrid.endRows();
         ++itRow) {

      EXPECT_EQ(itRow->value, tmpRowIndex + 1);

      int tmpColumnIndex = tmpRowIndex + 1;
      for (auto itColumn = itRow.beginColumns(); itColumn != itRow.endColumns();
           ++itColumn) {
        EXPECT_EQ(itColumn->value, tmpColumnIndex++);
        ++totalItertions;
      }

      tmpRowIndex += int(staticGrid.width());
    }

    EXPECT_EQ(totalItertions, k_width * k_height);
  }

  {
    const auto& cstaticGrid = staticGrid;

    int totalItertions = 0;

    int tmpRowIndex = 0;
    for (auto itRow = cstaticGrid.beginRows(); itRow != cstaticGrid.endRows();
         ++itRow) {

      EXPECT_EQ(itRow->value, tmpRowIndex + 1);

      int tmpColumnIndex = tmpRowIndex + 1;
      for (auto itColumn = itRow.beginColumns(); itColumn != itRow.endColumns();
           ++itColumn) {
        EXPECT_EQ(itColumn->value, tmpColumnIndex++);
        ++totalItertions;
      }

      tmpRowIndex += int(cstaticGrid.width());
    }

    EXPECT_EQ(totalItertions, k_width * k_height);
  }
}
