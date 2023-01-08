
#pragma once

#include <functional>
#include <stdint.h>

struct basic_double_linked_list {
  struct link {
    link* prev_link = nullptr;
    link* next_link = nullptr;
  };

  link* head_link = nullptr;
  uint32_t size = 0;

  static void add(basic_double_linked_list& list, link& new_link) {
    // add as head of list

    // FROM: (headLink) -> ...
    // TO:   (headLink) -> (newLink) <--> ...

    new_link.prev_link = nullptr;
    new_link.next_link = list.head_link;
    if (list.head_link)
      list.head_link->prev_link = &new_link;
    list.head_link = &new_link;

    list.size += 1;
  }

  static void remove(basic_double_linked_list& list, link& old_link) {
    // remove from list

    // FROM: ... <--> (oldLink) <--> ...
    // TO:   ... <--> ...

    if (old_link.prev_link)
      old_link.prev_link->next_link = old_link.next_link;
    if (old_link.next_link)
      old_link.next_link->prev_link = old_link.prev_link;

    // FROM: (headLink) -> (oldLink) -> ...
    // TO:   (headLink) -> ...
    if (list.head_link == &old_link)
      list.head_link = old_link.next_link;

    reset_link(old_link);

    list.size -= 1;
  }

  static void replace(basic_double_linked_list& list, link& old_link,
                      link& new_link) {
    // replace in list

    // FROM: ... <--> (oldLink) <--> ...
    // TO:   ... <--> (newLink) <--> ...

    new_link.prev_link = old_link.prev_link;
    new_link.next_link = old_link.next_link;

    old_link.prev_link = nullptr;
    old_link.next_link = nullptr;

    if (new_link.prev_link)
      new_link.prev_link->next_link = &new_link;
    if (new_link.next_link)
      new_link.next_link->prev_link = &new_link;

    // FROM: (headLink) -> (oldLink) -> ...
    // TO:   (headLink) -> (newLink) -> ...
    if (list.head_link == &old_link)
      list.head_link = &new_link;
  }

  static void reset_link(link& old_link) {
    old_link.prev_link = nullptr;
    old_link.next_link = nullptr;
  }

  static void reset_list(basic_double_linked_list& list) {
    link* curr_link = list.head_link;
    while (curr_link) {
      link* to_reset_link = curr_link;
      curr_link = curr_link->next_link;
      reset_link(*to_reset_link);
    }
    list.head_link = nullptr;
    list.size = 0;
  }

  // TODO: loop -> for_each ?

  template <typename T>
  static void loop(basic_double_linked_list& list,
                   const std::function<void(T*)>& callback) {
    link* curr_link = list.head_link;
    while (curr_link) {
      callback(static_cast<T*>(curr_link));
      curr_link = curr_link->next_link;
    }
  }

  template <typename T>
  static void loop_and_reset(basic_double_linked_list& list,
                             const std::function<void(T*)>& callback) {
    link* curr_link = list.head_link;
    while (curr_link) {
      link* to_reset_link = curr_link;

      callback(static_cast<T*>(curr_link));
      curr_link = curr_link->next_link;

      reset_link(*to_reset_link);
    }
    list.head_link = nullptr;
    list.size = 0;
  }
};
