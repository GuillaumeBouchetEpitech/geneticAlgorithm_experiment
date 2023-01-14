
#include "headers.hpp"

TEST(basic_double_linked_list__replace_link, can_replace_link_in_1_link_list) {
  List list;
  Link link1;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::add_link_to_list(list, link1);

  EXPECT_EQ(list.head_link, &link1);
  EXPECT_EQ(list.size, 1);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link1, link_x);

  EXPECT_EQ(list.head_link, &link_x);
  EXPECT_EQ(list.size, 1);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);
}

TEST(basic_double_linked_list__replace_link,
     can_replace_link_in_2_links_list_target_link1) {
  List list;
  Link link1;
  Link link2;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link1, link_x);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link_x);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, &link2);
  EXPECT_EQ(link_x.next_link, nullptr);
}

TEST(basic_double_linked_list__replace_link,
     can_replace_link_in_2_links_list_target_link2) {
  List list;
  Link link1;
  Link link2;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link2, link_x);

  EXPECT_EQ(list.head_link, &link_x);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link1.prev_link, &link_x);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, &link1);
}

TEST(basic_double_linked_list__replace_link,
     can_replace_link_in_3_links_list_target_link1) {
  List list;
  Link link1;
  Link link2;
  Link link3;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

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
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link1, link_x);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link2);
  EXPECT_EQ(link2.prev_link, &link3);
  EXPECT_EQ(link2.next_link, &link_x);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, &link2);
  EXPECT_EQ(link_x.next_link, nullptr);
}

TEST(basic_double_linked_list__replace_link,
     can_replace_link_in_3_links_list_target_link2) {
  List list;
  Link link1;
  Link link2;
  Link link3;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

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
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link2, link_x);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link_x);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link1.prev_link, &link_x);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, &link3);
  EXPECT_EQ(link_x.next_link, &link1);
}

TEST(basic_double_linked_list__replace_link,
     can_replace_link_in_3_links_list_target_link3) {
  List list;
  Link link1;
  Link link2;
  Link link3;
  Link link_x;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

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
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, nullptr);

  List::discard_and_replace_link_from_list(list, link3, link_x);

  EXPECT_EQ(list.head_link, &link_x);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, &link_x);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link_x.prev_link, nullptr);
  EXPECT_EQ(link_x.next_link, &link2);
}
