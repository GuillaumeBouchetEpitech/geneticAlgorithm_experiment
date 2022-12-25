
#include "framework/system//containers/static_array.hpp"

#include "framework/system/TraceLogger.hpp"

#include "common.hpp"

#include <functional>
#include <vector>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

// template<typename T>
// using empty_heap_array = dynamic_heap_array<T, std::allocator<T>, 0>;

void test_static_array() {
  D_MYLOG("test_static_array()");

  {

    static_array<int, 5> myArray;

    for (int ii = 0; ii < int(myArray.size()); ++ii)
      myArray[ii] = ii;

    for (int ii = 0; ii < int(myArray.size()); ++ii)
      assert(myArray[ii] == ii);

    {
      int index = 0;
      for (auto it = myArray.begin(); it != myArray.end(); ++it)
        assert(*it == index++);
      assert(index == int(myArray.size()));
    }

    {
      int index = 0;
      const auto& cmyArray = myArray;
      for (auto it = cmyArray.begin(); it != cmyArray.end(); ++it)
        assert(*it == index++);
      assert(index == int(myArray.size()));
    }

    {
      int index = 0;
      for (int value : myArray)
        assert(value == index++);
      assert(index == int(myArray.size()));
    }

    {
      int index = 0;
      const auto& cmyArray = myArray;
      for (const int value : cmyArray)
        assert(value == index++);
      assert(index == int(myArray.size()));
    }

    {
      int index = 0;
      for (auto it = myArray.begin(); it != myArray.end(); ++it)
        assert(*it == index++);
      assert(index == int(myArray.size()));
    }

    {
      int index = 0;
      for (int value : myArray)
        assert(value == index++);
      assert(index == int(myArray.size()));
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

  {
    static_array<int, 5> myArray;

    assert(myArray.total_iterators() == 0);

    {
      auto it1 = myArray.begin();

      assert(myArray.total_iterators() == 1);
    }

    assert(myArray.total_iterators() == 0);

    {
      auto it1 = myArray.begin();
      auto it2 = myArray.end();

      const auto& cmyArray = myArray;
      auto it3 = cmyArray.begin();
      auto it4 = cmyArray.end();

      assert(myArray.total_iterators() == 4);
    }

    assert(myArray.total_iterators() == 0);
  }

  {
    auto* pArray = new static_array<int, 5>();

    assert(pArray->total_iterators() == 0);

    auto it1 = pArray->begin();
    auto it2 = pArray->end();

    const auto* cpArray = pArray;
    auto it3 = cpArray->begin();
    auto it4 = cpArray->end();

    assert(pArray->total_iterators() == 4);
    assert(it1.is_valid());
    assert(it2.is_valid());
    assert(it3.is_valid());
    assert(it4.is_valid());

    delete pArray, pArray = nullptr;

    assert(!it1.is_valid());
    assert(!it2.is_valid());
    assert(!it3.is_valid());
    assert(!it4.is_valid());
  }

  D_MYLOG(" => DONE");
}
