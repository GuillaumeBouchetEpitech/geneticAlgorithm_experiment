
#include "headers.hpp"

#include "generic_array_container_commons/common.hpp"

namespace {

struct dynamic_heap_array__remove_data : public common::threadsafe_fixture {};

} // namespace

TEST_F(dynamic_heap_array__remove_data, not_pre_allocated__pop_back) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 7);
  EXPECT_EQ(common::getTotalDtor(), 7);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 3);
  EXPECT_EQ(common::getTotalDealloc(), 2);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, is_pre_allocated__pop_back) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 4);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 2);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "2");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.pop_back();
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, not_pre_allocated__unsorted_erase) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 7);
  EXPECT_EQ(common::getTotalDtor(), 7);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 3);
  EXPECT_EQ(common::getTotalDealloc(), 2);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(1); // remove middle
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 1);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(0); // remove first
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 1);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(1); // remove last
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(0); // remove only left
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, is_pre_allocated__unsorted_erase) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 4);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(1); // remove middle
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 1);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(0); // remove first
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 1);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(1); // remove last
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.unsorted_erase(0); // remove only left
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, not_pre_allocated__sorted_erase) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 7);
  EXPECT_EQ(common::getTotalDtor(), 7);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 3);
  EXPECT_EQ(common::getTotalDealloc(), 2);
  common::reset();

  myDefaultDynamicArray.sorted_erase(1); // remove middle
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 2);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(0); // remove first
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 2);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(1); // remove last
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(0); // remove only left
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, is_pre_allocated__sorted_erase) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 4);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(1); // remove middle
  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 1);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "1");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 2);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(0); // remove first
  EXPECT_EQ(myDefaultDynamicArray.size(), 2);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 4);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "4");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 2);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(1); // remove last
  EXPECT_EQ(myDefaultDynamicArray.size(), 1);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 3);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "3");

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.sorted_erase(0); // remove only left
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 1);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, not_pre_allocated__clear) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 7);
  EXPECT_EQ(common::getTotalDtor(), 7);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 3);
  EXPECT_EQ(common::getTotalDealloc(), 2);
  common::reset();

  myDefaultDynamicArray.clear();
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 4);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__remove_data, is_pre_allocated__clear) {
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
  myDefaultDynamicArray.push_back(common::TestStructure(1, "1"));
  myDefaultDynamicArray.push_back(common::TestStructure(2, "2"));
  myDefaultDynamicArray.push_back(common::TestStructure(3, "3"));
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

  EXPECT_EQ(common::getTotalCtor(), 4);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 4);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.clear();
  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 4);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}
