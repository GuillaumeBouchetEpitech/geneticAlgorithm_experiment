
#include "headers.hpp"

TEST(basic_double_linked_list__add_link_to_list, can_add_link_to_list_once) {
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
}

TEST(basic_double_linked_list__add_link_to_list, can_add_link_to_list_twice) {
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
}

TEST(basic_double_linked_list__add_link_to_list,
     can_add_link_to_list_three_times) {
  List list;
  Link link1;
  Link link2;
  Link link3;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);
  List::add_link_to_list(list, link3);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);

  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link2);

  EXPECT_EQ(link2.prev_link, &link3);
  EXPECT_EQ(link2.next_link, &link1);

  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);
}
