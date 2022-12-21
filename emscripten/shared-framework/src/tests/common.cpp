
#include "common.hpp"

#include "framework/system/TraceLogger.hpp"

#include <functional>
#include <iostream>
#include <vector>

// #include <cstring> // <= memset
// #include <cstdlib> // <= EXIT_SUCCESS

#include <cassert>

namespace common {

bool logEnabled = false;

int totalCtor = 0;
int totalCopyCtor = 0;
int totalMoveCtor = 0;
int totalDtor = 0;

int getTotalCtor() { return totalCtor; }
int getTotalCopyCtor() { return totalCopyCtor; }
int getTotalMoveCtor() { return totalMoveCtor; }
int getTotalDtor() { return totalDtor; }
void enableLogs() { logEnabled = true; }
void disableLogs() { logEnabled = false; }

void reset() {
  totalCtor = 0;
  totalCopyCtor = 0;
  totalMoveCtor = 0;
  totalDtor = 0;
}

void print() {
  std::cout << " => totalCtor=     " << totalCtor << std::endl;
  std::cout << " => totalCopyCtor= " << totalCopyCtor << std::endl;
  std::cout << " => totalMoveCtor= " << totalMoveCtor << std::endl;
  std::cout << " => totalDtor=     " << totalDtor << std::endl;
}

Test::Test(int inValue /* = 0 */) : value(inValue) {
  ++totalCtor;
  if (logEnabled)
    std::cout << "ctor " << value << std::endl;
}

Test::~Test() {
  ++totalDtor;
  if (logEnabled)
    std::cout << "dtor " << value << std::endl;
}

Test::Test(const Test& other) {
  ++totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
  if (logEnabled)
    std::cout << "ctor copy " << value << std::endl;
}

Test::Test(Test&& other) {

  if (logEnabled) {
    std::cout << "ctor move" << std::endl;
    std::cout << " -> " << value << std::endl;
    std::cout << " -> " << other.value << std::endl;
  }

  ++totalMoveCtor;
  // value = std::move(other.value);
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  if (logEnabled) {
    std::cout << "ctor move " << value << ", " << other.value << std::endl;
  }
}

Test& Test::operator=(const Test& other) {
  ++totalCopyCtor;
  value = other.value;
  my_string = other.my_string;
  if (logEnabled)
    std::cout << "op= copy " << value << std::endl;
  return *this;
}

Test& Test::operator=(Test&& other) {
  if (logEnabled) {
    std::cout << "op= move" << std::endl;
    std::cout << " -> " << value << std::endl;
    std::cout << " -> " << other.value << std::endl;
  }

  ++totalMoveCtor;
  // value = std::move(other.value);
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  if (logEnabled)
    std::cout << "op= move " << value << ", " << other.value << std::endl;
  return *this;
}

//
//
//
//
//

Test2::Test2(Test2&& other) {
  D_MYLOG("value " << value);
  D_MYLOG("other.value " << other.value);

  // value = std::move(other.value);
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
}

Test2& Test2::operator=(Test2&& other) {
  D_MYLOG("value " << value);
  D_MYLOG("other.value " << other.value);

  // value = std::move(other.value);
  std::swap(value, other.value);
  my_string = std::move(other.my_string);
  return *this;
}

} // namespace common
