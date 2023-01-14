
#include "headers.hpp"

#include "generic_array_container_commons/common.hpp"

TEST(generic_array_container_iterators,
     can_be_iterator_looped_and_set_and_then_get_values_afterward) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {
      int index = 100;
      for (auto it = mySaticArray.begin(); it != mySaticArray.end(); ++it) {
        it->value = index++;
        it->my_string = "test";
      }
    }

    {
      int index = 100;
      for (auto it = mySaticArray.begin(); it != mySaticArray.end(); ++it) {
        EXPECT_EQ(it->value, index++);
        EXPECT_EQ(it->my_string, "test");
      }
    }
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    {
      int index = 100;
      for (auto it = mySizedDynamicArray.begin();
           it != mySizedDynamicArray.end(); ++it) {
        it->value = index++;
        it->my_string = "test";
      }
    }

    {
      int index = 100;
      for (auto it = mySizedDynamicArray.begin();
           it != mySizedDynamicArray.end(); ++it) {
        EXPECT_EQ(it->value, index++);
        EXPECT_EQ(it->my_string, "test");
      }
    }
  }
}

TEST(generic_array_container_iterators,
     can_be_const_iterator_looped_and_set_and_then_get_values_afterward) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {

      {int index = 100;
    for (auto it = mySaticArray.begin(); it != mySaticArray.end(); ++it) {
      (*it).value = index++;
      (*it).my_string = "test";
    }
  }

  {
    int index = 100;
    const auto& cmySaticArray = mySaticArray;
    for (auto it = cmySaticArray.begin(); it != cmySaticArray.end(); ++it) {
      EXPECT_EQ((*it).value, index++);
      EXPECT_EQ((*it).my_string, "test");
    }
  }
}

{

  {
    int index = 100;
    for (auto it = mySaticArray.begin(); it != mySaticArray.end(); ++it) {
      (*it).value = index++;
      (*it).my_string = "test";
    }
  }

  {
    int index = 100;
    const auto& cmySaticArray = mySaticArray;
    for (auto it = cmySaticArray.begin(); it != cmySaticArray.end(); ++it) {
      EXPECT_EQ((*it).value, index++);
      EXPECT_EQ((*it).my_string, "test");
    }
  }
}
}

{
  shorthand_dynamic_heap_array<5> mySizedDynamicArray;
  mySizedDynamicArray.ensure_size(5);

  {

    {int index = 100;
  for (auto it = mySizedDynamicArray.begin(); it != mySizedDynamicArray.end();
       ++it) {
    (*it).value = index++;
    (*it).my_string = "test";
  }
}

{
  int index = 100;
  const auto& cmySizedDynamicArray = mySizedDynamicArray;
  for (auto it = cmySizedDynamicArray.begin(); it != cmySizedDynamicArray.end();
       ++it) {
    EXPECT_EQ((*it).value, index++);
    EXPECT_EQ((*it).my_string, "test");
  }
}
}

{

  {
    int index = 100;
    for (auto it = mySizedDynamicArray.begin(); it != mySizedDynamicArray.end();
         ++it) {
      (*it).value = index++;
      (*it).my_string = "test";
    }
  }

  {
    int index = 100;
    const auto& cmySizedDynamicArray = mySizedDynamicArray;
    for (auto it = cmySizedDynamicArray.begin();
         it != cmySizedDynamicArray.end(); ++it) {
      EXPECT_EQ((*it).value, index++);
      EXPECT_EQ((*it).my_string, "test");
    }
  }
}
}
}

TEST(generic_array_container_iterators,
     can_be_iterator_incremented_and_set_and_then_get_values_afterward) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {
      int index = 100;
      auto it = mySaticArray.begin();
      for (int ii = 0; ii < int(mySaticArray.size()); ++ii) {
        it[ii].value = index++;
        it[ii].my_string = "test";
      }
    }

    {
      int index = 100;
      const auto& cmySaticArray = mySaticArray;
      auto it = cmySaticArray.begin();
      for (int ii = 0; ii < int(cmySaticArray.size()); ++ii) {
        EXPECT_EQ(it[ii].value, index++);
        EXPECT_EQ(it[ii].my_string, "test");
      }
    }
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    {
      int index = 100;
      auto it = mySizedDynamicArray.begin();
      for (int ii = 0; ii < int(mySizedDynamicArray.size()); ++ii) {
        it[ii].value = index++;
        it[ii].my_string = "test";
      }
    }

    {
      int index = 100;
      auto it = mySizedDynamicArray.begin();
      for (int ii = 0; ii < int(mySizedDynamicArray.size()); ++ii) {
        EXPECT_EQ(it[ii].value, index++);
        EXPECT_EQ(it[ii].my_string, "test");
      }
    }
  }
}

TEST(generic_array_container_iterators,
     can_be_const_iterator_incremented_and_set_and_then_get_values_afterward) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {
      int index = 100;
      auto it = mySaticArray.begin();
      for (int ii = 0; ii < int(mySaticArray.size()); ++ii) {
        it[ii].value = index++;
        it[ii].my_string = "test";
      }
    }

    {
      int index = 100;
      auto it = mySaticArray.begin();
      for (int ii = 0; ii < int(mySaticArray.size()); ++ii) {
        EXPECT_EQ(it[ii].value, index++);
        EXPECT_EQ(it[ii].my_string, "test");
      }
    }
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    {
      int index = 100;
      auto it = mySizedDynamicArray.begin();
      for (int ii = 0; ii < int(mySizedDynamicArray.size()); ++ii) {
        it[ii].value = index++;
        it[ii].my_string = "test";
      }
    }

    {
      int index = 100;
      const auto& cmySizedDynamicArray = mySizedDynamicArray;
      auto it = cmySizedDynamicArray.begin();
      for (int ii = 0; ii < int(cmySizedDynamicArray.size()); ++ii) {
        EXPECT_EQ(it[ii].value, index++);
        EXPECT_EQ(it[ii].my_string, "test");
      }
    }
  }
}

TEST(generic_array_container_iterators,
     can_make_and_count_total_live_iterators) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {
      EXPECT_EQ(mySaticArray.total_iterators(), 0);
      {
        auto it1 = mySaticArray.begin();
        EXPECT_EQ(mySaticArray.total_iterators(), 1);
      }
      EXPECT_EQ(mySaticArray.total_iterators(), 0);
    }

    {
      const auto& cmySaticArray = mySaticArray;
      EXPECT_EQ(cmySaticArray.total_iterators(), 0);
      {
        auto it1 = cmySaticArray.begin();
        EXPECT_EQ(cmySaticArray.total_iterators(), 1);
      }
      EXPECT_EQ(cmySaticArray.total_iterators(), 0);
    }
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    {
      EXPECT_EQ(mySizedDynamicArray.total_iterators(), 0);
      {
        auto it1 = mySizedDynamicArray.begin();
        EXPECT_EQ(mySizedDynamicArray.total_iterators(), 1);
      }
      EXPECT_EQ(mySizedDynamicArray.total_iterators(), 0);
    }

    {
      const auto& cmySizedDynamicArray = mySizedDynamicArray;
      EXPECT_EQ(cmySizedDynamicArray.total_iterators(), 0);
      {
        auto it1 = cmySizedDynamicArray.begin();
        EXPECT_EQ(cmySizedDynamicArray.total_iterators(), 1);
      }
      EXPECT_EQ(cmySizedDynamicArray.total_iterators(), 0);
    }
  }
}

TEST(generic_array_container_iterators, can_invalidate_all_iterators) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    EXPECT_EQ(mySaticArray.total_iterators(), 0);

    auto it1 = mySaticArray.begin();
    auto it2 = mySaticArray.end();

    const auto& cmySaticArray = mySaticArray;
    auto cit1 = cmySaticArray.begin();
    auto cit2 = cmySaticArray.end();

    EXPECT_EQ(mySaticArray.total_iterators(), 4);
    EXPECT_EQ(it1.is_valid(), true);
    EXPECT_EQ(it2.is_valid(), true);
    EXPECT_EQ(cit1.is_valid(), true);
    EXPECT_EQ(cit2.is_valid(), true);

    mySaticArray.invalidate_all_iterators();

    EXPECT_EQ(it1.is_valid(), false);
    EXPECT_EQ(it2.is_valid(), false);
    EXPECT_EQ(cit1.is_valid(), false);
    EXPECT_EQ(cit2.is_valid(), false);
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    EXPECT_EQ(mySizedDynamicArray.total_iterators(), 0);

    auto it1 = mySizedDynamicArray.begin();
    auto it2 = mySizedDynamicArray.end();

    const auto& cmySizedDynamicArray = mySizedDynamicArray;
    auto cit1 = cmySizedDynamicArray.begin();
    auto cit2 = cmySizedDynamicArray.end();

    EXPECT_EQ(mySizedDynamicArray.total_iterators(), 4);
    EXPECT_EQ(it1.is_valid(), true);
    EXPECT_EQ(it2.is_valid(), true);
    EXPECT_EQ(cit1.is_valid(), true);
    EXPECT_EQ(cit2.is_valid(), true);

    mySizedDynamicArray.invalidate_all_iterators();

    EXPECT_EQ(it1.is_valid(), false);
    EXPECT_EQ(it2.is_valid(), false);
    EXPECT_EQ(cit1.is_valid(), false);
    EXPECT_EQ(cit2.is_valid(), false);
  }
}

TEST(generic_array_container_iterators,
     must_invalidate_all_iterators_on_container_destruction) {
  {
    auto mySaticArray =
      std::make_unique<static_array<common::TestStructure, 5>>();

    EXPECT_EQ(mySaticArray->total_iterators(), 0);

    auto it1 = mySaticArray->begin();
    auto it2 = mySaticArray->end();

    const auto& cmySaticArray = mySaticArray;
    auto cit1 = cmySaticArray->begin();
    auto cit2 = cmySaticArray->end();

    EXPECT_EQ(mySaticArray->total_iterators(), 4);
    EXPECT_EQ(it1.is_valid(), true);
    EXPECT_EQ(it2.is_valid(), true);
    EXPECT_EQ(cit1.is_valid(), true);
    EXPECT_EQ(cit2.is_valid(), true);

    mySaticArray.reset(nullptr);

    EXPECT_EQ(it1.is_valid(), false);
    EXPECT_EQ(it2.is_valid(), false);
    EXPECT_EQ(cit1.is_valid(), false);
    EXPECT_EQ(cit2.is_valid(), false);
  }

  {
    auto mySizedDynamicArray =
      std::make_unique<shorthand_dynamic_heap_array<5>>();
    mySizedDynamicArray->ensure_size(5);

    // for (std::size_t ii = 0; ii < mySizedDynamicArray->size(); ++ii)
    // {
    //   (*mySizedDynamicArray)[int(ii)].value = int(ii);
    //   (*mySizedDynamicArray)[int(ii)].my_string = "test";
    // }

    EXPECT_EQ(mySizedDynamicArray->total_iterators(), 0);

    auto it1 = mySizedDynamicArray->begin();
    auto it2 = mySizedDynamicArray->end();

    const auto& cmySizedDynamicArray = mySizedDynamicArray;
    auto cit1 = cmySizedDynamicArray->begin();
    auto cit2 = cmySizedDynamicArray->end();

    EXPECT_EQ(mySizedDynamicArray->total_iterators(), 4);
    EXPECT_EQ(it1.is_valid(), true);
    EXPECT_EQ(it2.is_valid(), true);
    EXPECT_EQ(cit1.is_valid(), true);
    EXPECT_EQ(cit2.is_valid(), true);

    mySizedDynamicArray.reset(nullptr);

    EXPECT_EQ(it1.is_valid(), false);
    EXPECT_EQ(it2.is_valid(), false);
    EXPECT_EQ(cit1.is_valid(), false);
    EXPECT_EQ(cit2.is_valid(), false);
  }
}
