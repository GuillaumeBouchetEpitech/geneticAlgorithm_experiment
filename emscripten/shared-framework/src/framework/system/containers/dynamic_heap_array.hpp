
#pragma once

#include "generic_array_container.hpp"

template <typename Type, typename Allocator = std::allocator<Type>,
          std::size_t initial_size = 0>
class dynamic_heap_array : public generic_array_container<Type> {

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
  void call_constructor(Type* dataPtr) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr);
  }

  // call the move constructor only, do not allocate memory
  void call_move_constructor(Type* dataPtr, Type&& value) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr, std::move(value));
  }

  // call the move constructor only, do not allocate memory
  template <typename... Args>
  Type& emplace_move_constructor(Type* dataPtr, Args&&... args) {
    Allocator alloc;
    traits_t::construct(alloc, dataPtr, std::forward<Args>(args)...);
    return *dataPtr;
  }

  // call the destructor only, do not deallocate memory
  void call_destructor(Type* dataPtr) {
    Allocator alloc;
    traits_t::destroy(alloc, dataPtr);
  }

public:
  dynamic_heap_array() {
    if (initial_size > 0)
      pre_allocate(initial_size);
  }

  // disable copy
  dynamic_heap_array(const dynamic_heap_array& other) = delete;
  dynamic_heap_array& operator=(const dynamic_heap_array& other) = delete;
  // disable copy

  dynamic_heap_array(dynamic_heap_array&& other)
    : base_class(std::move(other)) {
    std::swap(_capacity, other._capacity);
  }

  dynamic_heap_array& operator=(dynamic_heap_array&& other) {
    base_class::operator=(std::move(other));
    std::swap(_capacity, other._capacity);
    return *this;
  }

  virtual ~dynamic_heap_array() {

    // base_class::invalidate_all_iterators();

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

    call_move_constructor(this->_data + this->_size, std::move(value));

    ++this->_size;
  }

  // may reallocate
  template <typename... Args> Type& emplace_back(Args&&... args) {
    if (this->_size == _capacity)
      _realloc(_capacity * 2);

    Type& result = emplace_move_constructor(this->_data + this->_size,
                                            std::forward<Args>(args)...);

    ++this->_size;

    return result;
  }

public:
  void pop_back() {
    if (this->_size == 0)
      return;

    --this->_size;

    call_destructor(this->_data + this->_size);
  }

  // no reallocation
  void unsorted_erase(std::size_t inIndex) {
    if (this->is_out_of_range(inIndex))
      return;

    // swap data at the end
    if (this->_size > 1 && inIndex < this->_size - 1) {
      this->_data[inIndex].applySwap(this->_data[this->_size - 1]);
    }

    // remove the back
    pop_back();
  }

  // no reallocation
  void sorted_erase(std::size_t inIndex) {
    if (this->is_out_of_range(inIndex))
      return;

    // swap data at the end
    for (std::size_t ii = inIndex; ii + 1 < this->_size; ++ii)
      this->_data[ii].applySwap(this->_data[ii + 1]);

    // remove the back
    pop_back();
  }

public:
  void clear() {
    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_destructor(this->_data + ii);

    this->_size = 0;
  }

  void ensure_size(std::size_t target_size) {
    if (target_size < this->_size)
      return;

    _realloc(target_size);

    for (std::size_t ii = this->_size; ii < target_size; ++ii)
      call_constructor(this->_data + ii);

    this->_size = target_size;
  }

  void pre_allocate(std::size_t capacity) { _realloc(capacity); }

public:
  std::size_t capacity() const { return _capacity; }

protected:
  void _realloc(std::size_t newCapacity) {
    if (newCapacity == 0) // true the first time
      newCapacity = 1;

    if (newCapacity <= _capacity)
      return;

    Type* newData = allocate_memory(newCapacity);

    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_move_constructor(newData + ii, std::move(this->_data[ii]));

    for (std::size_t ii = 0; ii < this->_size; ++ii)
      call_destructor(this->_data + ii);

    if (_capacity > 0)
      deallocate_memory(this->_data, _capacity);

    this->_data = newData;
    _capacity = newCapacity;
  }
};
