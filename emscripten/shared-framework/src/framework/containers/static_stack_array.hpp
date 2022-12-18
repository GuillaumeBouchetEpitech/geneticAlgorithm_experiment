#pragma once

#include "framework/ErrorHandler.hpp"

#include <string>

template <typename static_stack_array> class static_stack_array_iterator {
public:
  using ValueType = typename static_stack_array::ValueType;

private:
  static_stack_array& _container;
  int _index;

public:
  static_stack_array_iterator(static_stack_array& container, int index)
    : _container(container), _index(index) {}

  static_stack_array_iterator& operator++() // ++pre
  {
    if (_index < int(_container.size()))
      ++_index;
    return *this;
  }
  static_stack_array_iterator& operator++(int) // post++
  {
    static_stack_array_iterator copy = *this;
    ++(*this);
    return copy;
  }

  static_stack_array_iterator& operator--() // --pre
  {
    if (_index > 0)
      --_index;
    return *this;
  }
  static_stack_array_iterator& operator--(int) // post--
  {
    static_stack_array_iterator copy = *this;
    --(*this);
    return copy;
  }

  ValueType& operator[](int index) { return _container[_index + index]; }
  ValueType* operator->() { return &_container[_index]; }
  ValueType& operator*() { return _container[_index]; }

  bool operator==(const static_stack_array_iterator& other) const {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const static_stack_array_iterator& other) const {
    return !(*this == other);
  }
};

//
//
//
//
//

template <typename static_stack_array> class static_stack_array_const_iterator {
public:
  using ValueType = typename static_stack_array::ValueType;

private:
  const static_stack_array& _container;
  int _index;

public:
  static_stack_array_const_iterator(const static_stack_array& container,
                                    int index)
    : _container(container), _index(index) {}

  static_stack_array_const_iterator& operator++() // ++pre
  {
    if (_index < int(_container.size()))
      ++_index;
    return *this;
  }
  static_stack_array_const_iterator& operator++(int) // post++
  {
    static_stack_array_const_iterator copy = *this;
    ++(*this);
    return copy;
  }

  static_stack_array_const_iterator& operator--() // --pre
  {
    if (_index > 0)
      --_index;
    return *this;
  }
  static_stack_array_const_iterator& operator--(int) // post--
  {
    static_stack_array_const_iterator copy = *this;
    --(*this);
    return copy;
  }

  const ValueType& operator[](int index) const {
    return _container[_index + index];
  }
  const ValueType* operator->() const { return &_container[_index]; }
  const ValueType& operator*() const { return _container[_index]; }

  bool operator==(const static_stack_array_const_iterator& other) const {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const static_stack_array_const_iterator& other) const {
    return !(*this == other);
  }
};

//
//
//
//
//

template <typename Type, std::size_t _Size> class static_stack_array {
public:
  using ValueType = Type;
  using Iterator = static_stack_array_iterator<static_stack_array<Type, _Size>>;
  using ConstIterator =
    static_stack_array_const_iterator<static_stack_array<Type, _Size>>;

private:
  Type _data[_Size];

private:
  std::size_t _getIndex(int index) const {
    if (index < 0)
      index = int(_Size) - (-index) % int(_Size);
    if (index >= int(_Size))
      index = index % int(_Size);
    return std::size_t(index);
  }

public:
  Iterator begin() { return Iterator(*this, 0); }
  Iterator end() { return Iterator(*this, _Size); }

  ConstIterator begin() const { return ConstIterator(*this, 0); }
  ConstIterator end() const { return ConstIterator(*this, _Size); }

public:
  constexpr std::size_t size() const { return _Size; }
  bool is_out_of_range(std::size_t index) const { return (index >= _Size); }

public:
  const Type* data() const { return _data; }
  Type* data() { return _data; }

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

public:
  bool operator==(const static_stack_array& other) const {
    return this == &other;
  }
  bool operator!=(const static_stack_array& other) const {
    return !(*this == other);
  }
};
