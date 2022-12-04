
#pragma once

#include "framework/ErrorHandler.hpp"

#include <string>
#include <memory>
#include <iostream>

template<typename dynamic_heap_array>
class dynamic_heap_array_iterator
{
public:
  using ValueType = typename dynamic_heap_array::ValueType;

private:
  dynamic_heap_array& _container;
  int _index;

public:
  dynamic_heap_array_iterator(dynamic_heap_array& container, int index)
    : _container(container)
    , _index(index)
  {}

  dynamic_heap_array_iterator& operator++() // ++pre
  {
    if (_index < int(_container.size()))
      ++_index;
    return *this;
  }
  dynamic_heap_array_iterator& operator++(int) // post++
  {
    dynamic_heap_array_iterator copy = *this;
    ++(*this);
    return copy;
  }

  dynamic_heap_array_iterator& operator--() // --pre
  {
    if (_index > 0)
      --_index;
    return *this;
  }
  dynamic_heap_array_iterator& operator--(int) // post--
  {
    dynamic_heap_array_iterator copy = *this;
    --(*this);
    return copy;
  }

  ValueType& operator[](int index) { return _container[_index + index]; }
  ValueType* operator->() { return &_container[_index]; }
  ValueType& operator*() { return _container[_index]; }

  bool operator==(const dynamic_heap_array_iterator& other) const
  {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const dynamic_heap_array_iterator& other) const
  {
    return !(*this == other);
  }
};

//
//
//
//
//


template<typename dynamic_heap_array>
class dynamic_heap_array_const_iterator
{
public:
  using ValueType = typename dynamic_heap_array::ValueType;

private:
  const dynamic_heap_array& _container;
  int _index;

public:
  dynamic_heap_array_const_iterator(const dynamic_heap_array& container, int index)
    : _container(container)
    , _index(index)
  {}

  dynamic_heap_array_const_iterator& operator++() // ++pre
  {
    if (_index < int(_container.size()))
      ++_index;
    return *this;
  }
  dynamic_heap_array_const_iterator& operator++(int) // post++
  {
    dynamic_heap_array_const_iterator copy = *this;
    ++(*this);
    return copy;
  }

  dynamic_heap_array_const_iterator& operator--() // --pre
  {
    if (_index > 0)
      --_index;
    return *this;
  }
  dynamic_heap_array_const_iterator& operator--(int) // post--
  {
    dynamic_heap_array_const_iterator copy = *this;
    --(*this);
    return copy;
  }

  const ValueType& operator[](int index) const { return _container[_index + index]; }
  const ValueType* operator->() const { return &_container[_index]; }
  const ValueType& operator*() const { return _container[_index]; }

  bool operator==(const dynamic_heap_array_const_iterator& other) const
  {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const dynamic_heap_array_const_iterator& other) const
  {
    return !(*this == other);
  }
};

//
//
//
//
//

template<typename Type, typename Allocator = std::allocator<Type>, std::size_t initial_size = 256>
class dynamic_heap_array
{
public:
  using ValueType = Type;
  using Iterator = dynamic_heap_array_iterator<dynamic_heap_array<Type, Allocator, initial_size>>;
  using ConstIterator = dynamic_heap_array_const_iterator<dynamic_heap_array<Type, Allocator, initial_size>>;

private:
  using traits_t = std::allocator_traits<Allocator>; // The matching trait


private:
  std::size_t _capacity = 0;
  std::size_t _size = 0;
  Type* _data = nullptr;

private:

  // allocate memory only, will not call any constructor
  Type* allocateMemory(std::size_t size)
  {
    // Type* newData = (Type*) ::operator new (size * sizeof(Type));

    Allocator alloc;
    Type* newData = alloc.allocate(size);

    return newData;
  }

  // deallocate memory only, will not call any destructor
  void deallocateMemory(Type* data, std::size_t size)
  {
    // ::operator delete(data, size * sizeof(Type));

    Allocator alloc;
    alloc.deallocate(data, size);
  }

  // call the move constructor only, do not allocate memory
  void callMoveConstructor(Type* data, std::size_t index, Type&& value)
  {
    // new (&data[index]) Type(std::move(value));

    Allocator alloc;
    traits_t::construct(alloc, data + index, std::move(value));
  }

  // call the move constructor only, do not allocate memory
  template<typename...Args>
  Type& emplaceMoveConstructor(Type* data, std::size_t index, Args&&... args)
  {
    // new (&data[index]) Type(std::forward<Args>(args)...);

    Allocator alloc;
    traits_t::construct(alloc, data + index, std::forward<Args>(args)...);
    return data[index];
  }

  // call the destructor only, do not deallocate memory
  void callDestructor(std::size_t index)
  {
    // _data[index].~Type();

    Allocator alloc;
    traits_t::destroy(alloc, _data + index);
  }

public:
  dynamic_heap_array()
  {
    if (initial_size > 0)
    {
      pre_allocate(initial_size);
    }
  }

  // disable copy
  dynamic_heap_array(const dynamic_heap_array& other) = delete;
  dynamic_heap_array& operator=(const dynamic_heap_array& other) = delete;
  // disable copy

  dynamic_heap_array(dynamic_heap_array&& other)
  {
    std::swap(_capacity, other._capacity);
    std::swap(_size, other._size);
    std::swap(_data, other._data);
  }

  dynamic_heap_array& operator=(dynamic_heap_array&& other)
  {
    std::swap(_capacity, other._capacity);
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
  }

  ~dynamic_heap_array()
  {
    clear();
    deallocateMemory(_data, _capacity);
  }


private:
  std::size_t _getIndex(int index) const
  {
    if (index < 0)
      index = int(_size) - (-index) % int(_size);
    if (index >= int(_size))
      index = index % int(_size);
    return std::size_t(index);
  }

  // std::size_t _getIndex(std::size_t index) const
  // {
  //   return std::size_t(index) % _size;
  // }

public:
  Iterator begin() { return Iterator(*this, 0); }
  Iterator end() { return Iterator(*this, int(_size)); }

  ConstIterator begin() const { return ConstIterator(*this, 0); }
  ConstIterator end() const { return ConstIterator(*this, int(_size)); }

  void push_back(const Type& value)
  {
    if (_size == _capacity)
      _realloc(_capacity * 2);

    _data[_size++] = value; // copy
  }

  void push_back(Type&& value)
  {
    if (_size == _capacity)
      _realloc(_capacity * 2);

    callMoveConstructor(_data, _size, std::move(value));

    ++_size;
  }

  template<typename...Args>
  Type& emplace_back(Args&&... args)
  {
    if (_size == _capacity)
      _realloc(_capacity * 2);

    Type& result = emplaceMoveConstructor(_data, _size, std::forward<Args>(args)...);

    ++_size;

    return result;
  }

  void pop_back()
  {
    if (_size == 0)
      return;

    --_size;

    callDestructor(_size);
  }

  void clear()
  {
    for (std::size_t ii = 0; ii < _size; ++ii)
      callDestructor(ii);

    _size = 0;
  }

  void pre_allocate(std::size_t capacity)
  {
    _realloc(capacity);
  }

  void unsorted_erase(std::size_t index)
  {
    if (_size > 1 && index + 1 < _size)
    {
      // move target to the back
      callMoveConstructor(_data, index, std::move(_data[_size - 1]));
    }
    // remove the back
    pop_back();
  }

  void sorted_erase(std::size_t index)
  {
    // call target destructor
    callDestructor(index);

    // move data after the target
    for (std::size_t ii = index; ii < _size; ++ii)
      callMoveConstructor(_data, ii, std::move(_data[ii + 1]));

    --_size;
  }

public:
  bool empty() const { return _size == 0; }
  std::size_t size() const { return _size; }
  std::size_t capacity() const { return _capacity; }

public:
  // const Type& operator[](std::size_t index) const { return _data[_getIndex(index)]; }
  // Type& operator[](std::size_t index) { return _data[_getIndex(index)]; }

  const Type& operator[](int index) const { return _data[_getIndex(index)]; }
  Type& operator[](int index) { return _data[_getIndex(index)]; }

  const Type& at(std::size_t index) const
  {
    if (index >= _size)
      D_THROW(std::runtime_error, "out of range, index: " << index);
    return _data[_getIndex(index)];
  }
  Type& at(std::size_t index)
  {
    if (index >= _size)
      D_THROW(std::runtime_error, "out of range, index: " << index);
    return _data[_getIndex(index)];
  }

  // const Type& at(std::size_t index) const
  // {
  //   if (!(index < _size))
  //   {
  //     std::cerr << "index " << index << std::endl;
  //     std::cerr << "_size " << _size << std::endl;
  //     throw std::out_of_range("out of range");
  //   }
  //   return _data[index];
  // }
  // Type& at(std::size_t index)
  // {
  //   if (!(index < _size))
  //   {
  //     std::cerr << "index " << index << std::endl;
  //     std::cerr << "_size " << _size << std::endl;
  //     throw std::out_of_range("out of range");
  //   }
  //   return _data[index];
  // }

  const Type& front() const { return _data[0]; }
  Type& front() { return _data[0]; }

  const Type& back() const { return _data[_size - 1]; }
  Type& back() { return _data[_size - 1]; }

public:
  bool operator==(const dynamic_heap_array& other) const { return this == &other; }
  bool operator!=(const dynamic_heap_array& other) const { return !(*this == other); }

private:
  void _realloc(std::size_t newCapacity)
  {
    if (newCapacity == 0) // true the first time
      newCapacity = 1;

    if (newCapacity < _capacity)
      return;

    Type* newData = allocateMemory(newCapacity);

    for (std::size_t ii = 0; ii < _size; ++ii)
      callMoveConstructor(newData, ii, std::move(_data[ii]));

    for (std::size_t ii = 0; ii < _size; ++ii)
      callDestructor(ii);

    deallocateMemory(_data, _capacity);

    _data = newData;
    _capacity = newCapacity;
  }
};
