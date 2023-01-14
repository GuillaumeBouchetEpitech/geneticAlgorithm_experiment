
#include "common.hpp"

#include "framework/system/TraceLogger.hpp"

#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

namespace common {

int totalCtor = 0;
int totalCopyCtor = 0;
int totalMoveCtor = 0;
int totalDtor = 0;
int totalSwapApplied = 0;
int g_totalAlloc = 0;
int g_totalDealloc = 0;

std::mutex g_singleThread;

int getTotalCtor() { return totalCtor; }
int getTotalCopyCtor() { return totalCopyCtor; }
int getTotalMoveCtor() { return totalMoveCtor; }
int getTotalDtor() { return totalDtor; }
int getTotalSwapApplied() { return totalSwapApplied; }
int getTotalAlloc() { return g_totalAlloc; }
int getTotalDealloc() { return g_totalDealloc; }
void setTotalAlloc(int val) { g_totalAlloc = val; }
void setTotalDealloc(int val) { g_totalDealloc = val; }

void reset() {
  totalCtor = 0;
  totalCopyCtor = 0;
  totalMoveCtor = 0;
  totalDtor = 0;
  totalSwapApplied = 0;

  g_totalAlloc = 0;
  g_totalDealloc = 0;
}

void lockThread() { g_singleThread.lock(); }
void unlockThread() { g_singleThread.unlock(); }

TestStructure::TestStructure(int inValue /* = 0 */,
                             const char* inString /*= ""*/)
  : value(inValue), my_string(inString) {
  ++totalCtor;
}

TestStructure::~TestStructure() { ++totalDtor; }

TestStructure::TestStructure(const TestStructure& other) {
  ++totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
}

TestStructure::TestStructure(TestStructure&& other) {
  ++totalMoveCtor;
  value = std::move(other.value);
  my_string = std::move(other.my_string);
}

TestStructure& TestStructure::operator=(const TestStructure& other) {
  ++totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
  return *this;
}

TestStructure& TestStructure::operator=(TestStructure&& other) {
  ++totalMoveCtor;
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  return *this;
}

void TestStructure::applySwap(TestStructure& other) {
  ++totalSwapApplied;
  std::swap(value, other.value);
  std::swap(my_string, other.my_string);
}

} // namespace common
