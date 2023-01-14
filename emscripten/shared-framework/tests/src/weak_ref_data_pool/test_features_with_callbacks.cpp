
#include "headers.hpp"

TEST(weak_ref_data_pool__features_with_callbacks, can_filter_pool) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref0 = myPool.acquire(0, "test");
  auto ref1 = myPool.acquire(1, "test");
  auto ref2 = myPool.acquire(2, "test");
  auto ref3 = myPool.acquire(3, "test");
  auto ref4 = myPool.acquire(4, "test");
  auto ref5 = myPool.acquire(5, "test");
  auto ref6 = myPool.acquire(6, "test");
  auto ref7 = myPool.acquire(7, "test");
  auto ref8 = myPool.acquire(8, "test");
  auto ref9 = myPool.acquire(9, "test");

  EXPECT_EQ(myPool.size(), 10);
  for (int ii = 0; ii < 10; ++ii) {
    EXPECT_EQ(myPool.get(ii).get()->value, ii);
    EXPECT_EQ(myPool.get(ii).get()->my_string, "test");
  }

  myPool.filter([](common::TestStructure& currValue) {
    return (currValue.value % 2 == 0);
  });

  EXPECT_EQ(myPool.size(), 5);

  EXPECT_EQ(ref0, true);
  EXPECT_EQ(ref0.is_active(), true);
  EXPECT_EQ(ref2, true);
  EXPECT_EQ(ref2.is_active(), true);
  EXPECT_EQ(ref4, true);
  EXPECT_EQ(ref4.is_active(), true);
  EXPECT_EQ(ref6, true);
  EXPECT_EQ(ref6.is_active(), true);
  EXPECT_EQ(ref8, true);
  EXPECT_EQ(ref8.is_active(), true);

  EXPECT_EQ(ref1, false);
  EXPECT_EQ(ref1.is_active(), false);
  EXPECT_EQ(ref3, false);
  EXPECT_EQ(ref3.is_active(), false);
  EXPECT_EQ(ref5, false);
  EXPECT_EQ(ref5.is_active(), false);
  EXPECT_EQ(ref7, false);
  EXPECT_EQ(ref7.is_active(), false);
  EXPECT_EQ(ref9, false);
  EXPECT_EQ(ref9.is_active(), false);

  EXPECT_EQ(myPool.get(0).get()->value, 0);
  EXPECT_EQ(myPool.get(0).get()->my_string, "test");
  EXPECT_EQ(myPool.get(1).get()->value, 8);
  EXPECT_EQ(myPool.get(1).get()->my_string, "test");
  EXPECT_EQ(myPool.get(2).get()->value, 2);
  EXPECT_EQ(myPool.get(2).get()->my_string, "test");
  EXPECT_EQ(myPool.get(3).get()->value, 6);
  EXPECT_EQ(myPool.get(3).get()->my_string, "test");
  EXPECT_EQ(myPool.get(4).get()->value, 4);
  EXPECT_EQ(myPool.get(4).get()->my_string, "test");
}

TEST(weak_ref_data_pool__features_with_callbacks, can_loop_pool) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref0 = myPool.acquire(0, "test");
  auto ref1 = myPool.acquire(1, "test");
  auto ref2 = myPool.acquire(2, "test");
  auto ref3 = myPool.acquire(3, "test");
  auto ref4 = myPool.acquire(4, "test");
  auto ref5 = myPool.acquire(5, "test");
  auto ref6 = myPool.acquire(6, "test");
  auto ref7 = myPool.acquire(7, "test");
  auto ref8 = myPool.acquire(8, "test");
  auto ref9 = myPool.acquire(9, "test");

  EXPECT_EQ(myPool.size(), 10);

  {
    int index = 0;
    myPool.for_each([&index](common::TestStructure& currValue) {
      EXPECT_EQ(currValue.value, index);
      EXPECT_EQ(currValue.my_string, "test");
      ++index;
    });
    EXPECT_EQ(index, 10);
  }

  {
    const auto& cmyPool = myPool;
    int index = 0;
    cmyPool.for_each([&index](const common::TestStructure& currValue) {
      EXPECT_EQ(currValue.value, index);
      EXPECT_EQ(currValue.my_string, "test");
      ++index;
    });
    EXPECT_EQ(index, 10);
  }
}

TEST(weak_ref_data_pool__features_with_callbacks, can_find_if) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref0 = myPool.acquire(0, "test");
  auto ref1 = myPool.acquire(1, "test");
  auto ref2 = myPool.acquire(2, "test");
  auto ref3 = myPool.acquire(3, "test");
  auto ref4 = myPool.acquire(4, "test");
  auto ref5 = myPool.acquire(5, "test");
  auto ref6 = myPool.acquire(6, "test");
  auto ref7 = myPool.acquire(7, "test");
  auto ref8 = myPool.acquire(8, "test");
  auto ref9 = myPool.acquire(9, "test");

  EXPECT_EQ(myPool.size(), 10);

  {
    auto foundRef =
      myPool.find_if([](const common::TestStructure& currValue) -> bool {
        return (currValue.value == 6);
      });
    EXPECT_EQ(foundRef.is_active(), true);
    EXPECT_EQ(foundRef, true);
    EXPECT_EQ(foundRef->value, 6);
    EXPECT_EQ(foundRef->my_string, "test");
  }

  {
    const auto& cmyPool = myPool;
    auto foundRef =
      cmyPool.find_if([](const common::TestStructure& currValue) -> bool {
        return (currValue.value == 6);
      });
    EXPECT_EQ(foundRef.is_active(), true);
    EXPECT_EQ(foundRef, true);
    EXPECT_EQ(foundRef->value, 6);
    EXPECT_EQ(foundRef->my_string, "test");
  }
}
