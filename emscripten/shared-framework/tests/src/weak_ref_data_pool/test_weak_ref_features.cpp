
#include "headers.hpp"

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_the_values_from_it) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_copy_and_move_it) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy

  auto movedRef = copiedRef2;            // copy
  auto copiedRef3 = std::move(movedRef); // move

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);
  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(movedRef.is_active(), false);
  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(movedRef, false);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(movedRef.get(), nullptr);
  EXPECT_EQ(mainRef.get(), copiedRef1.get());
  EXPECT_EQ(mainRef.get(), copiedRef2.get());
  EXPECT_EQ(mainRef.get(), copiedRef3.get());

  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_and_set_the_values_of_it) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");

  ref->value += 100;
  ref->my_string += " test";

  EXPECT_EQ(ref->value, 655);
  EXPECT_EQ(ref->my_string, "test test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_total_ref_count_from_the_pool) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(ref), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");

  ref.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(ref.is_active(), false);
  EXPECT_EQ(ref, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_invalidate_it) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy
  auto copiedRef3 = copiedRef2; // copy

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef2), 0);
  EXPECT_EQ(myPool.get_index(copiedRef3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);

  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  mainRef.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 3);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef2.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 2);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef1.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 1);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef3.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 0);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features, can_acquire_weak_ref_and_release_it) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy
  auto copiedRef3 = copiedRef2; // copy

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef2), 0);
  EXPECT_EQ(myPool.get_index(copiedRef3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);

  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  myPool.release(mainRef);

  EXPECT_EQ(myPool.size(), 0);
  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);
  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);
  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);
  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_several_weak_ref_and_release_them_one_by_one) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto ref1 = myPool.acquire(555, "test");
  auto ref2 = myPool.acquire(666, "test");
  auto ref3 = myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);

  EXPECT_EQ(myPool.get_index(ref1), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref1.is_active(), true);
  EXPECT_EQ(ref1, true);
  EXPECT_EQ(ref1.get(), myPool.get(0).get());
  EXPECT_EQ(ref1->value, 555);
  EXPECT_EQ(ref1->my_string, "test");

  EXPECT_EQ(myPool.get_index(ref2), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(ref2.is_active(), true);
  EXPECT_EQ(ref2, true);
  EXPECT_EQ(ref2.get(), myPool.get(1).get());
  EXPECT_EQ(ref2->value, 666);
  EXPECT_EQ(ref2->my_string, "test");

  EXPECT_EQ(myPool.get_index(ref3), 2);
  EXPECT_EQ(myPool.get_ref_count(2), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(2).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref2);

  EXPECT_EQ(myPool.size(), 2);

  EXPECT_EQ(myPool.get_index(ref1), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref1.is_active(), true);
  EXPECT_EQ(ref1, true);
  EXPECT_EQ(ref1.get(), myPool.get(0).get());
  EXPECT_EQ(ref1->value, 555);
  EXPECT_EQ(ref1->my_string, "test");

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(myPool.get_index(ref3), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(1).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref1);

  EXPECT_EQ(myPool.size(), 1);

  EXPECT_EQ(ref1.is_active(), false);
  EXPECT_EQ(ref1, false);

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(myPool.get_index(ref3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(0).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref3);

  EXPECT_EQ(myPool.size(), 0);

  EXPECT_EQ(ref1.is_active(), false);
  EXPECT_EQ(ref1, false);

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(ref3.is_active(), false);
  EXPECT_EQ(ref3, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_several_weak_ref_and_clear_the_pool) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  auto mainRef1 = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef1; // copy

  auto mainRef2 = myPool.acquire(666, "test");
  auto copiedRef2 = mainRef2; // copy

  auto mainRef3 = myPool.acquire(777, "test");
  auto copiedRef3 = mainRef3; // copy

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_index(mainRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(mainRef2), 1);
  EXPECT_EQ(myPool.get_index(copiedRef2), 1);
  EXPECT_EQ(myPool.get_index(mainRef3), 2);
  EXPECT_EQ(myPool.get_index(copiedRef3), 2);
  EXPECT_EQ(myPool.get_ref_count(0), 2);
  EXPECT_EQ(myPool.get_ref_count(1), 2);
  EXPECT_EQ(myPool.get_ref_count(2), 2);

  EXPECT_EQ(mainRef1.is_active(), true);
  EXPECT_EQ(mainRef1, true);
  EXPECT_EQ(mainRef1.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef1->value, 555);
  EXPECT_EQ(mainRef1->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(mainRef2.is_active(), true);
  EXPECT_EQ(mainRef2, true);
  EXPECT_EQ(mainRef2.get(), myPool.get(1).get());
  EXPECT_EQ(mainRef2->value, 666);
  EXPECT_EQ(mainRef2->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(1).get());
  EXPECT_EQ(copiedRef2->value, 666);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(mainRef3.is_active(), true);
  EXPECT_EQ(mainRef3, true);
  EXPECT_EQ(mainRef3.get(), myPool.get(2).get());
  EXPECT_EQ(mainRef3->value, 777);
  EXPECT_EQ(mainRef3->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(2).get());
  EXPECT_EQ(copiedRef3->value, 777);
  EXPECT_EQ(copiedRef3->my_string, "test");

  myPool.clear();

  EXPECT_EQ(myPool.size(), 0);
  EXPECT_EQ(mainRef1.is_active(), false);
  EXPECT_EQ(mainRef1, false);
  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);
  EXPECT_EQ(mainRef2.is_active(), false);
  EXPECT_EQ(mainRef2, false);
  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);
  EXPECT_EQ(mainRef3.is_active(), false);
  EXPECT_EQ(mainRef3, false);
  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features, can_directly_access_pool_data) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  myPool.acquire(555, "test");
  myPool.acquire(666, "test");
  myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);

  EXPECT_EQ(myPool.get(0)->value, 555);
  EXPECT_EQ(myPool.get(0)->my_string, "test");
  EXPECT_EQ(myPool.get(1)->value, 666);
  EXPECT_EQ(myPool.get(1)->my_string, "test");
  EXPECT_EQ(myPool.get(2)->value, 777);
  EXPECT_EQ(myPool.get(2)->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features, can_prevent_pool_growth) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  for (int ii = 0; ii < 20; ++ii) {
    auto ref = myPool.acquire(ii);
    if (ii < 10) {
      EXPECT_EQ(ref.is_active(), true);
    } else {
      EXPECT_EQ(ref.is_active(), false);
    }
  }

  EXPECT_EQ(myPool.size(), 10);
  for (int ii = 0; ii < 10; ++ii)
    EXPECT_EQ(myPool.get(ii).get()->value, ii);
}

TEST(weak_ref_data_pool__refs_features, can_allow_pool_growth) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, false>
    myPool;

  for (int ii = 0; ii < 20; ++ii) {
    auto ref = myPool.acquire(ii);
    EXPECT_EQ(ref.is_active(), true);
  }

  EXPECT_EQ(myPool.size(), 20);
  for (int ii = 0; ii < 20; ++ii)
    EXPECT_EQ(myPool.get(ii).get()->value, ii);
}

TEST(weak_ref_data_pool__refs_features, can_move_data_into_a_weak_ref) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, false>
    myPool;

  auto mainRef = myPool.acquire(666, "test");

  EXPECT_EQ(mainRef->value, 666);
  EXPECT_EQ(mainRef->my_string, "test");

  common::TestStructure tmpData(777, "test test");
  *mainRef = std::move(tmpData);

  EXPECT_EQ(mainRef->value, 777);
  EXPECT_EQ(mainRef->my_string, "test test");
}

TEST(weak_ref_data_pool__refs_features, can_move_entire_pool) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, false>
    myPool1;
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, false>
    myPool2;

  for (int ii = 0; ii < 20; ++ii)
    myPool1.acquire(ii, "test");

  EXPECT_EQ(myPool1.size(), 20);
  EXPECT_EQ(myPool2.size(), 0);
  for (int ii = 0; ii < 20; ++ii) {
    EXPECT_EQ(myPool1.get(ii)->value, ii);
    EXPECT_EQ(myPool1.get(ii)->my_string, "test");
  }

  myPool2 = std::move(myPool1);

  EXPECT_EQ(myPool1.size(), 0);
  EXPECT_EQ(myPool2.size(), 20);
  for (int ii = 0; ii < 20; ++ii) {
    EXPECT_EQ(myPool2.get(ii)->value, ii);
    EXPECT_EQ(myPool2.get(ii)->my_string, "test");
  }
}

TEST(weak_ref_data_pool__refs_features,
     can_make_weak_ref_that_invalidate_once_out_of_scope) {
  weak_ref_data_pool<common::TestStructure, common::TestStructure, 10, true>
    myPool;

  myPool.acquire(555, "test");
  myPool.acquire(666, "test");
  myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);

  {
    auto ref1 = myPool.get(0);
    auto ref2 = myPool.get(1);
    auto ref3 = myPool.get(2);

    EXPECT_EQ(myPool.get_ref_count(0), 1);
    EXPECT_EQ(myPool.get_ref_count(1), 1);
    EXPECT_EQ(myPool.get_ref_count(2), 1);
  }

  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);
}
