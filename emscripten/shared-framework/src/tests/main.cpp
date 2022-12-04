
#include "./tests.hpp"

#include "framework/TraceLogger.hpp"

#include "framework/helpers/GLMath.hpp"

#include <cstdlib> // <= EXIT_SUCCESS

int main()
{
  // D_MYLOG("test " << glm::vec3(1,2,3));
  // D_MYLOG("test " << true);

  test_basic_double_linked_list();
  test_dynamic_heap_array();
  test_static_heap_grid_array();
  test_weak_ref_data_pool();
  // test_math();
  test_frustum_culling();
  test_static_stack_array();
  test_deterministic_rng();

  return EXIT_SUCCESS;
}
