
#include "headers.hpp"

#include "generic_array_container_commons/common.hpp"

namespace {

struct dynamic_heap_array__allocating_features
  : public common::threadsafe_fixture {};

} // namespace

TEST_F(dynamic_heap_array__allocating_features,
       not_pre_allocated__pre_allocate) {
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

  myDefaultDynamicArray.pre_allocate(5);

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
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__pre_allocate_same) {
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

  myDefaultDynamicArray.pre_allocate(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__pre_allocate_less) {
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

  myDefaultDynamicArray.pre_allocate(3);

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__pre_allocate_more) {
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

  myDefaultDynamicArray.pre_allocate(10);

  EXPECT_EQ(myDefaultDynamicArray.size(), 0);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       not_pre_allocated__ensure_size) {
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

  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_same) {
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

  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_less) {
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

  myDefaultDynamicArray.ensure_size(3);

  EXPECT_EQ(myDefaultDynamicArray.size(), 3);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 3);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_more) {
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

  myDefaultDynamicArray.ensure_size(10);

  EXPECT_EQ(myDefaultDynamicArray.size(), 10);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[5].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[5].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[6].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[6].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[7].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[7].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[8].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[8].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[9].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[9].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 10);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       not_pre_allocated__ensure_size_grow_existing) {
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
  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 5);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.ensure_size(10);

  EXPECT_EQ(myDefaultDynamicArray.size(), 10);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[5].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[5].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[6].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[6].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[7].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[7].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[8].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[8].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[9].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[9].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 5);
  EXPECT_EQ(common::getTotalDtor(), 5);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_grow_existing_same) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 10>
    myDefaultDynamicArray;
  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.ensure_size(10);

  EXPECT_EQ(myDefaultDynamicArray.size(), 10);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[5].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[5].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[6].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[6].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[7].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[7].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[8].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[8].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[9].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[9].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_grow_existing_less) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 10>
    myDefaultDynamicArray;
  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.ensure_size(8);

  EXPECT_EQ(myDefaultDynamicArray.size(), 8);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[5].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[5].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[6].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[6].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[7].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[7].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 3);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();
}

TEST_F(dynamic_heap_array__allocating_features,
       is_pre_allocated__ensure_size_grow_existing_more) {
  EXPECT_EQ(common::getTotalCtor(), 0);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 0);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  dynamic_heap_array<common::TestStructure,
                     common::MyAllocator<common::TestStructure>, 10>
    myDefaultDynamicArray;
  myDefaultDynamicArray.ensure_size(5);

  EXPECT_EQ(myDefaultDynamicArray.size(), 5);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);

  EXPECT_EQ(common::getTotalCtor(), 5);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 0);
  EXPECT_EQ(common::getTotalDtor(), 0);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 0);
  common::reset();

  myDefaultDynamicArray.ensure_size(15);

  EXPECT_EQ(myDefaultDynamicArray.size(), 15);
  EXPECT_EQ(myDefaultDynamicArray.capacity(), 15);
  EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);
  EXPECT_EQ(myDefaultDynamicArray[0].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[0].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[1].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[1].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[2].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[2].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[3].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[3].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[4].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[4].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[5].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[5].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[6].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[6].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[7].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[7].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[8].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[8].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[9].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[9].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[10].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[10].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[11].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[11].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[12].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[12].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[13].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[13].my_string, "");
  EXPECT_EQ(myDefaultDynamicArray[14].value, 0);
  EXPECT_EQ(myDefaultDynamicArray[14].my_string, "");

  EXPECT_EQ(common::getTotalCtor(), 10);
  EXPECT_EQ(common::getTotalCopyCtor(), 0);
  EXPECT_EQ(common::getTotalMoveCtor(), 5);
  EXPECT_EQ(common::getTotalDtor(), 5);
  EXPECT_EQ(common::getTotalSwapApplied(), 0);
  EXPECT_EQ(common::getTotalAlloc(), 1);
  EXPECT_EQ(common::getTotalDealloc(), 1);
  common::reset();
}
