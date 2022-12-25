#pragma once

#include "generic_array_container.hpp"

template <typename Type, std::size_t _Size>
class static_array : public generic_array_container<Type> {

  using base_class = generic_array_container<Type>;

private:
  Type _static_data[_Size];

public:
  static_array() {
    this->_size = _Size;
    this->_data = this->_static_data;
  }

  static_array(static_array&& other) : base_class(other) {
    std::swap(_static_data, other._static_data); // copy (-_-)
  }

  static_array& operator=(static_array&& other) {
    base_class::operator=(std::move(other));
    std::swap(_static_data, other._static_data); // copy (-_-)
    return *this;
  }

  virtual ~static_array() = default;
};
