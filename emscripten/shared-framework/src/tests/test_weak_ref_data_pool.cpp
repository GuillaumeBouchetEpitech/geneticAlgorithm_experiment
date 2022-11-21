
#include "framework/containers/weak_ref_data_pool.hpp"

#include <cassert>

#include "common.hpp"

void test_weak_ref_data_pool()
{
  std::cout << "test_weak_ref_data_pool()" << std::endl;

  weak_ref_data_pool<common::Test> myPool;

  // myPool.initialiseSize(5);
  myPool.reserve(10);

  // std::cout << "myPool.size()=" << myPool.size() << std::endl;

  auto ref1A = myPool.acquire(555);
  auto ref1B = ref1A;
  auto ref1C = ref1B;

  auto ref1D_moved = ref1C;
  auto ref1E = std::move(ref1D_moved);

  assert(myPool.size() == 1);
  assert(myPool.getIndex(ref1A) == 0);
  assert(ref1A.isActive() == true);
  assert(ref1B.isActive() == true);
  assert(ref1C.isActive() == true);
  assert(ref1D_moved.isActive() == false);
  assert(ref1E.isActive() == true);
  assert(ref1A == true);
  assert(ref1B == true);
  assert(ref1C == true);
  assert(ref1D_moved == false);
  assert(ref1E == true);
  assert(ref1A.get() == myPool.get(0).get());
  assert(ref1D_moved.get() == nullptr);
  assert(ref1A.get() == ref1B.get());
  assert(ref1A.get() == ref1C.get());
  assert(ref1A.get() == ref1E.get());

  // std::cout << "myPool.size()=" << myPool.size() << std::endl;

  auto ref2A = myPool.acquire(777777);
  assert(myPool.size() == 2);
  assert(myPool.getIndex(ref2A) == 1);
  assert(ref2A.get() == myPool.get(1).get());
  assert(ref2A.isActive() == true);
  assert(ref2A->value == 777777);

  // std::cout << "myPool.size()=" << myPool.size() << std::endl;

  assert(ref1A->value == 555);
  assert(ref1B->value == 555);
  assert(ref1C->value == 555);
  assert(ref1E->value == 555);

  ref1A->value = 666;

  assert(ref1A.isActive() == true);
  assert(ref1B.isActive() == true);
  assert(ref1C.isActive() == true);
  assert(ref1D_moved.isActive() == false);
  assert(ref1E.isActive() == true);

  assert(ref1A->value == 666);
  assert(ref1B->value == 666);
  assert(ref1C->value == 666);
  assert(ref1E->value == 666);

  // std::cout << "ref1A=" << ref1A->value << ", " << ref1A << ", " << ref1A.isActive() << std::endl;
  // std::cout << "ref1B=" << ref1B->value << ", " << ref1B << ", " << ref1B.isActive() << std::endl;
  // std::cout << "ref1C=" << ref1C->value << ", " << ref1C << ", " << ref1C.isActive() << std::endl;
  // std::cout << "ref1E=" << ref1E->value << ", " << ref1E << ", " << ref1E.isActive() << std::endl;

  // std::cout << "RELEASE REF1A" << std::endl;

  assert(ref1A.isActive() == true);

  myPool.release(ref1A);

  assert(ref1A.isActive() == false);
  assert(ref1B.isActive() == false);
  assert(ref1C.isActive() == false);
  assert(ref1D_moved.isActive() == false);
  assert(ref1E.isActive() == false);

  assert(myPool.size() == 1);
  assert(myPool.getIndex(ref2A) == 0);
  assert(ref2A.get() == myPool.get(0).get());
  assert(ref2A.isActive() == true);
  assert(ref2A->value == 777777);

  // std::cout << "ref1A=" << ref1A << std::endl;
  // std::cout << "ref1B=" << ref1B << std::endl;
  // std::cout << "ref1C=" << ref1C << std::endl;
  // std::cout << "ref1E=" << ref1E << std::endl;
  // std::cout << "ref2A=" << ref2A->value << ", " << ref2A << std::endl;

  myPool.clear();

  assert(myPool.size() == 0);
  assert(ref1A.isActive() == false);
  assert(ref1B.isActive() == false);
  assert(ref1C.isActive() == false);
  assert(ref1E.isActive() == false);
  assert(ref1A.get() == nullptr);
  assert(ref1B.get() == nullptr);
  assert(ref1C.get() == nullptr);
  assert(ref1E.get() == nullptr);

  assert(ref2A == false);
  assert(ref2A.get() == nullptr);

  {
    common::reset();

    for (int ii = 0; ii <= 10; ++ii)
      myPool.acquire(ii);

    assert(myPool.size() == 10);

    myPool.forEach([](common::Test& item)
    {

      // std::cout << " (" << item.value << " % 2)=" << ((item.value % 2) == 0) << std::endl;

      return ((item.value % 2) == 0); // keep only the even values
    });

    // std::cout << "myPool.size()=" << myPool.size() << std::endl;

    assert(myPool.size() == 5);
    assert(myPool.get(0).get()->value == 0);
    assert(myPool.get(1).get()->value == 8);
    assert(myPool.get(2).get()->value == 2);
    assert(myPool.get(3).get()->value == 6);
    assert(myPool.get(4).get()->value == 4);

    // std::cout << "myPool.size()=" << myPool.size() << std::endl;

    // print();

    assert(common::getTotalCtor() == 10);
    assert(common::getTotalCopyCtor() == 0);
    assert(common::getTotalMoveCtor() == 4);
    assert(common::getTotalDtor() == 5);

  }


  // std::cout << "myPool.size()=" << myPool.size() << std::endl;

  // std::cout << "ref1A=" << ref1A.get() << ", " << ref1A << std::endl;
  // std::cout << "ref1B=" << ref1B.get() << ", " << ref1B << std::endl;
  // std::cout << "ref1C=" << ref1C.get() << ", " << ref1C << std::endl;
  // std::cout << "ref1E=" << ref1E.get() << ", " << ref1E << std::endl;
  // std::cout << "ref2A=" << ref2A.get() << ", " << ref2A << std::endl;

  std::cout << " => DONE" << std::endl;
}