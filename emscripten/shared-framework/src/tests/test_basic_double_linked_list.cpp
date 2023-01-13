
#include "framework/system/TraceLogger.hpp"
#include "framework/system/containers/basic_double_linked_list.hpp"

#include "common.hpp"

using List = basic_double_linked_list;
using Link = List::link;

void test_single_link() {
  List list;
  Link link1;

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);

  List::add_link_to_list(list, link1);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1](Link* currLink) {
      assert(currLink == &link1);
      count += 1;
    });
    assert(count == 1);
  }

  assert(list.head_link == &link1);
  assert(list.size == 1);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link1);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
    assert(count == 0);
  }

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
}

void test_double_link() {
  List list;
  Link link1;
  Link link2;

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
  assert(link2.prev_link == nullptr);
  assert(link2.next_link == nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1, &link2](Link* currLink) {
      if (count == 0)
        assert(currLink == &link2);
      else
        assert(currLink == &link1);
      count += 1;
    });
    assert(count == 2);
  }

  assert(list.head_link == &link2);
  assert(list.size == 2);
  assert(link2.prev_link == nullptr);
  assert(link2.next_link == &link1);
  assert(link1.prev_link == &link2);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link2);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1](Link* currLink) {
      assert(currLink == &link1);
      count += 1;
    });
    assert(count == 1);
  }

  assert(list.head_link == &link1);
  assert(list.size == 1);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link1);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
    assert(count == 0);
  }

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
}

void test_triple_link() {
  List list;
  Link link1;
  Link link2;
  Link link3;

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
  assert(link2.prev_link == nullptr);
  assert(link2.next_link == nullptr);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);
  List::add_link_to_list(list, link3);

  {
    int count = 0;
    List::loop_list_links<Link>(
      list, [&count, &link1, &link2, &link3](Link* currLink) {
        if (count == 0)
          assert(currLink == &link3);
        else if (count == 1)
          assert(currLink == &link2);
        else
          assert(currLink == &link1);
        count += 1;
      });
    assert(count == 3);
  }

  assert(list.head_link == &link3);
  assert(list.size == 3);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == &link2);
  assert(link2.prev_link == &link3);
  assert(link2.next_link == &link1);
  assert(link1.prev_link == &link2);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link2);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1, &link3](Link* currLink) {
      if (count == 0)
        assert(currLink == &link3);
      else
        assert(currLink == &link1);
      count += 1;
    });
    assert(count == 2);
  }

  assert(list.head_link == &link3);
  assert(list.size == 2);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == &link1);
  assert(link1.prev_link == &link3);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link3);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1](Link* currLink) {
      assert(currLink == &link1);
      count += 1;
    });
    assert(count == 1);
  }

  assert(list.head_link == &link1);
  assert(list.size == 1);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link1);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
    assert(count == 0);
  }

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
}

void test_triple_link_replaced() {
  List list;
  Link link1;
  Link link2;
  Link link3;
  Link link4;

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
  assert(link2.prev_link == nullptr);
  assert(link2.next_link == nullptr);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == nullptr);
  assert(link4.prev_link == nullptr);
  assert(link4.next_link == nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);
  List::add_link_to_list(list, link3);

  {
    int count = 0;
    List::loop_list_links<Link>(
      list, [&count, &link1, &link2, &link3](Link* currLink) {
        if (count == 0)
          assert(currLink == &link3);
        else if (count == 1)
          assert(currLink == &link2);
        else
          assert(currLink == &link1);
        count += 1;
      });
    assert(count == 3);
  }

  assert(list.head_link == &link3);
  assert(list.size == 3);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == &link2);
  assert(link2.prev_link == &link3);
  assert(link2.next_link == &link1);
  assert(link1.prev_link == &link2);
  assert(link1.next_link == nullptr);

  List::replace_link_from_list(list, link2, link4);

  {
    int count = 0;
    List::loop_list_links<Link>(
      list, [&count, &link1, &link4, &link3](Link* currLink) {
        if (count == 0)
          assert(currLink == &link3);
        else if (count == 1)
          assert(currLink == &link4);
        else
          assert(currLink == &link1);
        count += 1;
      });
    assert(count == 3);
  }

  assert(list.head_link == &link3);
  assert(list.size == 3);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == &link4);
  assert(link4.prev_link == &link3);
  assert(link4.next_link == &link1);
  assert(link1.prev_link == &link4);
  assert(link1.next_link == nullptr);

  assert(link2.prev_link == nullptr);
  assert(link2.next_link == nullptr);

  List::remove_link_from_list(list, link4);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1, &link3](Link* currLink) {
      if (count == 0)
        assert(currLink == &link3);
      else
        assert(currLink == &link1);
      count += 1;
    });
    assert(count == 2);
  }

  assert(list.head_link == &link3);
  assert(list.size == 2);
  assert(link3.prev_link == nullptr);
  assert(link3.next_link == &link1);
  assert(link1.prev_link == &link3);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link3);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count, &link1](Link* currLink) {
      assert(currLink == &link1);
      count += 1;
    });
    assert(count == 1);
  }

  assert(list.head_link == &link1);
  assert(list.size == 1);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);

  List::remove_link_from_list(list, link1);

  {
    int count = 0;
    List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
    assert(count == 0);
  }

  assert(list.head_link == nullptr);
  assert(list.size == 0);
  assert(link1.prev_link == nullptr);
  assert(link1.next_link == nullptr);
}

void test_basic_double_linked_list() {
  D_MYLOG("test_basic_double_linked_list()");

  {
    test_single_link();
    test_double_link();
    test_triple_link();
    test_triple_link_replaced();

    {
      List list;
      Link link1;
      Link link2;
      Link link3;

      assert(list.head_link == nullptr);
      assert(list.size == 0);
      assert(link1.prev_link == nullptr);
      assert(link1.next_link == nullptr);
      assert(link2.prev_link == nullptr);
      assert(link2.next_link == nullptr);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == nullptr);

      List::add_link_to_list(list, link1);
      List::add_link_to_list(list, link2);
      List::add_link_to_list(list, link3);

      {
        int count = 0;
        List::loop_list_links<Link>(
          list, [&count, &link1, &link2, &link3](Link* currLink) {
            if (count == 0)
              assert(currLink == &link3);
            else if (count == 1)
              assert(currLink == &link2);
            else
              assert(currLink == &link1);
            count += 1;
          });
        assert(count == 3);
      }

      assert(list.head_link == &link3);
      assert(list.size == 3);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == &link2);
      assert(link2.prev_link == &link3);
      assert(link2.next_link == &link1);
      assert(link1.prev_link == &link2);
      assert(link1.next_link == nullptr);

      List::reset_list(list);

      {
        int count = 0;
        List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
        assert(count == 0);
      }

      assert(list.head_link == nullptr);
      assert(list.size == 0);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == nullptr);
      assert(link2.prev_link == nullptr);
      assert(link2.next_link == nullptr);
      assert(link1.prev_link == nullptr);
      assert(link1.next_link == nullptr);
    }

    {
      List list;
      Link link1;
      Link link2;
      Link link3;

      assert(list.head_link == nullptr);
      assert(list.size == 0);
      assert(link1.prev_link == nullptr);
      assert(link1.next_link == nullptr);
      assert(link2.prev_link == nullptr);
      assert(link2.next_link == nullptr);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == nullptr);

      List::add_link_to_list(list, link1);
      List::add_link_to_list(list, link2);
      List::add_link_to_list(list, link3);

      {
        int count = 0;
        List::loop_list_links<Link>(
          list, [&count, &link1, &link2, &link3](Link* currLink) {
            if (count == 0)
              assert(currLink == &link3);
            else if (count == 1)
              assert(currLink == &link2);
            else
              assert(currLink == &link1);
            count += 1;
          });
        assert(count == 3);
      }

      assert(list.head_link == &link3);
      assert(list.size == 3);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == &link2);
      assert(link2.prev_link == &link3);
      assert(link2.next_link == &link1);
      assert(link1.prev_link == &link2);
      assert(link1.next_link == nullptr);

      {
        int count = 0;
        List::loop_list_links_and_reset<Link>(
          list, [&count, &link1, &link2, &link3](Link* currLink) {
            if (count == 0)
              assert(currLink == &link3);
            else if (count == 1)
              assert(currLink == &link2);
            else
              assert(currLink == &link1);
            count += 1;
          });
        assert(count == 3);
      }

      {
        int count = 0;
        List::loop_list_links<Link>(list, [&count](Link*) { count += 1; });
        assert(count == 0);
      }

      assert(list.head_link == nullptr);
      assert(list.size == 0);
      assert(link3.prev_link == nullptr);
      assert(link3.next_link == nullptr);
      assert(link2.prev_link == nullptr);
      assert(link2.next_link == nullptr);
      assert(link1.prev_link == nullptr);
      assert(link1.next_link == nullptr);
    }
  }

  D_MYLOG(" => DONE");
}