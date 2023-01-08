
#include "framework/system//containers/static_heap_grid_array.hpp"

#include "framework/system/TraceLogger.hpp"

#include "common.hpp"

#include <functional>
#include <vector>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

void test_static_heap_grid_array() {
  D_MYLOG("test_static_heap_grid_array()");

  {

    constexpr std::size_t k_width = 6;
    constexpr std::size_t k_height = 4;

    static_heap_grid_array<common::Test> staticGrid;
    staticGrid.allocate(k_height, k_width);

    for (int yy = 0; yy < int(staticGrid.height()); ++yy)
      for (int xx = 0; xx < int(staticGrid.witdh()); ++xx)
        staticGrid(yy, xx).value = 1 + yy * int(k_width) + xx;

    for (int ii = 0; ii < int(staticGrid.size()); ++ii) {
      assert(staticGrid[ii].value == ii + 1);
      assert(staticGrid[ii + int(staticGrid.size())].value == ii + 1);
      assert(staticGrid[ii - int(staticGrid.size())].value == ii + 1);
    }

    for (int yy = 0; yy < int(staticGrid.height()); ++yy)
      for (int xx = 0; xx < int(staticGrid.witdh()); ++xx) {
        const int expectedVal = 1 + yy * int(k_width) + xx;

        assert(staticGrid(yy, xx).value == expectedVal);

        for (int stepY = -1; stepY <= +1; ++stepY)
          for (int stepX = -1; stepX <= +1; ++stepX)
            assert(staticGrid(yy + stepY * int(staticGrid.height()),
                              xx + stepX * int(staticGrid.witdh()))
                     .value == expectedVal);
      }

    // {
    //   int tmpIndex = 1;
    //   for (auto& item : staticGrid)
    //     assert(item.value == tmpIndex++);
    // }

    {
      int tmpRowIndex = 0;
      for (auto itRow = staticGrid.beginRows(); itRow != staticGrid.endRows();
           ++itRow) {

        assert(itRow->value == tmpRowIndex + 1);

        int tmpColumnIndex = tmpRowIndex + 1;
        for (auto itColumn = itRow.beginColumns();
             itColumn != itRow.endColumns(); ++itColumn)
          assert(itColumn->value == tmpColumnIndex++);

        tmpRowIndex += int(staticGrid.witdh());
      }
    }

    // {
    //   for (std::size_t row = 0; row < staticGrid.height(); ++row) {
    //     int tmpColumnIndex = int(row * staticGrid.witdh()) + 1;
    //     for (auto itColumn = staticGrid.beginColumns(row);
    //          itColumn != staticGrid.endColumns(row); ++itColumn)
    //       assert(itColumn->value == tmpColumnIndex++);
    //   }
    // }
  }

  D_MYLOG(" => DONE");
}
