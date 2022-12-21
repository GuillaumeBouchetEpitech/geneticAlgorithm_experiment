
#pragma once

#include "generic_array_container.hpp"

template <typename Type, typename Allocator = std::allocator<Type>,
          std::size_t initial_size = 0>
using dynamic_heap_array = test_dynamic_array<Type, Allocator, initial_size>;
