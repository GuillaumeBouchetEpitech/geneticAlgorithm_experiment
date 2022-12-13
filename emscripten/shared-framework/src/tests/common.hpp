
#pragma once

#include <string>

namespace common {

void reset();

int getTotalCtor();
int getTotalCopyCtor();
int getTotalMoveCtor();
int getTotalDtor();
void enableLogs();
void disableLogs();

// void print();

struct Test {
  int value = -1;

  std::string my_string;

  Test(int inValue = 0);
  ~Test();
  Test(const Test& other);
  Test(Test&& other);
  Test& operator=(const Test& other);
  Test& operator=(Test&& other);
};

struct Test2 {
  int value = -2;

  std::string my_string;

  Test2() = default;

  Test2(Test2&& other);
  Test2& operator=(Test2&& other);
};

} // namespace common
