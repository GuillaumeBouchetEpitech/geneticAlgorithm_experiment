
#include "framework/system/containers/dynamic_heap_array.hpp"

#include "framework/system/TraceLogger.hpp"

#include "common.hpp"

#include <functional>
#include <vector>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

template <typename T>
using empty_heap_array = dynamic_heap_array<T, std::allocator<T>, 0>;

void test_dynamic_heap_array() {
  D_MYLOG("test_dynamic_heap_array()");

  {

    // common::enableLogs();

    empty_heap_array<common::Test> vertices;
    // std::vector<Test> vertices;
    // vertices.pre_allocate(16);

    assert(vertices.is_empty() == true);
    assert(vertices.size() == 0);
    assert(vertices.capacity() == 0);

    vertices.push_back(common::Test(1));

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 1);
    assert(vertices.capacity() == 1);
    assert(vertices[0].value == 1);

    // std::cout << "------" << std::endl;

    common::Test& lol = vertices.emplace_back(222222222); // emplace test

    assert(lol.value == 222222222);

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 2);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(3));

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 3);
    assert(vertices.capacity() == 4);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(4));

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 4);
    assert(vertices.capacity() == 4);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(5));

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 5);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);
    assert(vertices[4].value == 5);

    vertices.unsorted_erase(1);

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 4);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 5);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);

    vertices.sorted_erase(1);

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 3);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);
    assert(vertices[2].value == 4);

    vertices.pop_back();

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.pre_allocate(20);

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 20);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.pre_allocate(8);

    assert(vertices.is_empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 20);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.clear();

    assert(vertices.is_empty() == true);
    assert(vertices.size() == 0);
    assert(vertices.capacity() == 20);

    // std::cout << "==========" << std::endl;
    // std::cout << std::endl;
    // for (auto lol : vertices)
    //     std::cout << " => " << lol.value << std::endl;
    // std::cout << std::endl;

    // std::cout << "==========" << std::endl;
    // std::cout << std::endl;
    // for (auto& lol : vertices)
    //     std::cout << " => " << lol.value << std::endl;
    // std::cout << std::endl;

    // std::cout << "SCOPE END" << std::endl;

    // std::cout << " => totalCtor=     " << totalCtor << std::endl;
    // std::cout << " => totalCopyCtor= " << totalCopyCtor << std::endl;
    // std::cout << " => totalMoveCtor= " << totalMoveCtor << std::endl;
    // std::cout << " => totalDtor=     " << totalDtor << std::endl;
  }

  common::disableLogs();

  {
    // std::cout << "std::vector" << std::endl;

    common::reset();
    std::vector<common::Test> vertices;
    // vertices.pre_allocate(16);

    vertices.push_back(common::Test(1));
    vertices.emplace_back(222222222); // emplace test
    vertices.push_back(common::Test(3));
    vertices.push_back(common::Test(4));
    vertices.push_back(common::Test(5));

    // for (volatile auto lol : vertices)
    //     ;

    // for (volatile auto& lol : vertices)
    //     ;

    // print();
  }

  {
    // std::cout << "dynamic_heap_array" << std::endl;

    common::reset();
    empty_heap_array<common::Test> vertices;
    // vertices.pre_allocate(16);

    vertices.push_back(common::Test(1));
    vertices.emplace_back(222222222); // emplace test
    vertices.push_back(common::Test(3));
    vertices.push_back(common::Test(4));
    vertices.push_back(common::Test(5));

    assert(common::getTotalCtor() == 5);
    assert(common::getTotalCopyCtor() == 0);
    assert(common::getTotalMoveCtor() == 11);
    assert(common::getTotalDtor() == 11);

    // for (volatile auto lol : vertices)
    //     ;

    // for (volatile auto& lol : vertices)
    //     ;

    // print();
  }

  {
    // std::cout << "dynamic_heap_array" << std::endl;

    common::reset();
    empty_heap_array<common::Test> vertices;
    vertices.pre_allocate(16);

    vertices.push_back(common::Test(1));
    vertices.push_back(common::Test(2));
    vertices.push_back(common::Test(3));
    vertices.push_back(common::Test(4));
    vertices.push_back(common::Test(5));

    {
      int index = 0;
      for (volatile auto& test : vertices) {
        assert(test.value == ((index++) + 1));
      }
    }

    {
      int index = 0;
      empty_heap_array<common::Test>& cvertices = vertices;
      for (volatile const auto& test : cvertices) {
        assert(test.value == ((index++) + 1));
      }
    }

    // print();
  }

  {

    common::reset();
    dynamic_heap_array<common::Test> vertices;
    // vertices.pre_allocate(16);

    vertices.push_back(common::Test(1));
    vertices.emplace_back(222222222); // emplace test

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    vertices[0].my_string = "LOL";
    vertices[1].my_string = "LOL";

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    // my_string
  }

  {

    common::reset();
    dynamic_heap_array<common::Test2> vertices;
    // vertices.pre_allocate(16);

    // vertices.push_back();
    vertices.emplace_back();

    vertices.emplace_back();

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    vertices[0].my_string = "LOL";
    vertices[1].my_string = "LOL";

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    // my_string
  }

  {

    common::reset();
    dynamic_heap_array<common::Test2> vertices;

    common::Test2 test1;
    test1.my_string = "666";

    assert(!test1.my_string.empty());
    assert(test1.my_string == "666");

    vertices.push_back(std::move(test1));

    assert(test1.my_string.empty());
    assert(!vertices[0].my_string.empty());
    assert(vertices[0].my_string == "666");
  }

  {

    common::reset();
    dynamic_heap_array<common::Test> vertices1;
    dynamic_heap_array<common::Test> vertices2;
    // vertices.pre_allocate(16);

    vertices1.emplace_back(111);
    vertices1.emplace_back(222);

    assert(vertices1.size() == 2);
    assert(vertices2.size() == 0);
    assert(vertices1[0].value == 111);
    assert(vertices1[1].value == 222);

    vertices2 = std::move(vertices1);

    assert(vertices1.size() == 0);
    assert(vertices2.size() == 2);
    assert(vertices2[0].value == 111);
    assert(vertices2[1].value == 222);
  }

  {

    common::reset();
    dynamic_heap_array<common::Test> vertices;

    for (int ii = 0; ii < 5; ++ii)
      vertices.emplace_back(ii);

    assert(vertices.size() == 5);

    assert(vertices[-5].value == 0);
    assert(vertices[-4].value == 1);
    assert(vertices[-3].value == 2);
    assert(vertices[-2].value == 3);
    assert(vertices[-1].value == 4);

    assert(vertices[0].value == 0);
    assert(vertices[1].value == 1);
    assert(vertices[2].value == 2);
    assert(vertices[3].value == 3);
    assert(vertices[4].value == 4);

    assert(vertices[5].value == 0);
    assert(vertices[6].value == 1);
    assert(vertices[7].value == 2);
    assert(vertices[8].value == 3);
    assert(vertices[9].value == 4);

    {
      int index = 0;
      for (auto it = vertices.begin(); it != vertices.end(); ++it)
        assert(it->value == index++);
      assert(index == int(vertices.size()));
    }

    {
      int index = 0;
      const auto& cvertices = vertices;
      for (auto it = cvertices.begin(); it != cvertices.end(); ++it)
        assert(it->value == index++);
      assert(index == int(vertices.size()));
    }

    {
      int index = 0;
      for (common::Test& item : vertices)
        assert(item.value == index++);
      assert(index == int(vertices.size()));
    }

    {
      int index = 0;
      const auto& cvertices = vertices;
      for (const common::Test& item : cvertices)
        assert(item.value == index++);
      assert(index == int(vertices.size()));
    }
  }

  {

    common::reset();
    dynamic_heap_array<common::Test> vertices;

    for (int ii = 0; ii < 5; ++ii)
      vertices.emplace_back(ii);

    assert(vertices.total_iterators() == 0);

    {
      auto it1 = vertices.begin();

      assert(vertices.total_iterators() == 1);
    }

    assert(vertices.total_iterators() == 0);

    {
      auto it1 = vertices.begin();
      auto it2 = vertices.end();

      const auto& cvertices = vertices;
      auto it3 = cvertices.begin();
      auto it4 = cvertices.end();

      assert(vertices.total_iterators() == 4);
    }

    assert(vertices.total_iterators() == 0);
  }

  {

    common::reset();
    dynamic_heap_array<common::Test>* pVertices =
      new dynamic_heap_array<common::Test>();

    for (int ii = 0; ii < 5; ++ii)
      pVertices->emplace_back(ii);

    assert(pVertices->total_iterators() == 0);

    auto it1 = pVertices->begin();
    auto it2 = pVertices->end();

    const auto* cpVertices = pVertices;
    auto it3 = cpVertices->begin();
    auto it4 = cpVertices->end();

    assert(pVertices->total_iterators() == 4);
    assert(it1.is_valid());
    assert(it2.is_valid());
    assert(it3.is_valid());
    assert(it4.is_valid());

    delete pVertices, pVertices = nullptr;

    assert(!it1.is_valid());
    assert(!it2.is_valid());
    assert(!it3.is_valid());
    assert(!it4.is_valid());
  }

  D_MYLOG(" => DONE");
}
