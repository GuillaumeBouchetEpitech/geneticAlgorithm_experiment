
#pragma once

#include "framework/system/ErrorHandler.hpp"

#include <vector>

template <typename Type> class static_buffer_view {
public:
  using ValueType = Type;
  // using Iterator = dynamic_heap_array_iterator<
  //   dynamic_heap_array<Type, Allocator, initial_size>>;
  // using ConstIterator = dynamic_heap_array_const_iterator<
  //   dynamic_heap_array<Type, Allocator, initial_size>>;

private:
  std::size_t _size = 0;
  Type* _data = nullptr;

public:
  static_buffer_view(Type* inData, std::size_t inSize)
    : _data(inData), _size(inSize) {}

  static_buffer_view(std::vector<Type> inData)
    : _data(inData.data()), _size(inData.size()) {}

private:
  std::size_t _getIndex(int index) const {
    if (index < 0)
      index = int(_size) - (-index) % int(_size);
    if (index >= int(_size))
      index = index % int(_size);
    return std::size_t(index);
  }

public:
  std::size_t size() const { return _size; }
  bool is_out_of_range(std::size_t index) const { return (index >= _size); }
  bool is_valid() const { return _size == 0 || _data == nullptr; }

public:
  const Type& operator[](int index) const { return _data[_getIndex(index)]; }
  Type& operator[](int index) { return _data[_getIndex(index)]; }

  const Type& at(std::size_t index) const {
    if (is_out_of_range(index))
      D_THROW(std::runtime_error, "out of range, index: " << index);
    return _data[index];
  }
  Type& at(std::size_t index) {
    if (is_out_of_range(index))
      D_THROW(std::runtime_error, "out of range, index: " << index);
    return _data[index];
  }

  const Type& front() const { return _data[0]; }
  Type& front() { return _data[0]; }

  const Type& back() const { return _data[_size - 1]; }
  Type& back() { return _data[_size - 1]; }

public:
  bool operator==(const dynamic_heap_array& other) const {
    return this == &other;
  }
  bool operator!=(const dynamic_heap_array& other) const {
    return !(*this == other);
  }
};