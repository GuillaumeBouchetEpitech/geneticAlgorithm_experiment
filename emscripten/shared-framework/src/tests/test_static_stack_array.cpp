
#include "framework/containers/static_stack_array.hpp"

#include "framework/TraceLogger.hpp"

#include "common.hpp"

#include <vector>
#include <functional>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

// template<typename T>
// using empty_heap_array = dynamic_heap_array<T, std::allocator<T>, 0>;


void test_static_stack_array()
{
  D_MYLOG("test_static_stack_array()");

  {

    static_stack_array<int, 5> myArray;

    for (int ii = 0; ii < int(myArray.size()); ++ii)
      myArray[ii] = ii;

    for (int ii = 0; ii < int(myArray.size()); ++ii)
      assert(myArray[ii] == ii);

    {
      int index = 0;
      for (auto it = myArray.begin(); it != myArray.end(); ++it)
        assert(*it == index++);
      assert(index == myArray.size());
    }

    {
      int index = 0;
      const auto& cmyArray = myArray;
      for (auto it = cmyArray.begin(); it != cmyArray.end(); ++it)
        assert(*it == index++);
      assert(index == myArray.size());
    }

    {
      int index = 0;
      for (int value : myArray)
        assert(value == index++);
      assert(index == myArray.size());
    }

    {
      int index = 0;
      const auto& cmyArray = myArray;
      for (const int value : cmyArray)
        assert(value == index++);
      assert(index == myArray.size());
    }

    {
      int index = 0;
      for (auto it = myArray.begin(); it != myArray.end(); ++it)
        assert(*it == index++);
      assert(index == myArray.size());
    }

    {
      int index = 0;
      for (int value : myArray)
        assert(value == index++);
      assert(index == myArray.size());
    }

    assert(myArray[-5] == 0);
    assert(myArray[-4] == 1);
    assert(myArray[-3] == 2);
    assert(myArray[-2] == 3);
    assert(myArray[-1] == 4);

    assert(myArray[0] == 0);
    assert(myArray[1] == 1);
    assert(myArray[2] == 2);
    assert(myArray[3] == 3);
    assert(myArray[4] == 4);

    assert(myArray[5] == 0);
    assert(myArray[6] == 1);
    assert(myArray[7] == 2);
    assert(myArray[8] == 3);
    assert(myArray[9] == 4);

  }

  D_MYLOG(" => DONE");
}

