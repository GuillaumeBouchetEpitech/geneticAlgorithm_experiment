
#include "headers.hpp"

TEST(basic_double_linked_list__remove_link, can_remove_only_link) {
  List list;
  Link link1;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);

  List::add_link_to_list(list, link1);

  EXPECT_EQ(list.head_link, &link1);
  EXPECT_EQ(list.size, 1);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);

  List::remove_link_from_list(list, link1);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list__remove_link, can_remove_both_link_link1_first) {
  List list;
  Link link1;
  Link link2;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);

  List::remove_link_from_list(list, link1);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 1);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::remove_link_from_list(list, link2);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
}

TEST(basic_double_linked_list__remove_link, can_remove_both_link_link2_first) {
  List list;
  Link link1;
  Link link2;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);

  List::remove_link_from_list(list, link2);

  EXPECT_EQ(list.head_link, &link1);
  EXPECT_EQ(list.size, 1);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::remove_link_from_list(list, link1);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
}
