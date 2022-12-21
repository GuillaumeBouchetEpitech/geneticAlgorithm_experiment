
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

    for (std::size_t yy = 0; yy < staticGrid.height(); ++yy)
      for (std::size_t xx = 0; xx < staticGrid.witdh(); ++xx)
        staticGrid(yy, xx).value = int(1 + yy * k_width + xx);

    for (std::size_t ii = 0; ii < staticGrid.size(); ++ii)
      assert(staticGrid[ii].value == int(ii + 1));

    {
      int tmpIndex = 1;
      for (auto& item : staticGrid)
        assert(item.value == tmpIndex++);
    }

    {
      int tmpRowIndex = 0;
      for (auto itRow = staticGrid.beginRow(); itRow != staticGrid.endRow();
           ++itRow) {
        assert(itRow->value == tmpRowIndex + 1);

        int tmpColumnIndex = tmpRowIndex + 1;
        for (auto itColumn = itRow.beginColumns();
             itColumn != itRow.endColumns(); ++itColumn)
          assert(itColumn->value == tmpColumnIndex++);

        tmpRowIndex += int(staticGrid.witdh());
      }
    }

    {
      for (std::size_t row = 0; row < staticGrid.height(); ++row) {
        int tmpColumnIndex = int(row * staticGrid.witdh()) + 1;
        for (auto itColumn = staticGrid.beginColumns(row);
             itColumn != staticGrid.endColumns(row); ++itColumn)
          assert(itColumn->value == tmpColumnIndex++);
      }
    }
  }

  D_MYLOG(" => DONE");
}
