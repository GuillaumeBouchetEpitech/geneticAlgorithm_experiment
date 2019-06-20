
#pragma once

#include <type_traits>

// utility function to auto convert enums class to their underlying type
template <typename E>
constexpr auto toUnderlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
