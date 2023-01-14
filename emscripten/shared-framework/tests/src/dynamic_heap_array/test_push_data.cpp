
#include "headers.hpp"

#include "generic_array_container_commons/common.hpp"

namespace {

struct dynamic_heap_array__push_data : public common::threadsafe_fixture {};

} // namespace

TEST_F(dynamic_heap_array__push_data, not_pre_allocated__push_back) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 0>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 0);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 1);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 2);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 2);
  EXPECT_EQ(common::getTotalDtor(), 2);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 3);
  EXPECT_EQ(common::getTotalDtor(), 3);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(4, "4"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__push_data, not_pre_allocated__push_back_move) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 0>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 0);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data1(1, "1");
  myDefaultDynamicArray.push_back(std::move(data1));
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 1);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data2(2, "2");
  myDefaultDynamicArray.push_back(std::move(data2));
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 2);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 2);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  common::TestStructure data3(3, "3");
  myDefaultDynamicArray.push_back(std::move(data3));
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 3);
  EXPECT_EQ(common::getTotalDtor(), 2);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  common::TestStructure data4(4, "4");
  myDefaultDynamicArray.push_back(std::move(data4));
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__push_data, is_pre_allocated__push_back) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 5>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.push_back(common::TestStructure(4, "4"));
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__push_data, is_pre_allocated__push_back_move) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 5>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data1(1, "1");
  myDefaultDynamicArray.push_back(std::move(data1));
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data2(2, "2");
  myDefaultDynamicArray.push_back(std::move(data2));
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data3(3, "3");
  myDefaultDynamicArray.push_back(std::move(data3));
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  common::TestStructure data4(4, "4");
  myDefaultDynamicArray.push_back(std::move(data4));
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__push_data, not_pre_allocated__emplace_back) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 0>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 0);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(1, "1");
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 1);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(2, "2");
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 2);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 1);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  myDefaultDynamicArray.emplace_back(3, "3");
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 2);
  EXPECT_EQ(common::getTotalDtor(), 2);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();

  myDefaultDynamicArray.emplace_back(4, "4");
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__push_data, is_pre_allocated__emplace_back) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 5>
    myDefaultDynamicArray;

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(1, "1");
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(2, "2");
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(3, "3");
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.emplace_back(4, "4");
  EXPECT_EQ(myDefaultDynamicArray.size(), 4);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 1);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}
