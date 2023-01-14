
#include "headers.hpp"

TEST(basic_double_linked_list__reset_link, link_can_be_reset) {
  Link link1;
  link1.prev_link = (Link*)666;
  link1.next_link = (Link*)666;

  List::reset_link(link1);

  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}
