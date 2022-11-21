
#include "framework/containers/dynamic_heap_array.hpp"

#include "common.hpp"

#include <vector>
#include <functional>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

void test_dynamic_heap_array()
{
  std::cout << "test_dynamic_heap_array()" << std::endl;

  {

    // common::enableLogs();

    dynamic_heap_array<common::Test> vertices;
    // std::vector<Test> vertices;
    // vertices.reserve(16);

    assert(vertices.empty() == true);
    assert(vertices.size() == 0);
    assert(vertices.capacity() == 0);

    vertices.push_back(common::Test(1));

    assert(vertices.empty() == false);
    assert(vertices.size() == 1);
    assert(vertices.capacity() == 1);
    assert(vertices[0].value == 1);

    // std::cout << "------" << std::endl;

    common::Test& lol = vertices.emplace_back(222222222); // emplace test

    assert(lol.value == 222222222);

    assert(vertices.empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 2);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(3));

    assert(vertices.empty() == false);
    assert(vertices.size() == 3);
    assert(vertices.capacity() == 4);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(4));

    assert(vertices.empty() == false);
    assert(vertices.size() == 4);
    assert(vertices.capacity() == 4);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);

    // std::cout << "------" << std::endl;
    vertices.push_back(common::Test(5));

    assert(vertices.empty() == false);
    assert(vertices.size() == 5);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 222222222);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);
    assert(vertices[4].value == 5);

    vertices.unsortedErase(1);

    assert(vertices.empty() == false);
    assert(vertices.size() == 4);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 5);
    assert(vertices[2].value == 3);
    assert(vertices[3].value == 4);

    vertices.sortedErase(1);

    assert(vertices.empty() == false);
    assert(vertices.size() == 3);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);
    assert(vertices[2].value == 4);

    vertices.pop_back();

    assert(vertices.empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 8);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.reserve(20);

    assert(vertices.empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 20);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.reserve(8);

    assert(vertices.empty() == false);
    assert(vertices.size() == 2);
    assert(vertices.capacity() == 20);
    assert(vertices[0].value == 1);
    assert(vertices[1].value == 3);

    vertices.clear();

    assert(vertices.empty() == true);
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
    // vertices.reserve(16);

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
    dynamic_heap_array<common::Test> vertices;
    // vertices.reserve(16);

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

  std::cout << " => DONE" << std::endl;
}
