
#include "headers.hpp"

TEST(basic_double_linked_list__start_empty, list_is_empty_initially) {
  List list;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
}

TEST(basic_double_linked_list__start_empty, link_is_empty_initially) {
  Link link1;

  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}
