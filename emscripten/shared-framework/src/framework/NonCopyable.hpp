
#pragma once

class NonCopyable {
public:
  NonCopyable() = default;
  virtual ~NonCopyable() = default;

  // disallow copy/move constructor
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = delete;

  // disallow assignment operator
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable& operator=(NonCopyable&&) = delete;
};
