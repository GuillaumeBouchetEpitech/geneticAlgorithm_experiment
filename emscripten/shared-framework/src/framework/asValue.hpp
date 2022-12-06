
#pragma once

#include <type_traits>

// utility function to auto convert enums class to their underlying type
template <typename EnumType>
constexpr auto asValue(EnumType currEnum) noexcept {
  return static_cast<std::underlying_type_t<EnumType>>(currEnum);
}
