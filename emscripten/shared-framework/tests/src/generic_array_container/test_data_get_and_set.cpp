
#include "headers.hpp"

#include "generic_array_container_commons/common.hpp"

TEST(generic_array_container_get_and_set_values,
     can_be_for_looped_and_set_and_then_get_values_afterward) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    {
      int totalIterations = 0;
      for (std::size_t ii = 0; ii < mySaticArray.size(); ++ii) {
        mySaticArray[int(ii)].value = int(ii);
        mySaticArray[int(ii)].my_string = "test";
        ++totalIterations;
      }
      EXPECT_EQ(totalIterations, 5);
    }

    {
      int totalIterations = 0;
      for (std::size_t ii = 0; ii < mySaticArray.size(); ++ii) {
        EXPECT_EQ(mySaticArray[int(ii)].value, int(ii));
        EXPECT_EQ(mySaticArray[int(ii)].my_string, "test");
        ++totalIterations;
      }
      EXPECT_EQ(totalIterations, 5);
    }
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    {
      int totalIterations = 0;
      for (std::size_t ii = 0; ii < mySizedDynamicArray.size(); ++ii) {
        mySizedDynamicArray[int(ii)].value = int(ii);
        mySizedDynamicArray[int(ii)].my_string = "test";
        ++totalIterations;
      }
      EXPECT_EQ(totalIterations, 5);
    }

    {
      int totalIterations = 0;
      for (std::size_t ii = 0; ii < mySizedDynamicArray.size(); ++ii) {
        EXPECT_EQ(mySizedDynamicArray[int(ii)].value, int(ii));
        EXPECT_EQ(mySizedDynamicArray[int(ii)].my_string, "test");
        ++totalIterations;
      }
      EXPECT_EQ(totalIterations, 5);
    }
  }
}

TEST(generic_array_container_get_and_set_values,
     can_access_out_of_range_indexed_values) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySaticArray.size(); ++ii) {
      mySaticArray[int(ii)].value = int(ii);
      mySaticArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    EXPECT_EQ(mySaticArray[-5].value, 0);
    EXPECT_EQ(mySaticArray[-5].my_string, "test");
    EXPECT_EQ(mySaticArray[-4].value, 1);
    EXPECT_EQ(mySaticArray[-4].my_string, "test");
    EXPECT_EQ(mySaticArray[-3].value, 2);
    EXPECT_EQ(mySaticArray[-3].my_string, "test");
    EXPECT_EQ(mySaticArray[-2].value, 3);
    EXPECT_EQ(mySaticArray[-2].my_string, "test");
    EXPECT_EQ(mySaticArray[-1].value, 4);
    EXPECT_EQ(mySaticArray[-1].my_string, "test");

    EXPECT_EQ(mySaticArray[0].value, 0);
    EXPECT_EQ(mySaticArray[0].my_string, "test");
    EXPECT_EQ(mySaticArray[1].value, 1);
    EXPECT_EQ(mySaticArray[1].my_string, "test");
    EXPECT_EQ(mySaticArray[2].value, 2);
    EXPECT_EQ(mySaticArray[2].my_string, "test");
    EXPECT_EQ(mySaticArray[3].value, 3);
    EXPECT_EQ(mySaticArray[3].my_string, "test");
    EXPECT_EQ(mySaticArray[4].value, 4);
    EXPECT_EQ(mySaticArray[4].my_string, "test");

    EXPECT_EQ(mySaticArray[5].value, 0);
    EXPECT_EQ(mySaticArray[5].my_string, "test");
    EXPECT_EQ(mySaticArray[6].value, 1);
    EXPECT_EQ(mySaticArray[6].my_string, "test");
    EXPECT_EQ(mySaticArray[7].value, 2);
    EXPECT_EQ(mySaticArray[7].my_string, "test");
    EXPECT_EQ(mySaticArray[8].value, 3);
    EXPECT_EQ(mySaticArray[8].my_string, "test");
    EXPECT_EQ(mySaticArray[9].value, 4);
    EXPECT_EQ(mySaticArray[9].my_string, "test");

    const auto& cmySaticArray = mySaticArray;

    EXPECT_EQ(cmySaticArray[-5].value, 0);
    EXPECT_EQ(cmySaticArray[-5].my_string, "test");
    EXPECT_EQ(cmySaticArray[-4].value, 1);
    EXPECT_EQ(cmySaticArray[-4].my_string, "test");
    EXPECT_EQ(cmySaticArray[-3].value, 2);
    EXPECT_EQ(cmySaticArray[-3].my_string, "test");
    EXPECT_EQ(cmySaticArray[-2].value, 3);
    EXPECT_EQ(cmySaticArray[-2].my_string, "test");
    EXPECT_EQ(cmySaticArray[-1].value, 4);
    EXPECT_EQ(cmySaticArray[-1].my_string, "test");

    EXPECT_EQ(cmySaticArray[0].value, 0);
    EXPECT_EQ(cmySaticArray[0].my_string, "test");
    EXPECT_EQ(cmySaticArray[1].value, 1);
    EXPECT_EQ(cmySaticArray[1].my_string, "test");
    EXPECT_EQ(cmySaticArray[2].value, 2);
    EXPECT_EQ(cmySaticArray[2].my_string, "test");
    EXPECT_EQ(cmySaticArray[3].value, 3);
    EXPECT_EQ(cmySaticArray[3].my_string, "test");
    EXPECT_EQ(cmySaticArray[4].value, 4);
    EXPECT_EQ(cmySaticArray[4].my_string, "test");

    EXPECT_EQ(cmySaticArray[5].value, 0);
    EXPECT_EQ(cmySaticArray[5].my_string, "test");
    EXPECT_EQ(cmySaticArray[6].value, 1);
    EXPECT_EQ(cmySaticArray[6].my_string, "test");
    EXPECT_EQ(cmySaticArray[7].value, 2);
    EXPECT_EQ(cmySaticArray[7].my_string, "test");
    EXPECT_EQ(cmySaticArray[8].value, 3);
    EXPECT_EQ(cmySaticArray[8].my_string, "test");
    EXPECT_EQ(cmySaticArray[9].value, 4);
    EXPECT_EQ(cmySaticArray[9].my_string, "test");
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySizedDynamicArray.size(); ++ii) {
      mySizedDynamicArray[int(ii)].value = int(ii);
      mySizedDynamicArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    EXPECT_EQ(mySizedDynamicArray[-5].value, 0);
    EXPECT_EQ(mySizedDynamicArray[-5].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[-4].value, 1);
    EXPECT_EQ(mySizedDynamicArray[-4].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[-3].value, 2);
    EXPECT_EQ(mySizedDynamicArray[-3].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[-2].value, 3);
    EXPECT_EQ(mySizedDynamicArray[-2].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[-1].value, 4);
    EXPECT_EQ(mySizedDynamicArray[-1].my_string, "test");

    EXPECT_EQ(mySizedDynamicArray[0].value, 0);
    EXPECT_EQ(mySizedDynamicArray[0].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[1].value, 1);
    EXPECT_EQ(mySizedDynamicArray[1].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[2].value, 2);
    EXPECT_EQ(mySizedDynamicArray[2].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[3].value, 3);
    EXPECT_EQ(mySizedDynamicArray[3].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[4].value, 4);
    EXPECT_EQ(mySizedDynamicArray[4].my_string, "test");

    EXPECT_EQ(mySizedDynamicArray[5].value, 0);
    EXPECT_EQ(mySizedDynamicArray[5].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[6].value, 1);
    EXPECT_EQ(mySizedDynamicArray[6].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[7].value, 2);
    EXPECT_EQ(mySizedDynamicArray[7].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[8].value, 3);
    EXPECT_EQ(mySizedDynamicArray[8].my_string, "test");
    EXPECT_EQ(mySizedDynamicArray[9].value, 4);
    EXPECT_EQ(mySizedDynamicArray[9].my_string, "test");

    const auto& cmySizedDynamicArray = mySizedDynamicArray;

    EXPECT_EQ(cmySizedDynamicArray[-5].value, 0);
    EXPECT_EQ(cmySizedDynamicArray[-5].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[-4].value, 1);
    EXPECT_EQ(cmySizedDynamicArray[-4].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[-3].value, 2);
    EXPECT_EQ(cmySizedDynamicArray[-3].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[-2].value, 3);
    EXPECT_EQ(cmySizedDynamicArray[-2].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[-1].value, 4);
    EXPECT_EQ(cmySizedDynamicArray[-1].my_string, "test");

    EXPECT_EQ(cmySizedDynamicArray[0].value, 0);
    EXPECT_EQ(cmySizedDynamicArray[0].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[1].value, 1);
    EXPECT_EQ(cmySizedDynamicArray[1].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[2].value, 2);
    EXPECT_EQ(cmySizedDynamicArray[2].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[3].value, 3);
    EXPECT_EQ(cmySizedDynamicArray[3].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[4].value, 4);
    EXPECT_EQ(cmySizedDynamicArray[4].my_string, "test");

    EXPECT_EQ(cmySizedDynamicArray[5].value, 0);
    EXPECT_EQ(cmySizedDynamicArray[5].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[6].value, 1);
    EXPECT_EQ(cmySizedDynamicArray[6].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[7].value, 2);
    EXPECT_EQ(cmySizedDynamicArray[7].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[8].value, 3);
    EXPECT_EQ(cmySizedDynamicArray[8].my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray[9].value, 4);
    EXPECT_EQ(cmySizedDynamicArray[9].my_string, "test");
  }
}

TEST(generic_array_container_get_and_set_values,
     can_set_and_get_front_and_back_values) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySaticArray.size(); ++ii) {
      mySaticArray[int(ii)].value = int(ii);
      mySaticArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    const auto& cmySaticArray = mySaticArray;
    EXPECT_EQ(mySaticArray.front().value, 0);
    EXPECT_EQ(mySaticArray.front().my_string, "test");
    EXPECT_EQ(cmySaticArray.front().value, 0);
    EXPECT_EQ(cmySaticArray.front().my_string, "test");
    mySaticArray.front().value += 666;
    mySaticArray.front().my_string += " test";
    EXPECT_EQ(mySaticArray.front().value, 666);
    EXPECT_EQ(mySaticArray.front().my_string, "test test");
    EXPECT_EQ(cmySaticArray.front().value, 666);
    EXPECT_EQ(cmySaticArray.front().my_string, "test test");

    EXPECT_EQ(mySaticArray.back().value, 4);
    EXPECT_EQ(mySaticArray.back().my_string, "test");
    EXPECT_EQ(cmySaticArray.back().value, 4);
    EXPECT_EQ(cmySaticArray.back().my_string, "test");
    mySaticArray.back().value += 666;
    mySaticArray.back().my_string += " test";
    EXPECT_EQ(mySaticArray.back().value, 670);
    EXPECT_EQ(mySaticArray.back().my_string, "test test");
    EXPECT_EQ(cmySaticArray.back().value, 670);
    EXPECT_EQ(cmySaticArray.back().my_string, "test test");
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySizedDynamicArray.size(); ++ii) {
      mySizedDynamicArray[int(ii)].value = int(ii);
      mySizedDynamicArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    const auto& cmySizedDynamicArray = mySizedDynamicArray;
    EXPECT_EQ(mySizedDynamicArray.front().value, 0);
    EXPECT_EQ(mySizedDynamicArray.front().my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.front().value, 0);
    EXPECT_EQ(cmySizedDynamicArray.front().my_string, "test");
    mySizedDynamicArray.front().value += 666;
    mySizedDynamicArray.front().my_string += " test";
    EXPECT_EQ(mySizedDynamicArray.front().value, 666);
    EXPECT_EQ(mySizedDynamicArray.front().my_string, "test test");
    EXPECT_EQ(cmySizedDynamicArray.front().value, 666);
    EXPECT_EQ(cmySizedDynamicArray.front().my_string, "test test");

    EXPECT_EQ(mySizedDynamicArray.back().value, 4);
    EXPECT_EQ(mySizedDynamicArray.back().my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.back().value, 4);
    EXPECT_EQ(cmySizedDynamicArray.back().my_string, "test");
    mySizedDynamicArray.back().value += 666;
    mySizedDynamicArray.back().my_string += " test";
    EXPECT_EQ(mySizedDynamicArray.back().value, 670);
    EXPECT_EQ(mySizedDynamicArray.back().my_string, "test test");
    EXPECT_EQ(cmySizedDynamicArray.back().value, 670);
    EXPECT_EQ(cmySizedDynamicArray.back().my_string, "test test");
  }
}

TEST(generic_array_container_get_and_set_values,
     can_set_and_get_range_checked_values) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySaticArray.size(); ++ii) {
      mySaticArray[int(ii)].value = int(ii);
      mySaticArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    EXPECT_EQ(mySaticArray.at(0).value, 0);
    EXPECT_EQ(mySaticArray.at(0).my_string, "test");
    EXPECT_EQ(mySaticArray.at(1).value, 1);
    EXPECT_EQ(mySaticArray.at(0).my_string, "test");
    EXPECT_EQ(mySaticArray.at(2).value, 2);
    EXPECT_EQ(mySaticArray.at(0).my_string, "test");
    EXPECT_EQ(mySaticArray.at(3).value, 3);
    EXPECT_EQ(mySaticArray.at(0).my_string, "test");
    EXPECT_EQ(mySaticArray.at(4).value, 4);
    EXPECT_EQ(mySaticArray.at(0).my_string, "test");

    const auto& cmySaticArray = mySaticArray;

    EXPECT_EQ(cmySaticArray.at(0).value, 0);
    EXPECT_EQ(cmySaticArray.at(0).my_string, "test");
    EXPECT_EQ(cmySaticArray.at(1).value, 1);
    EXPECT_EQ(cmySaticArray.at(1).my_string, "test");
    EXPECT_EQ(cmySaticArray.at(2).value, 2);
    EXPECT_EQ(cmySaticArray.at(2).my_string, "test");
    EXPECT_EQ(cmySaticArray.at(3).value, 3);
    EXPECT_EQ(cmySaticArray.at(3).my_string, "test");
    EXPECT_EQ(cmySaticArray.at(4).value, 4);
    EXPECT_EQ(cmySaticArray.at(4).my_string, "test");
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    int totalIterations = 0;
    for (std::size_t ii = 0; ii < mySizedDynamicArray.size(); ++ii) {
      mySizedDynamicArray[int(ii)].value = int(ii);
      mySizedDynamicArray[int(ii)].my_string = "test";
      ++totalIterations;
    }
    EXPECT_EQ(totalIterations, 5);

    EXPECT_EQ(mySizedDynamicArray.at(0).value, 0);
    EXPECT_EQ(mySizedDynamicArray.at(0).my_string, "test");
    EXPECT_EQ(mySizedDynamicArray.at(1).value, 1);
    EXPECT_EQ(mySizedDynamicArray.at(1).my_string, "test");
    EXPECT_EQ(mySizedDynamicArray.at(2).value, 2);
    EXPECT_EQ(mySizedDynamicArray.at(2).my_string, "test");
    EXPECT_EQ(mySizedDynamicArray.at(3).value, 3);
    EXPECT_EQ(mySizedDynamicArray.at(3).my_string, "test");
    EXPECT_EQ(mySizedDynamicArray.at(4).value, 4);
    EXPECT_EQ(mySizedDynamicArray.at(4).my_string, "test");

    const auto& cmySizedDynamicArray = mySizedDynamicArray;

    EXPECT_EQ(cmySizedDynamicArray.at(0).value, 0);
    EXPECT_EQ(cmySizedDynamicArray.at(0).my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.at(1).value, 1);
    EXPECT_EQ(cmySizedDynamicArray.at(1).my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.at(2).value, 2);
    EXPECT_EQ(cmySizedDynamicArray.at(2).my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.at(3).value, 3);
    EXPECT_EQ(cmySizedDynamicArray.at(3).my_string, "test");
    EXPECT_EQ(cmySizedDynamicArray.at(4).value, 4);
    EXPECT_EQ(cmySizedDynamicArray.at(4).my_string, "test");
  }
}

TEST(generic_array_container_get_and_set_values,
     can_check_if_index_is_out_of_range) {
  {
    static_array<common::TestStructure, 5> mySaticArray;

    assert(!mySaticArray.is_out_of_range(0));
    assert(!mySaticArray.is_out_of_range(1));
    assert(!mySaticArray.is_out_of_range(2));
    assert(!mySaticArray.is_out_of_range(3));
    assert(!mySaticArray.is_out_of_range(4));
    assert(mySaticArray.is_out_of_range(5));
    assert(mySaticArray.is_out_of_range(6));
    assert(mySaticArray.is_out_of_range(7));

    const auto& cmySaticArray = mySaticArray;

    assert(!cmySaticArray.is_out_of_range(0));
    assert(!cmySaticArray.is_out_of_range(1));
    assert(!cmySaticArray.is_out_of_range(2));
    assert(!cmySaticArray.is_out_of_range(3));
    assert(!cmySaticArray.is_out_of_range(4));
    assert(cmySaticArray.is_out_of_range(5));
    assert(cmySaticArray.is_out_of_range(6));
    assert(cmySaticArray.is_out_of_range(7));
  }

  {
    shorthand_dynamic_heap_array<5> mySizedDynamicArray;
    mySizedDynamicArray.ensure_size(5);

    assert(!mySizedDynamicArray.is_out_of_range(0));
    assert(!mySizedDynamicArray.is_out_of_range(1));
    assert(!mySizedDynamicArray.is_out_of_range(2));
    assert(!mySizedDynamicArray.is_out_of_range(3));
    assert(!mySizedDynamicArray.is_out_of_range(4));
    assert(mySizedDynamicArray.is_out_of_range(5));
    assert(mySizedDynamicArray.is_out_of_range(6));
    assert(mySizedDynamicArray.is_out_of_range(7));

    const auto& cmySizedDynamicArray = mySizedDynamicArray;

    assert(!cmySizedDynamicArray.is_out_of_range(0));
    assert(!cmySizedDynamicArray.is_out_of_range(1));
    assert(!cmySizedDynamicArray.is_out_of_range(2));
    assert(!cmySizedDynamicArray.is_out_of_range(3));
    assert(!cmySizedDynamicArray.is_out_of_range(4));
    assert(cmySizedDynamicArray.is_out_of_range(5));
    assert(cmySizedDynamicArray.is_out_of_range(6));
    assert(cmySizedDynamicArray.is_out_of_range(7));
  }
}
