
#pragma once

#include "basic_double_linked_list.hpp"

#include "framework/system/ErrorHandler.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#if 0

template <typename Type>
class custom_allocator
{
public:
  // allocate memory only, will not call any constructor
  Type* allocate_memory(std::size_t size) {
    return (Type*) ::operator new (size * sizeof(Type));
  }

  // deallocate memory only, will not call any destructor
  void deallocate_memory(Type* data, std::size_t size) {
    ::operator delete(data, size * sizeof(Type));
  }

  // call the move constructor only, do not allocate memory
  void call_move_constructor(Type* data, std::size_t index, Type&& value) {
    new (&data[index]) Type(std::move(value));
  }

  // call the move constructor only, do not allocate memory
  template <typename... Args>
  Type& emplace_move_constructor(Type* data, std::size_t index,
                                 Args&&... args) {
    new (&data[index]) Type(std::forward<Args>(args)...);
    return data[index];
  }

  // call the destructor only, do not deallocate memory
  void call_destructor(std::size_t index) {
    _data[index].~Type();
  }
};

#endif

//
//
//
//
//

// will survive a container reallocation
// will survive a container destruction
template <typename generic_array_container>
class generic_array_container_base_iterator
  : public basic_double_linked_list::link {

  friend generic_array_container;

public:
  using ValueType = typename generic_array_container::ValueType;

protected:
  generic_array_container* _container;
  int _index;

public:
  generic_array_container_base_iterator(generic_array_container& container,
                                        int index)
    : _container(&container), _index(index) {
    basic_double_linked_list::add(_container->_iterators_list, *this);
  }

  // COPY
  generic_array_container_base_iterator(
    const generic_array_container_base_iterator& other) {
    if (&other == this)
      return;

    _container = other._container;
    _index = other._index;
    if (_container != nullptr) {
      basic_double_linked_list::add(_container->_iterators_list, *this);
    }
  }

  // MOVE
  generic_array_container_base_iterator(
    generic_array_container_base_iterator&& other) {
    if (&other == this)
      return;

    std::swap(_container, other._container);
    std::swap(_index, other._index);
    if (_container != nullptr) {
      basic_double_linked_list::replace(_container->_iterators_list, other,
                                        *this);
    }

    basic_double_linked_list::replace(_container->_iterators_list, other,
                                      *this);
  }

  // COPY
  generic_array_container_base_iterator&
  operator=(const generic_array_container_base_iterator& other) {
    if (&other == this)
      return *this;

    _container = other._container;
    _index = other._index;
    if (_container != nullptr) {
      basic_double_linked_list::add(_container->_iterators_list, *this);
    }

    return *this;
  }

  // MOVE
  generic_array_container_base_iterator&
  operator=(const generic_array_container_base_iterator&& other) {
    if (&other == this)
      return *this;

    std::swap(_container, other._container);
    std::swap(_index, other._index);
    if (_container != nullptr) {
      basic_double_linked_list::replace(_container->_iterators_list, other,
                                        *this);
    }

    return *this;
  }

  ~generic_array_container_base_iterator() {
    if (_container)
      basic_double_linked_list::remove(_container->_iterators_list, *this);
  }

public:
  bool is_valid() const { return _container != nullptr; }

protected:
  void _ensure_is_valid() const {
    if (!is_valid())
      D_THROW(std::runtime_error, "invalid iterator");
  }

public:
  generic_array_container_base_iterator& operator++() // ++pre
  {
    _ensure_is_valid();
    if (_index < int(_container->size()))
      ++_index;
    return *this;
  }
  generic_array_container_base_iterator& operator++(int) // post++
  {
    generic_array_container_base_iterator copy = *this;
    ++(*this);
    return copy;
  }

  generic_array_container_base_iterator& operator--() // --pre
  {
    _ensure_is_valid();
    if (_index > 0)
      --_index;
    return *this;
  }
  generic_array_container_base_iterator& operator--(int) // post--
  {
    generic_array_container_base_iterator copy = *this;
    --(*this);
    return copy;
  }

public:
  bool operator==(const generic_array_container_base_iterator& other) const {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const generic_array_container_base_iterator& other) const {
    return !(*this == other);
  }
};

// will survive a container reallocation
// will survive a container destruction
template <typename generic_array_container>
class generic_array_container_iterator
  : public generic_array_container_base_iterator<generic_array_container> {

  friend generic_array_container;

public:
  using BaseType =
    generic_array_container_base_iterator<generic_array_container>;
  using ValueType = typename BaseType::ValueType;

public:
  generic_array_container_iterator(generic_array_container& container,
                                   int index)
    : BaseType(container, index) {}

public:
  ValueType& operator[](int index) {
    BaseType::_ensure_is_valid();
    return (*BaseType::_container)[BaseType::_index + index];
  }
  ValueType* operator->() {
    BaseType::_ensure_is_valid();
    return &((*BaseType::_container)[BaseType::_index]);
  }
  ValueType& operator*() {
    BaseType::_ensure_is_valid();
    return (*BaseType::_container)[BaseType::_index];
  }
};

// will survive a container reallocation
// will survive a container destruction
template <typename generic_array_container>
class generic_array_container_const_iterator
  : public generic_array_container_base_iterator<generic_array_container> {

  friend generic_array_container;

public:
  using BaseType =
    generic_array_container_base_iterator<generic_array_container>;
  using ValueType = typename BaseType::ValueType;

public:
  generic_array_container_const_iterator(generic_array_container& container,
                                         int index)
    : BaseType(container, index) {}

public:
  const ValueType& operator[](int index) const {
    BaseType::_ensure_is_valid();
    return (*BaseType::_container)[BaseType::_index + index];
  }
  const ValueType* operator->() const {
    BaseType::_ensure_is_valid();
    return &((*BaseType::_container)[BaseType::_index]);
  }
  const ValueType& operator*() const {
    BaseType::_ensure_is_valid();
    return (*BaseType::_container)[BaseType::_index];
  }
};

//
//
//
//
//

template <typename Type> class generic_array_container {
public:
  using ValueType = Type;
  using BaseIterator =
    generic_array_container_base_iterator<generic_array_container<Type>>;
  using Iterator =
    generic_array_container_iterator<generic_array_container<Type>>;
  using ConstIterator =
    generic_array_container_const_iterator<generic_array_container<Type>>;

protected:
  friend BaseIterator;
  friend Iterator;
  friend ConstIterator;

protected:
  std::size_t _size = 0;
  Type* _data = nullptr;

  basic_double_linked_list _iterators_list;
  basic_double_linked_list _const_iterators_list;

public:
  generic_array_container() = default;

  // disable copy
  generic_array_container(const generic_array_container& other) = delete;
  generic_array_container&
  operator=(const generic_array_container& other) = delete;
  // disable copy

  generic_array_container(generic_array_container&& other) {
    std::swap(_size, other._size);
    std::swap(_data, other._data);
  }

  generic_array_container& operator=(generic_array_container&& other) {
    std::swap(_size, other._size);
    std::swap(_data, other._data);
    return *this;
  }

  virtual ~generic_array_container() { invalidate_all_iterators(); }

protected:
  void _ensure_not_empty() const {
    if (_size == 0)
      D_THROW(std::runtime_error, "empty array");
  }

  std::size_t _get_index(int index) const {
    _ensure_not_empty();
    if (index < 0)
      index = int(_size) - (-index) % int(_size);
    if (index >= int(_size))
      index = index % int(_size);
    return std::size_t(index);
  }

  // std::size_t _get_index(std::size_t index) const
  // {
  //   return std::size_t(index) % _size;
  // }

public:
  Iterator begin() { return Iterator(*this, 0); }
  Iterator end() { return Iterator(*this, int(_size)); }

  ConstIterator begin() const {
    return ConstIterator(*const_cast<generic_array_container*>(this), 0);
  }
  ConstIterator end() const {
    return ConstIterator(*const_cast<generic_array_container*>(this),
                         int(_size));
  }

  void invalidate_all_iterators() {
    basic_double_linked_list::loop_and_reset<Iterator>(
      _iterators_list, [](Iterator* it) -> void { it->_container = nullptr; });
    basic_double_linked_list::loop_and_reset<ConstIterator>(
      _const_iterators_list,
      [](ConstIterator* it) -> void { it->_container = nullptr; });
  }

public:
  bool is_empty() const { return _size == 0; }
  std::size_t size() const { return _size; }
  bool is_out_of_range(std::size_t index) const { return (index >= _size); }
  std::size_t total_iterators() const {
    return _iterators_list.size + _const_iterators_list.size;
  }

public:
  // support out of range index (negative values included)
  const Type& operator[](int index) const { return _data[_get_index(index)]; }
  Type& operator[](int index) { return _data[_get_index(index)]; }

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

  const Type& front() const {
    _ensure_not_empty();
    return _data[0];
  }
  Type& front() {
    _ensure_not_empty();
    return _data[0];
  }

  const Type& back() const {
    _ensure_not_empty();
    return _data[_size - 1];
  }
  Type& back() {
    _ensure_not_empty();
    return _data[_size - 1];
  }

public:
  bool operator==(const generic_array_container& other) const {
    return this == &other;
  }
  bool operator!=(const generic_array_container& other) const {
    return !(*this == other);
  }
};
