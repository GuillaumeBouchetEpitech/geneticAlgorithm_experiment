
#include "headers.hpp"

TEST(basic_double_linked_list__loop_and_reset_feature,
     can_loop_and_reset_list_a_list_of_1_link) {
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

  int totalIteration = 0;
  std::array<Link*, 1> expectedLinks = {{&link1}};

  List::loop_list_links_and_reset<Link>(
    list, [this, &expectedLinks, &totalIteration](Link* inLink) -> void {
      EXPECT_EQ(inLink, expectedLinks[totalIteration]);
      ++totalIteration;
    });
  EXPECT_EQ(totalIteration, 1);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list__loop_and_reset_feature,
     can_loop_and_reset_list_a_list_of_2_links) {
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

  int totalIteration = 0;
  std::array<Link*, 2> expectedLinks = {{&link2, &link1}};

  List::loop_list_links_and_reset<Link>(
    list, [this, &expectedLinks, &totalIteration](Link* inLink) -> void {
      EXPECT_EQ(inLink, expectedLinks[totalIteration]);
      ++totalIteration;
    });
  EXPECT_EQ(totalIteration, 2);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list__loop_and_reset_feature,
     can_loop_and_reset_list_a_list_of_3_links) {
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
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);

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

  int totalIteration = 0;
  std::array<Link*, 3> expectedLinks = {{&link3, &link2, &link1}};

  List::loop_list_links_and_reset<Link>(
    list, [this, &expectedLinks, &totalIteration](Link* inLink) -> void {
      EXPECT_EQ(inLink, expectedLinks[totalIteration]);
      ++totalIteration;
    });
  EXPECT_EQ(totalIteration, 3);

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);
}
