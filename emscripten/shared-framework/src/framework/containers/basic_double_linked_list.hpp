
#pragma once

#include <functional>

struct basic_double_linked_list
{
  struct link
  {
    link* prev_link = nullptr;
    link* next_link = nullptr;
  };

  link* head_link = nullptr;

  static void add(basic_double_linked_list& list, link& newLink)
  {
    // add as head of list

    newLink.prev_link = nullptr;
    newLink.next_link = list.head_link;
    if (list.head_link)
      list.head_link->prev_link = &newLink;
    list.head_link = &newLink;
  }

  static void remove(basic_double_linked_list& list, link& oldLink)
  {
    // remove from list

    if (oldLink.prev_link)
      oldLink.prev_link->next_link = oldLink.next_link;
    if (oldLink.next_link)
      oldLink.next_link->prev_link = oldLink.prev_link;

    if (list.head_link == &oldLink)
      list.head_link = oldLink.next_link;

    reset(oldLink);
  }

  static void replace(basic_double_linked_list& list, link& oldLink, link& newLink)
  {
    // replace in list

    newLink.prev_link = oldLink.prev_link;
    newLink.next_link = oldLink.next_link;

    oldLink.prev_link = nullptr;
    oldLink.next_link = nullptr;

    if (newLink.prev_link) newLink.prev_link->next_link = &newLink;
    if (newLink.next_link) newLink.next_link->prev_link = &newLink;

    if (list.head_link == &oldLink)
      list.head_link = &newLink;
  }

  static void reset(link& oldLink)
  {
    oldLink.prev_link = nullptr;
    oldLink.next_link = nullptr;
  }

  static void reset(basic_double_linked_list& list)
  {
    link* currLink = list.head_link;
    while (currLink)
    {
      link* tmpLink = currLink;
      currLink = currLink->next_link;
      reset(*tmpLink);
    }
    list.head_link = nullptr;
  }

  // TODO: loop -> for_each ?

  template<typename T>
  static void loop(basic_double_linked_list& list, const std::function<void(T*)>& callback)
  {
    link* currLink = list.head_link;
    while (currLink)
    {
      callback(reinterpret_cast<T*>(currLink));
      currLink = currLink->next_link;
    }
  }

  template<typename T>
  static void loop_and_reset(basic_double_linked_list& list, const std::function<void(T*)>& callback)
  {
    link* currLink = list.head_link;
    while (currLink)
    {
      callback(reinterpret_cast<T*>(currLink));

      link* tmpLink = currLink;
      currLink = currLink->next_link;
      reset(*tmpLink);
    }
    list.head_link = nullptr;
  }

};
