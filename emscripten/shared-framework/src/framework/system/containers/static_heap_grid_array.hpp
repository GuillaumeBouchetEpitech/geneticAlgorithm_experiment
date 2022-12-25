
#pragma once

#include "dynamic_heap_array.hpp"

#include <vector>
// #include <functional>
#include <iostream>
#include <utility>

// template <typename static_heap_grid_array>
// class static_heap_grid_array_iterator {
// public:
//   using ValueType = typename static_heap_grid_array::ValueType;

// protected:
//   ValueType* _ptr;

// public:
//   static_heap_grid_array_iterator(ValueType* ptr) : _ptr(ptr) {}

//   static_heap_grid_array_iterator& operator++() // ++pre
//   {
//     ++_ptr;
//     return *this;
//   }
//   static_heap_grid_array_iterator& operator++(int) // post++
//   {
//     static_heap_grid_array_iterator copy = *this;
//     ++(*this);
//     return copy;
//   }

//   static_heap_grid_array_iterator& operator--() // --pre
//   {
//     --_ptr;
//     return *this;
//   }
//   static_heap_grid_array_iterator& operator--(int) // post--
//   {
//     static_heap_grid_array_iterator copy = *this;
//     --(*this);
//     return copy;
//   }

//   static_heap_grid_array_iterator operator+(std::size_t value) {
//     // _ptr += value;
//     // return *this;
//     static_heap_grid_array_iterator copy = *this;
//     copy._ptr += value;
//     return copy;
//   }
//   static_heap_grid_array_iterator operator-(std::size_t value) {
//     static_heap_grid_array_iterator copy = *this;
//     copy._ptr -= value;
//     return copy;
//   }

//   ValueType& operator[](int index) { return *(_ptr + index); }
//   ValueType* operator->() { return _ptr; }
//   ValueType& operator*() { return *_ptr; }

//   bool operator==(const static_heap_grid_array_iterator& other) const {
//     return _ptr == other._ptr;
//   }
//   bool operator!=(const static_heap_grid_array_iterator& other) const {
//     return !(*this == other);
//   }
// };

// //
// //
// //
// //
// //

// template <typename static_heap_grid_array>
// class static_heap_grid_array_row_iterator {
// public:
//   using ValueType = typename static_heap_grid_array::ValueType;
//   using Iterator = typename static_heap_grid_array::Iterator;

// protected:
//   ValueType* _ptr;
//   std::size_t _width;

// public:
//   static_heap_grid_array_row_iterator(ValueType* ptr, std::size_t width)
//     : _ptr(ptr), _width(width) {}

//   static_heap_grid_array_row_iterator& operator++() // ++pre
//   {
//     _ptr += _width;
//     return *this;
//   }
//   static_heap_grid_array_row_iterator& operator++(int) // post++
//   {
//     static_heap_grid_array_row_iterator copy = *this;
//     _ptr += _width;
//     return copy;
//   }

//   static_heap_grid_array_row_iterator& operator--() // --pre
//   {
//     _ptr -= _width;
//     return *this;
//   }
//   static_heap_grid_array_row_iterator& operator--(int) // post--
//   {
//     static_heap_grid_array_row_iterator copy = *this;
//     _ptr -= _width;
//     return copy;
//   }

//   static_heap_grid_array_row_iterator operator+(std::size_t value) {
//     static_heap_grid_array_row_iterator copy = *this;
//     copy._ptr += _width * value;
//     return copy;
//   }
//   static_heap_grid_array_row_iterator operator-(std::size_t value) {
//     static_heap_grid_array_row_iterator copy = *this;
//     copy._ptr -= _width * value;
//     return copy;
//   }

//   ValueType& operator[](int index) { return *(_ptr + index); }
//   ValueType* operator->() { return _ptr; }
//   ValueType& operator*() { return *_ptr; }

//   bool operator==(const static_heap_grid_array_row_iterator& other) const {
//     return _ptr == other._ptr;
//   }
//   bool operator!=(const static_heap_grid_array_row_iterator& other) const {
//     return !(*this == other);
//   }

//   Iterator beginColumns() { return Iterator(_ptr); }
//   Iterator endColumns() { return Iterator(_ptr + _width); }
// };

// //
// //
// //
// //
// //

/**
 * static_heap_grid_array
 *
 * ...TODO
 */
template <typename Type> class static_heap_grid_array {
public:
  using ValueType = Type;
  // using Iterator =
  //   static_heap_grid_array_iterator<static_heap_grid_array<Type>>;
  // using RowIterator =
  //   static_heap_grid_array_row_iterator<static_heap_grid_array<Type>>;

private:
  dynamic_heap_array<Type> _data;
  std::size_t _height = 0;
  std::size_t _width = 0;

public:
  static_heap_grid_array() = default;
  ~static_heap_grid_array() { clear(); }

  // disable move/copy
  static_heap_grid_array(const static_heap_grid_array& other) = delete;
  static_heap_grid_array(static_heap_grid_array&& other) = delete;
  static_heap_grid_array&
  operator=(const static_heap_grid_array& other) = delete;
  static_heap_grid_array& operator=(static_heap_grid_array&& other) = delete;
  // disable move/copy

  void allocate(std::size_t height, std::size_t width) {
    _height = height;
    _width = width;
    _data.ensure_size(_height * _width);
  }

  void clear() { _data.clear(); }

private:
  int _get_index(int index, std::size_t size) const {
    if (index < 0)
      index = int(size) - (-index) % int(size);
    if (index >= int(size))
      index = index % int(size);
    return index;
  }

  int _get_data_index(int row, int column) const {
    return _get_index(row, _height) * int(_width) + _get_index(column, _width);
  }

public:

  Type& get(int row, int column) {
    return _data[_get_data_index(row, column)];
  }
  Type& operator()(int row, int column) {
    return get(row, column);
  }

  const Type& get(int row, int column) const {
    return _data[_get_data_index(row, column)];
  }
  const Type& operator()(int row, int column) const {
    return get(row, column);
  }

  const Type& operator[](int index) const { return _data[index]; }
  Type& operator[](int index) { return _data[index]; }

  std::size_t witdh() const { return _width; }
  std::size_t height() const { return _height; }
  std::size_t size() const { return _data.size(); }
  bool is_empty() const { return _data.is_empty(); }

  // Iterator begin() { return Iterator(_data.data()); }
  // Iterator end() { return Iterator(_data.data() + _data.size()); }

  // // Iterator beginColumns(uint32_t row) { return Iterator(_data + row *
  // // _width); } Iterator endColumns(uint32_t row) { return Iterator(_data + (row
  // // + 1) * _width); }

  // // RowIterator beginRow(uint32_t row) { return RowIterator(_data + row *
  // // _width, _width); } RowIterator endRow(uint32_t row) { return
  // // RowIterator(_data + (row + 1) * _width, _width); }

  // RowIterator beginRow() { return RowIterator(_data.data(), _width); }
  // RowIterator endRow() {
  //   return RowIterator(_data.data() + _data.size(), _width);
  // }

  // Iterator beginColumns(std::size_t row) {
  //   return Iterator(_data.data() + row * _width);
  // }
  // Iterator endColumns(std::size_t row) {
  //   return Iterator(_data.data() + (row + 1) * _width);
  // }
};
