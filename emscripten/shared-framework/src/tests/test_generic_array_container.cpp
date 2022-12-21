
#include "framework/system/containers/generic_array_container.hpp"

#include "framework/system/TraceLogger.hpp"

#include "common.hpp"

#include <functional>
#include <vector>
// #include <iostream>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

// template<typename T>
// using test_dynamic_array = test_dynamic_array<T, std::allocator<T>, 0>;

struct TestData {
  int value;
};

// template<typename Type>
void generic_tests(generic_array_container<TestData>& container,
                   generic_array_container<TestData>* pContainer) {

  assert(!container.is_empty());
  assert(container.size() == 5);

  {
    assert(container == container);
    assert(container != *pContainer);
  }

  {

    for (int ii = 0; ii < int(container.size()); ++ii)
      container[ii].value = ii;

    for (int ii = 0; ii < int(container.size()); ++ii)
      assert(container[ii].value == ii);

    {{std::size_t index = 0;
    for (auto it = container.begin(); it != container.end(); ++it)
      assert((*it).value == int(index++));
    assert(index == container.size());
  }

  {
    std::size_t index = 0;
    const auto& ccontainer = container;
    for (auto it = ccontainer.begin(); it != ccontainer.end(); ++it)
      assert((*it).value == int(index++));
    assert(index == ccontainer.size());
  }
}

{{std::size_t index = 0;
for (auto it = container.begin(); it != container.end(); ++it)
  assert(it->value == int(index++));
assert(index == container.size());
}

{
  std::size_t index = 0;
  const auto& ccontainer = container;
  for (auto it = ccontainer.begin(); it != ccontainer.end(); ++it)
    assert(it->value == int(index++));
  assert(index == ccontainer.size());
}
}

{
  std::array<int, 3> testIndices = {{-5, 0, 5}};

  for (int testIndex : testIndices) {
    {
      std::size_t index = 0;
      for (auto it = container.begin(); it != container.end(); ++it)
        assert(it[testIndex].value == int(index++));
      assert(index == container.size());
    }
    {
      std::size_t index = 0;
      const auto& ccontainer = container;
      for (auto it = ccontainer.begin(); it != ccontainer.end(); ++it)
        assert(it[testIndex].value == int(index++));
      assert(index == ccontainer.size());
    }
  }
}

{

  {std::size_t index = 0;
for (TestData& testData : container)
  assert(testData.value == int(index++));
assert(index == container.size());
}

{
  std::size_t index = 0;
  const auto& ccontainer = container;
  for (const TestData& testData : ccontainer)
    assert(testData.value == int(index++));
  assert(index == ccontainer.size());
}
}

{
  const auto& ccontainer = container;

  assert(container.front().value == 0);
  assert(ccontainer.front().value == 0);

  assert(container.back().value == 4);
  assert(ccontainer.back().value == 4);
}

{
  assert(container[-5].value == 0);
  assert(container[-4].value == 1);
  assert(container[-3].value == 2);
  assert(container[-2].value == 3);
  assert(container[-1].value == 4);

  assert(container[0].value == 0);
  assert(container[1].value == 1);
  assert(container[2].value == 2);
  assert(container[3].value == 3);
  assert(container[4].value == 4);

  assert(container[5].value == 0);
  assert(container[6].value == 1);
  assert(container[7].value == 2);
  assert(container[8].value == 3);
  assert(container[9].value == 4);
}

{
  assert(container.at(0).value == 0);
  assert(container.at(1).value == 1);
  assert(container.at(2).value == 2);
  assert(container.at(3).value == 3);
  assert(container.at(4).value == 4);

  {
    bool errorThrown = false;
    try {
      container.at(66666);
    } catch (...) {
      errorThrown = true;
    }
    assert(errorThrown);
  }

  assert(!container.is_out_of_range(0));
  assert(!container.is_out_of_range(1));
  assert(!container.is_out_of_range(2));
  assert(!container.is_out_of_range(3));
  assert(!container.is_out_of_range(4));
  assert(container.is_out_of_range(5));
  assert(container.is_out_of_range(6));
  assert(container.is_out_of_range(7));
}
}

{
  // test_static_array<int, 5> myArray;

  assert(container.total_iterators() == 0);

  {
    auto it1 = container.begin();

    assert(container.total_iterators() == 1);
  }

  assert(container.total_iterators() == 0);

  {
    auto it1 = container.begin();
    auto it2 = container.end();

    const auto& ccontainer = container;
    auto it3 = ccontainer.begin();
    auto it4 = ccontainer.end();

    assert(container.total_iterators() == 4);
  }

  assert(container.total_iterators() == 0);
}

{
  assert(container.total_iterators() == 0);

  auto it1 = container.begin();
  auto it2 = container.end();

  const auto& ccontainer = container;
  auto it3 = ccontainer.begin();
  auto it4 = ccontainer.end();

  assert(container.total_iterators() == 4);
  assert(it1.is_valid());
  assert(it2.is_valid());
  assert(it3.is_valid());
  assert(it4.is_valid());

  container.invalidate_all_iterators();

  assert(!it1.is_valid());
  assert(!it2.is_valid());
  assert(!it3.is_valid());
  assert(!it4.is_valid());
}

{

  for (int ii = 0; ii < int((*pContainer).size()); ++ii)
    (*pContainer)[ii].value = ii + 100;

  for (int ii = 0; ii < int(container.size()); ++ii)
    assert(container[ii].value == ii);
  for (int ii = 0; ii < int((*pContainer).size()); ++ii)
    assert((*pContainer)[ii].value == ii + 100);

  common::reset();

  container = std::move(*pContainer);

  assert(common::getTotalCtor() == 0);
  assert(common::getTotalCopyCtor() == 0);
  assert(common::getTotalMoveCtor() == 0);
  assert(common::getTotalDtor() == 0);

  for (int ii = 0; ii < int(container.size()); ++ii)
    assert(container[ii].value == ii + 100);
  for (int ii = 0; ii < int((*pContainer).size()); ++ii)
    assert((*pContainer)[ii].value == ii);

  common::reset();

  *pContainer = std::move(container);

  assert(common::getTotalCtor() == 0);
  assert(common::getTotalCopyCtor() == 0);
  assert(common::getTotalMoveCtor() == 0);
  assert(common::getTotalDtor() == 0);

  for (int ii = 0; ii < int(container.size()); ++ii)
    assert(container[ii].value == ii);
  for (int ii = 0; ii < int((*pContainer).size()); ++ii)
    assert((*pContainer)[ii].value == ii + 100);
}

{
  assert(pContainer->total_iterators() == 0);

  auto it1 = pContainer->begin();
  auto it2 = pContainer->end();

  const auto* cpContainer = pContainer;
  auto it3 = cpContainer->begin();
  auto it4 = cpContainer->end();

  assert(pContainer->total_iterators() == 4);
  assert(it1.is_valid());
  assert(it2.is_valid());
  assert(it3.is_valid());
  assert(it4.is_valid());

  delete pContainer, pContainer = nullptr;

  assert(!it1.is_valid());
  assert(!it2.is_valid());
  assert(!it3.is_valid());
  assert(!it4.is_valid());
}
}

namespace {

int g_totalAlloc = 0;
int g_totalDealloc = 0;

template <typename T> struct MyAllocator {
  using value_type = T;

  MyAllocator() = default;
  template <class U> MyAllocator(const MyAllocator<U>&) {}

  T* allocate(std::size_t n) {
    g_totalAlloc += 1;
    D_MYLOG("allocate -> " << n);
    if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      if (auto ptr = std::malloc(n * sizeof(T))) {
        return static_cast<T*>(ptr);
      }
    }
    throw std::bad_alloc();
  }
  void deallocate(T* ptr, std::size_t n) {

    // throw std::runtime_error("LOL");

    D_MYLOG("deallocate -> " << n);

    g_totalDealloc += 1;
    static_cast<void>(n); // unused
    std::free(ptr);
  }
};

template <typename T, typename U>
inline bool operator==(const MyAllocator<T>&, const MyAllocator<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator!=(const MyAllocator<T>& a, const MyAllocator<U>& b) {
  return !(a == b);
}
} // namespace

void test_dynamic_array_tests() {

  {

    // common::enableLogs();

    test_dynamic_array<common::Test> vertices;
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
    test_dynamic_array<common::Test> vertices;
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
    test_dynamic_array<common::Test> vertices;
    vertices.pre_allocate(5);

    vertices.push_back(common::Test(1));
    // vertices.emplace_back(222222222); // emplace test
    vertices.push_back(common::Test(2));
    vertices.push_back(common::Test(3));
    vertices.push_back(common::Test(4));
    vertices.push_back(common::Test(5));

    assert(common::getTotalCtor() == 5);
    assert(common::getTotalCopyCtor() == 0);
    assert(common::getTotalMoveCtor() == 5);
    assert(common::getTotalDtor() == 5);

    // for (volatile auto lol : vertices)
    //     ;

    // for (volatile auto& lol : vertices)
    //     ;

    // print();
  }

  {
    // std::cout << "dynamic_heap_array" << std::endl;

    common::reset();
    test_dynamic_array<common::Test> vertices;
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
      test_dynamic_array<common::Test>& cvertices = vertices;
      for (volatile const auto& test : cvertices) {
        assert(test.value == ((index++) + 1));
      }
    }

    // print();
  }

  {
    // std::cout << "dynamic_heap_array" << std::endl;

    g_totalAlloc = 0;
    g_totalDealloc = 0;

    {

      constexpr std::size_t k_pre_allocated_size = 10;

      common::reset();
      test_dynamic_array<common::Test, MyAllocator<common::Test>> vertices;
      vertices.pre_allocate(k_pre_allocated_size);

      assert(common::getTotalCtor() == 0);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 0);
      assert(common::getTotalDtor() == 0);
      common::reset();

      assert(g_totalAlloc == 1);
      assert(g_totalDealloc == 0);

      vertices.push_back(common::Test(1));
      vertices.push_back(common::Test(2));
      vertices.push_back(common::Test(3));
      vertices.push_back(common::Test(4));
      vertices.push_back(common::Test(5));

      assert(common::getTotalCtor() == 5);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 5);
      assert(common::getTotalDtor() == 5);
      common::reset();

      vertices.emplace_back(6);
      vertices.emplace_back(7);
      vertices.emplace_back(8);
      vertices.emplace_back(9);
      vertices.emplace_back(10);

      assert(common::getTotalCtor() == 5);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 0);
      assert(common::getTotalDtor() == 0);
      common::reset();

      assert(g_totalAlloc == 1);
      assert(g_totalDealloc == 0);

      assert(vertices.capacity() == k_pre_allocated_size);
      assert(vertices.size() == k_pre_allocated_size);

      assert(common::getTotalCtor() == 0);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 0);
      assert(common::getTotalDtor() == 0);
      common::reset();

      vertices.sorted_erase(2);

      assert(g_totalAlloc == 1);
      assert(g_totalDealloc == 0);

      assert(vertices.capacity() == k_pre_allocated_size);
      assert(vertices.size() == k_pre_allocated_size - 1);
      assert(vertices[0].value == 1);
      assert(vertices[1].value == 2);
      assert(vertices[2].value == 4);
      assert(vertices[3].value == 5);
      assert(vertices[4].value == 6);
      assert(vertices[5].value == 7);
      assert(vertices[6].value == 8);
      assert(vertices[7].value == 9);
      assert(vertices[8].value == 10);

      assert(common::getTotalCtor() == 0);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 7);
      assert(common::getTotalDtor() == 1);
      common::reset();

      vertices.unsorted_erase(4); // replace 6 with 10

      assert(g_totalAlloc == 1);
      assert(g_totalDealloc == 0);

      assert(vertices.capacity() == k_pre_allocated_size);
      assert(vertices.size() == k_pre_allocated_size - 2);
      assert(vertices[0].value == 1);
      assert(vertices[1].value == 2);
      assert(vertices[2].value == 4);
      assert(vertices[3].value == 5);
      assert(vertices[4].value == 10);
      assert(vertices[5].value == 7);
      assert(vertices[6].value == 8);
      assert(vertices[7].value == 9);

      assert(common::getTotalCtor() == 0);
      assert(common::getTotalCopyCtor() == 0);
      assert(common::getTotalMoveCtor() == 1);
      assert(common::getTotalDtor() == 1);
      common::reset();
    }

    assert(g_totalAlloc == 1);
    assert(g_totalDealloc == 1);
  }

  {

    common::reset();
    test_dynamic_array<common::Test> vertices;
    // vertices.pre_allocate(16);

    vertices.push_back(common::Test(1));
    vertices.emplace_back(222222222); // emplace test

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    assert(vertices[0].my_string.empty());
    assert(vertices[1].my_string.empty());
    assert(vertices[0].my_string == "");
    assert(vertices[1].my_string == "");

    vertices[0].my_string = "LOL";
    vertices[1].my_string = "LOL";

    assert(!vertices[0].my_string.empty());
    assert(!vertices[1].my_string.empty());
    assert(vertices[0].my_string == "LOL");
    assert(vertices[1].my_string == "LOL");

    D_MYLOG("vertices[0].my_string " << vertices[0].my_string);
    D_MYLOG("vertices[1].my_string " << vertices[1].my_string);

    // my_string
  }

  {

    common::reset();
    test_dynamic_array<common::Test> vertices;
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
    test_dynamic_array<common::Test> vertices;

    common::Test test1;
    test1.my_string = "666";

    assert(!test1.my_string.empty());
    assert(test1.my_string == "666");

    common::reset();

    vertices.push_back(std::move(test1));

    // common::print();
    assert(common::getTotalCtor() == 0);
    assert(common::getTotalCopyCtor() == 0);
    assert(common::getTotalMoveCtor() == 1);
    assert(common::getTotalDtor() == 0);

    assert(test1.my_string.empty());
    assert(!vertices[0].my_string.empty());
    assert(vertices[0].my_string == "666");
  }

  {

    common::reset();
    test_dynamic_array<common::Test> vertices1;
    test_dynamic_array<common::Test> vertices2;
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
}

void test_generic_array_container() {
  D_MYLOG("test_generic_array_container()");

  {
    D_MYLOG(" --> test_static_array");

    test_static_array<TestData, 5> mySaticArray;
    auto* pStaticArray = new test_static_array<TestData, 5>();

    generic_tests(mySaticArray, pStaticArray);
  }

  {
    D_MYLOG(" --> test_dynamic_array");

    test_dynamic_array<TestData> myDynamicArray;
    auto* pDynamicArray = new test_dynamic_array<TestData>();
    myDynamicArray.ensure_size(5);
    pDynamicArray->ensure_size(5);

    generic_tests(myDynamicArray, pDynamicArray);
  }

  { test_dynamic_array_tests(); }

  D_MYLOG(" => DONE");
}
