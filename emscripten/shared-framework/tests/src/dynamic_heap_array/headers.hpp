
#pragma once

#include "framework/system/containers/dynamic_heap_array.hpp"
#include "framework/system/containers/generic_array_container.hpp"
#include "framework/system/containers/static_array.hpp"

#include "framework/system/TraceLogger.hpp"

#include "generic_array_container_commons/common.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

template <std::size_t N>
using shorthand_dynamic_heap_array =
  dynamic_heap_array<common::TestStructure,
                     std::allocator<common::TestStructure>, N>;
