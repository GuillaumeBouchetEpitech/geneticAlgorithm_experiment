
#pragma once

#include "basic_double_linked_list.hpp"
#include "dynamic_heap_array.hpp"

#include "framework/system/ErrorHandler.hpp"

#include <vector>
// #include <functional>
#include <iostream>
#include <utility>

template <typename static_heap_grid_array>
class static_heap_grid_array_base_column_iterator
  : public basic_double_linked_list::link {

  friend static_heap_grid_array;

public:
  using ValueType = typename static_heap_grid_array::ValueType;

protected:
  static_heap_grid_array* _container;
  int _row;
  int _column;

public:
  static_heap_grid_array_base_column_iterator(static_heap_grid_array& container,
                                              int row, int column)
    : _container(&container), _row(row), _column(column) {
    basic_double_linked_list::add_link_to_list(
      _container->_column_iterators_list, *this);
  }

  ~static_heap_grid_array_base_column_iterator() {
    if (_container)
      basic_double_linked_list::remove_link_from_list(
        _container->_column_iterators_list, *this);
  }

public:
  bool is_valid() const { return _container != nullptr; }

protected:
  void _ensure_is_valid() const {
    if (!is_valid())
      D_THROW(std::runtime_error, "invalid iterator");
  }

public:
  static_heap_grid_array_base_column_iterator& operator++() // ++pre
  {
    _ensure_is_valid();
    // if (_column < int(_container->size()))
    ++_column;
    return *this;
  }
  static_heap_grid_array_base_column_iterator& operator++(int) // post++
  {
    static_heap_grid_array_base_column_iterator copy = *this;
    ++(*this);
    return copy;
  }

  static_heap_grid_array_base_column_iterator& operator--() // --pre
  {
    _ensure_is_valid();
    // if (_column > 0)
    --_column;
    return *this;
  }
  static_heap_grid_array_base_column_iterator& operator--(int) // post--
  {
    static_heap_grid_array_base_column_iterator copy = *this;
    --(*this);
    return copy;
  }

public:
  bool
  operator==(const static_heap_grid_array_base_column_iterator& other) const {
    return (_container == other._container && _row == other._row &&
            _column == other._column);
  }
  bool
  operator!=(const static_heap_grid_array_base_column_iterator& other) const {
    return !(*this == other);
  }

public:
  static_heap_grid_array_base_column_iterator operator+(int value) {
    static_heap_grid_array_base_column_iterator copy = *this;
    copy._column += value;
    return copy;
  }
  static_heap_grid_array_base_column_iterator operator-(int value) {
    static_heap_grid_array_base_column_iterator copy = *this;
    copy._column -= value;
    return copy;
  }
};

template <typename static_heap_grid_array>
class static_heap_grid_array_column_iterator
  : public static_heap_grid_array_base_column_iterator<static_heap_grid_array> {

  friend static_heap_grid_array;

public:
  using BaseType =
    static_heap_grid_array_base_column_iterator<static_heap_grid_array>;
  using ValueType = typename BaseType::ValueType;

public:
  static_heap_grid_array_column_iterator(static_heap_grid_array& container,
                                         int row, int column)
    : BaseType(container, row, column) {}

public:
  ValueType& operator[](int index) {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column + index);
  }
  const ValueType& operator[](int index) const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column + index);
  }
  ValueType* operator->() {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  const ValueType* operator->() const {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  ValueType& operator*() {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }
  const ValueType& operator*() const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }
};

template <typename static_heap_grid_array>
class static_heap_grid_array_const_column_iterator
  : public static_heap_grid_array_base_column_iterator<static_heap_grid_array> {

  friend static_heap_grid_array;

public:
  using BaseType =
    static_heap_grid_array_base_column_iterator<static_heap_grid_array>;
  using ValueType = typename BaseType::ValueType;

public:
  static_heap_grid_array_const_column_iterator(
    static_heap_grid_array& container, int row, int column)
    : BaseType(container, row, column) {}

public:
  const ValueType& operator[](int index) const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column + index);
  }
  const ValueType* operator->() const {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  const ValueType& operator*() const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }
};

//
//
//
//
//

template <typename static_heap_grid_array>
class static_heap_grid_array_base_row_iterator
  : public basic_double_linked_list::link {

  friend static_heap_grid_array;

public:
  using ValueType = typename static_heap_grid_array::ValueType;

protected:
  static_heap_grid_array* _container;
  int _row;
  int _column;

public:
  static_heap_grid_array_base_row_iterator(static_heap_grid_array& container,
                                           int row, int column)
    : _container(&container), _row(row), _column(column) {
    basic_double_linked_list::add_link_to_list(_container->_row_iterators_list,
                                               *this);
  }

  ~static_heap_grid_array_base_row_iterator() {
    if (_container)
      basic_double_linked_list::remove_link_from_list(
        _container->_row_iterators_list, *this);
  }

public:
  bool is_valid() const { return _container != nullptr; }

protected:
  void _ensure_is_valid() const {
    if (!is_valid())
      D_THROW(std::runtime_error, "invalid iterator");
  }

public:
  static_heap_grid_array_base_row_iterator& operator++() // ++pre
  {
    _ensure_is_valid();
    // if (_column < int(_container->size()))
    ++_row;
    return *this;
  }
  static_heap_grid_array_base_row_iterator& operator++(int) // post++
  {
    static_heap_grid_array_base_row_iterator copy = *this;
    ++(*this);
    return copy;
  }

  static_heap_grid_array_base_row_iterator& operator--() // --pre
  {
    _ensure_is_valid();
    // if (_column > 0)
    --_row;
    return *this;
  }
  static_heap_grid_array_base_row_iterator& operator--(int) // post--
  {
    static_heap_grid_array_base_row_iterator copy = *this;
    --(*this);
    return copy;
  }

public:
  bool operator==(const static_heap_grid_array_base_row_iterator& other) const {
    return (_container == other._container && _row == other._row &&
            _column == other._column);
  }
  bool operator!=(const static_heap_grid_array_base_row_iterator& other) const {
    return !(*this == other);
  }

public:
  static_heap_grid_array_base_row_iterator operator+(int value) {
    static_heap_grid_array_base_row_iterator copy = *this;
    copy._row += value;
    return copy;
  }
  static_heap_grid_array_base_row_iterator operator-(int value) {
    static_heap_grid_array_base_row_iterator copy = *this;
    copy._row -= value;
    return copy;
  }
};

template <typename static_heap_grid_array>
class static_heap_grid_array_row_iterator
  : public static_heap_grid_array_base_row_iterator<static_heap_grid_array> {

  friend static_heap_grid_array;

public:
  using BaseType =
    static_heap_grid_array_base_row_iterator<static_heap_grid_array>;
  using ColumnIterator = typename static_heap_grid_array::ColumnIterator;
  using ConstColumnIterator =
    typename static_heap_grid_array::ConstColumnIterator;
  using ValueType = typename BaseType::ValueType;

public:
  static_heap_grid_array_row_iterator(static_heap_grid_array& container,
                                      int row, int column)
    : BaseType(container, row, column) {}

public:
  ValueType& operator[](int index) {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row + index, this->_column);
  }
  const ValueType& operator[](int index) const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row + index, this->_column);
  }
  ValueType* operator->() {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  const ValueType* operator->() const {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  ValueType& operator*() {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }
  const ValueType& operator*() const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }

public:
  ColumnIterator beginColumns() {
    return ColumnIterator(*this->_container, this->_row, this->_column);
  }
  ColumnIterator endColumns() {
    return ColumnIterator(*this->_container, this->_row,
                          this->_column + int(this->_container->_width));
  }
  ConstColumnIterator beginColumns() const {
    return ConstColumnIterator(*this->_container, this->_row, this->_column);
  }
  ConstColumnIterator endColumns() const {
    return ConstColumnIterator(*this->_container, this->_row,
                               this->_column + int(this->_container->_width));
  }
};

template <typename static_heap_grid_array>
class static_heap_grid_array_const_row_iterator
  : public static_heap_grid_array_base_row_iterator<static_heap_grid_array> {

  friend static_heap_grid_array;

public:
  using BaseType =
    static_heap_grid_array_base_row_iterator<static_heap_grid_array>;
  using ConstColumnIterator =
    typename static_heap_grid_array::ConstColumnIterator;
  using ValueType = typename BaseType::ValueType;

public:
  static_heap_grid_array_const_row_iterator(static_heap_grid_array& container,
                                            int row, int column)
    : BaseType(container, row, column) {}

public:
  const ValueType& operator[](int index) const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row + index, this->_column);
  }
  const ValueType* operator->() const {
    BaseType::_ensure_is_valid();
    return &((*this->_container)(this->_row, this->_column));
  }
  const ValueType& operator*() const {
    BaseType::_ensure_is_valid();
    return (*this->_container)(this->_row, this->_column);
  }

public:
  ConstColumnIterator beginColumns() const {
    return ConstColumnIterator(*this->_container, this->_row, this->_column);
  }
  ConstColumnIterator endColumns() const {
    return ConstColumnIterator(*this->_container, this->_row,
                               this->_column + int(this->_container->_width));
  }
};

//
//
//
//
//

/**
 * static_heap_grid_array
 *
 * ...TODO
 */
template <typename Type> class static_heap_grid_array {
public:
  using ValueType = Type;
  using BaseColumnIterator =
    static_heap_grid_array_base_column_iterator<static_heap_grid_array<Type>>;
  using ColumnIterator =
    static_heap_grid_array_column_iterator<static_heap_grid_array<Type>>;
  using ConstColumnIterator =
    static_heap_grid_array_const_column_iterator<static_heap_grid_array<Type>>;
  using BaseRowIterator =
    static_heap_grid_array_base_row_iterator<static_heap_grid_array<Type>>;
  using RowIterator =
    static_heap_grid_array_row_iterator<static_heap_grid_array<Type>>;
  using ConstRowIterator =
    static_heap_grid_array_const_row_iterator<static_heap_grid_array<Type>>;

protected:
  friend BaseColumnIterator;
  friend ColumnIterator;
  friend ConstColumnIterator;
  friend BaseRowIterator;
  friend RowIterator;
  friend ConstRowIterator;

private:
  dynamic_heap_array<Type> _data;
  std::size_t _height = 0;
  std::size_t _width = 0;

  basic_double_linked_list _row_iterators_list;
  basic_double_linked_list _column_iterators_list;

public:
  static_heap_grid_array() = default;
  virtual ~static_heap_grid_array() { clear(); }

  // disable move/copy
  static_heap_grid_array(const static_heap_grid_array& other) = delete;
  static_heap_grid_array(static_heap_grid_array&& other) = delete;
  static_heap_grid_array&
  operator=(const static_heap_grid_array& other) = delete;
  static_heap_grid_array& operator=(static_heap_grid_array&& other) = delete;
  // disable move/copy

public:
  void allocate(std::size_t height, std::size_t width) {
    _height = height;
    _width = width;
    _data.ensure_size(_height * _width);
  }

  void clear() {
    invalidate_all_iterators();
    _data.clear();
  }

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
  Type& get(int row, int column) { return _data[_get_data_index(row, column)]; }
  Type& operator()(int row, int column) { return get(row, column); }

  const Type& get(int row, int column) const {
    return _data[_get_data_index(row, column)];
  }
  const Type& operator()(int row, int column) const { return get(row, column); }

  const Type& operator[](int index) const { return _data[index]; }
  Type& operator[](int index) { return _data[index]; }

  std::size_t width() const { return _width; }
  std::size_t height() const { return _height; }
  std::size_t size() const { return _data.size(); }
  bool is_empty() const { return _data.is_empty(); }

public:
  ColumnIterator beginColumns(uint32_t row) {
    return ColumnIterator(*this, int(row), 0);
  }
  ColumnIterator endColumns(uint32_t row) {
    return ColumnIterator(*this, int(row), int(_width));
  }
  ConstColumnIterator beginColumns(uint32_t row) const {
    return ConstColumnIterator(*const_cast<static_heap_grid_array*>(this),
                               int(row), 0);
  }
  ConstColumnIterator endColumns(uint32_t row) const {
    return ConstColumnIterator(*const_cast<static_heap_grid_array*>(this),
                               int(row), int(_width));
  }

  RowIterator beginRows(uint32_t row) {
    return RowIterator(*this, int(row), 0);
  }
  RowIterator endRows(uint32_t row) {
    return RowIterator(*this, int(row) + 1, 0);
  }
  ConstRowIterator beginRows(uint32_t row) const {
    return ConstRowIterator(*const_cast<static_heap_grid_array*>(this),
                            int(row), 0);
  }
  ConstRowIterator endRows(uint32_t row) const {
    return ConstRowIterator(*const_cast<static_heap_grid_array*>(this),
                            int(row) + 1, 0);
  }

  RowIterator beginRows() { return RowIterator(*this, 0, 0); }
  RowIterator endRows() { return RowIterator(*this, int(_height), 0); }
  ConstRowIterator beginRows() const {
    return ConstRowIterator(*const_cast<static_heap_grid_array*>(this), 0, 0);
  }
  ConstRowIterator endRows() const {
    return ConstRowIterator(*const_cast<static_heap_grid_array*>(this),
                            int(_height), 0);
  }

  void invalidate_all_iterators() {
    basic_double_linked_list::loop_list_links_and_reset<ColumnIterator>(
      _column_iterators_list,
      [](ColumnIterator* it) -> void { it->_container = nullptr; });
    basic_double_linked_list::loop_list_links_and_reset<RowIterator>(
      _row_iterators_list,
      [](RowIterator* it) -> void { it->_container = nullptr; });
  }
};
