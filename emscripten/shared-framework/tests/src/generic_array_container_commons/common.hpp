
#pragma once

#include "gtest/gtest.h"

#include <string>

namespace common {

void reset();

int getTotalCtor();
int getTotalCopyCtor();
int getTotalMoveCtor();
int getTotalDtor();
int getTotalSwapApplied();

int getTotalAlloc();
int getTotalDealloc();
void setTotalAlloc(int val);
void setTotalDealloc(int val);

void lockThread();
void unlockThread();

//
//
//
//
//

struct TestStructure {
  int value = -1;

  std::string my_string;

  TestStructure(int inValue = 0, const char* inString = "");
  ~TestStructure();
  TestStructure(const TestStructure& other);
  TestStructure(TestStructure&& other);
  TestStructure& operator=(const TestStructure& other);
  TestStructure& operator=(TestStructure&& other);

  void applySwap(TestStructure& other);
};

//
//
//
//
//

template <typename T> struct MyAllocator {
  using value_type = T;

  MyAllocator() = default;
  template <class U> MyAllocator(const MyAllocator<U>&) {}

  T* allocate(std::size_t n) {
    setTotalAlloc(getTotalAlloc() + 1);
    if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      if (auto ptr = std::malloc(n * sizeof(T))) {
        return static_cast<T*>(ptr);
      }
    }
    throw std::bad_alloc();
  }
  void deallocate(T* ptr, std::size_t n) {
    setTotalDealloc(getTotalDealloc() + 1);
    static_cast<void>(n); // unused
    std::free(ptr);
  }
};

//
//
//
//
//

class threadsafe_fixture : public ::testing::Test {
protected:
  virtual void SetUp() override {
    lockThread();
    common::reset();
  }

  virtual void TearDown() override {
    common::reset();
    unlockThread();
  }
};

} // namespace common
