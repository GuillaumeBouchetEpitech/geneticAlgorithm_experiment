
#pragma once

namespace common
{

  void reset();

  int getTotalCtor();
  int getTotalCopyCtor();
  int getTotalMoveCtor();
  int getTotalDtor();
  void enableLogs();
  void disableLogs();

  // void print();

  struct Test
  {
    int value;

    Test(int inValue = 0);
    ~Test();
    Test(const Test& other);
    Test(Test&& other);
    Test& operator=(const Test& other);
    Test& operator=(Test&& other);
  };

}
