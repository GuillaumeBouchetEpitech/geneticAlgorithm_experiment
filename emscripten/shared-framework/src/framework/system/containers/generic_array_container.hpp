
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
class generic_array_container_iterator
  : private basic_double_linked_list::link {

  friend generic_array_container;

public:
  using ValueType = typename generic_array_container::ValueType;

private:
  generic_array_container* _container;
  int _index;

public:
  generic_array_container_iterator(generic_array_container& container,
                                   int index)
    : _container(&container), _index(index) {
    basic_double_linked_list::add(_container->_iterators_list, *this);
  }

  ~generic_array_container_iterator() {
    if (_container)
      basic_double_linked_list::remove(_container->_iterators_list, *this);
  }

public:
  bool is_valid() const { return _container != nullptr; }

private:
  void _ensure_is_valid() const {
    if (!is_valid())
      D_THROW(std::runtime_error, "invalid iterator");
  }

public:
  generic_array_container_iterator& operator++() // ++pre
  {
    _ensure_is_valid();
    if (_index < int(_container->size()))
      ++_index;
    return *this;
  }
  generic_array_container_iterator& operator++(int) // post++
  {
    generic_array_container_iterator copy = *this;
    ++(*this);
    return copy;
  }

  generic_array_container_iterator& operator--() // --pre
  {
    _ensure_is_valid();
    if (_index > 0)
      --_index;
    return *this;
  }
  generic_array_container_iterator& operator--(int) // post--
  {
    generic_array_container_iterator copy = *this;
    --(*this);
    return copy;
  }

public:
  ValueType& operator[](int index) {
    _ensure_is_valid();
    return (*_container)[_index + index];
  }
  ValueType* operator->() {
    _ensure_is_valid();
    return &((*_container)[_index]);
  }
  ValueType& operator*() {
    _ensure_is_valid();
    return (*_container)[_index];
  }

public:
  bool operator==(const generic_array_container_iterator& other) const {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const generic_array_container_iterator& other) const {
    return !(*this == other);
  }
};

//
//
//
//
//

// will survive a container reallocation
// will survive a container destruction
template <typename generic_array_container>
class generic_array_container_const_iterator
  : private basic_double_linked_list::link {

  friend generic_array_container;

public:
  using ValueType = typename generic_array_container::ValueType;

private:
  const generic_array_container* _container;
  int _index;

public:
  generic_array_container_const_iterator(
    const generic_array_container& container, int index)
    : _container(&container), _index(index) {
    basic_double_linked_list::add(
      const_cast<generic_array_container*>(_container)->_const_iterators_list,
      *this);
  }

  ~generic_array_container_const_iterator() {
    if (_container)
      basic_double_linked_list::remove(
        const_cast<generic_array_container*>(_container)->_const_iterators_list,
        *this);
  }

public:
  bool is_valid() const { return _container != nullptr; }

private:
  void _ensure_is_valid() const {
    if (!is_valid())
      D_THROW(std::runtime_error, "invalid iterator");
  }

public:
  generic_array_container_const_iterator& operator++() // ++pre
  {
    _ensure_is_valid();
    if (_index < int(_container->size()))
      ++_index;
    return *this;
  }
  generic_array_container_const_iterator& operator++(int) // post++
  {
    generic_array_container_const_iterator copy = *this;
    ++(*this);
    return copy;
  }

  generic_array_container_const_iterator& operator--() // --pre
  {
    _ensure_is_valid();
    if (_index > 0)
      --_index;
    return *this;
  }
  generic_array_container_const_iterator& operator--(int) // post--
  {
    generic_array_container_const_iterator copy = *this;
    --(*this);
    return copy;
  }

public:
  const ValueType& operator[](int index) const {
    _ensure_is_valid();
    return (*_container)[_index + index];
  }
  const ValueType* operator->() const {
    _ensure_is_valid();
    return &((*_container)[_index]);
  }
  const ValueType& operator*() const {
    _ensure_is_valid();
    return (*_container)[_index];
  }

public:
  bool operator==(const generic_array_container_const_iterator& other) const {
    return (_container == other._container && _index == other._index);
  }
  bool operator!=(const generic_array_container_const_iterator& other) const {
    return !(*this == other);
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
  using Iterator =
    generic_array_container_iterator<generic_array_container<Type>>;
  using ConstIterator =
    generic_array_container_const_iterator<generic_array_container<Type>>;

protected:
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

  ConstIterator begin() const { return ConstIterator(*this, 0); }
  ConstIterator end() const { return ConstIterator(*this, int(_size)); }

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

template <typename Type, std::size_t _Size>
class test_static_array : public generic_array_container<Type> {

  using base_class = generic_array_container<Type>;

private:
  Type _static_data[_Size];

public:
  test_static_array() {
    this->_size = _Size;
    this->_data = this->_static_data;
  }

  test_static_array(test_static_array&& other) : base_class(other) {
    std::swap(_static_data, other._static_data); // copy (-_-)
  }

  test_static_array& operator=(test_static_array&& other) {
    base_class::operator=(std::move(other));
    std::swap(_static_data, other._static_data); // copy (-_-)
    return *this;
  }

  virtual ~test_static_array() = default;
};

template <typename Type, typename Allocator = std::allocator<Type>,
          std::size_t initial_size = 0>
class test_dynamic_array : public generic_array_container<Type> {

  using base_class = generic_array_container<Type>;

  using traits_t = std::allocator_traits<Allocator>; // The matching trait

protected:
  std::size_t _capacity = 0;

protected:
  // allocate memory only, will not call any constructor
  Type* allocate_memory(std::size_t size) {
    Allocator alloc;
    Type* newData = alloc.allocate(size);

    return newData;
  }

  // deallocate memory only, will not call any destructor
  void deallocate_memory(Type* data, std::size_t size) {
    Allocator alloc;
    alloc.deallocate(data, size);
  }

  // call the move constructor only, do not allocate memory
  void call_move_constructor(Type* data, std::size_t index, Type&& value) {
    Allocator alloc;
    traits_t::construct(alloc, data + index, std::move(value));
  }

  // call the move constructor only, do not allocate memory
  template <typename... Args>
  Type& emplace_move_constructor(Type* data, std::size_t index,
                                 Args&&... args) {
    Allocator alloc;
    traits_t::construct(alloc, data + index, std::forward<Args>(args)...);
    return data[index];
  }

  // call the destructor only, do not deallocate memory
  void call_destructor(std::size_t index) {
    Allocator alloc;
    traits_t::destroy(alloc, this->_data + index);
  }

public:
  test_dynamic_array() {
    if (initial_size > 0)
      pre_allocate(initial_size);
  }

  // disable copy
  test_dynamic_array(const test_dynamic_array& other) = delete;
  test_dynamic_array& operator=(const test_dynamic_array& other) = delete;
  // disable copy

  test_dynamic_array(test_dynamic_array&& other)
    : base_class(std::move(other)) {
    std::swap(_capacity, other._capacity);
  }

  test_dynamic_array& operator=(test_dynamic_array&& other) {
    base_class::operator=(std::move(other));
    std::swap(_capacity, other._capacity);
    return *this;
  }

  virtual ~test_dynamic_array() {

    // invalidate_all_iterators();

    clear();
    deallocate_memory(this->_data, _capacity);
  }

public:
  // may reallocate
  void push_back(const Type& value) {
    if (this->_size == _capacity)
      _realloc(_capacity * 2);

    this->_data[this->_size++] = value; // copy
  }

  // may reallocate
  void push_back(Type&& value) {
    if (this->_size == _capacity)
      _realloc(_capacity * 2);

    call_move_constructor(this->_data, this->_size, std::move(value));

    ++this->_size;
  }

  // may reallocate
  template <typename... Args> Type& emplace_back(Args&&... args) {
    if (this->_size == _capacity)
      _realloc(_capacity * 2);

    Type& result = emplace_move_constructor(this->_data, this->_size,
                                            std::forward<Args>(args)...);

    ++this->_size;

    return result;
  }

public:
  void pop_back() {
    if (this->_size == 0)
      return;

    --this->_size;

    call_destructor(this->_size);
  }

  // no reallocation
  void unsorted_erase(std::size_t index) {
    if (this->_size > 1 && index + 1 < this->_size) {
      // move target to the back
      call_move_constructor(this->_data, index,
                            std::move(this->_data[this->_size - 1]));
    }
    // remove the back
    pop_back();
  }

  // no reallocation
  void sorted_erase(std::size_t inIndex) {
    if (this->is_out_of_range(inIndex))
      return;

    // call target destructor
    call_destructor(inIndex);

    // move data after the target
    for (std::size_t ii = inIndex; ii + 1 < this->_size; ++ii)
      call_move_constructor(this->_data, ii, std::move(this->_data[ii + 1]));

    --this->_size;
  }

public:
  void clear() {
    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_destructor(ii);

    this->_size = 0;
  }

  void ensure_size(std::size_t target_size) {
    if (target_size < this->_size)
      return;

    _realloc(target_size);
    this->_size = target_size;
  }

  void pre_allocate(std::size_t capacity) { _realloc(capacity); }

public:
  std::size_t capacity() const { return _capacity; }

protected:
  void _realloc(std::size_t newCapacity) {
    if (newCapacity == 0) // true the first time
      newCapacity = 1;

    if (newCapacity < _capacity)
      return;

    Type* newData = allocate_memory(newCapacity);

    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_move_constructor(newData, ii, std::move(this->_data[ii]));

    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_destructor(ii);

    if (_capacity > 0)
      deallocate_memory(this->_data, _capacity);

    this->_data = newData;
    _capacity = newCapacity;
  }
};
