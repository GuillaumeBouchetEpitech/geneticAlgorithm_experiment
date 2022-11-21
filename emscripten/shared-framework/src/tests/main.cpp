
#include "./tests.hpp"

#include <cstdlib> // <= EXIT_SUCCESS

int main()
{
  test_dynamic_heap_array();
  test_weak_ref_data_pool();

  return EXIT_SUCCESS;
}
