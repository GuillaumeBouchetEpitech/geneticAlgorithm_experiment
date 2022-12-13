
#pragma once

#include "basic_double_linked_list.hpp"
#include "dynamic_heap_array.hpp"

// #include <vector>
#include <functional>
#include <iostream>
#include <utility>

/**
 * weak_ref_data_pool
 *
 * fixed size data pool
 * - no reallocation at runtime
 * - weak pointer to active data
 * - loop over active data
 */
template <typename InternalBaseType, typename PublicBaseType = InternalBaseType,
          std::size_t initial_size = 256, bool no_realloc = true>
class weak_ref_data_pool {
public:
  struct weak_ref;

public:
  struct internal_data : public InternalBaseType {
  public:
    int _index = -1;
    bool _is_active = false;
    basic_double_linked_list _list;

  public:
    using InternalBaseType::InternalBaseType; // reuse parent InternalBaseType
                                              // class ctor(s)

  public:
    internal_data(const internal_data& other) = delete; // block copy
    internal_data(internal_data&& other) : InternalBaseType(std::move(other)) {
      _doMove(other);
    }

    ~internal_data() { invalidate_all_ref(); }

    // internal_data(PublicBaseType&& other)
    //   : InternalBaseType(std::move(other))
    // {
    //   // _doMove(other);
    // }

  public:
    internal_data& operator=(const internal_data& other) = delete; // block copy
    internal_data& operator=(internal_data&& other) {
      _doMove(other);
      return InternalBaseType::operator=(std::move(other));
    }

    // internal_data& operator=(PublicBaseType&& other)
    // {
    //   InternalBaseType&& internal =
    //   reinterpret_cast<InternalBaseType&&>(other); _doMove(internal); return
    //   InternalBaseType::operator=(std::move(internal));
    // }

  private:
    // void _doCopy(const internal_data& other)
    // {
    //   _index = other._index;
    //   _is_active = other._is_active;
    //   _list = other._list;
    // }

    void _doMove(internal_data& other) {
      if (&other == this)
        return;
      std::swap(_index, other._index);
      std::swap(_is_active, other._is_active);
      std::swap(_list, other._list);
    }

  public:
    void invalidate_all_ref() {
      basic_double_linked_list::loop_and_reset<weak_ref>(
        _list, [](weak_ref* currLink) {
          currLink->_pool = nullptr;
          currLink->_index = -1;
        });
    }

    void sync_all_ref_index() {
      basic_double_linked_list::loop<weak_ref>(
        _list, [this](weak_ref* currLink) { currLink->_index = _index; });
    }

    void sync_all_ref_pool(weak_ref_data_pool* pool) {
      basic_double_linked_list::loop<weak_ref>(
        _list, [pool](weak_ref* currLink) { currLink->_pool = pool; });
    }
  };

public:
  struct weak_ref : protected basic_double_linked_list::link {
    friend weak_ref_data_pool;

  private:
    weak_ref_data_pool* _pool = nullptr;
    int32_t _index = -1;

  public:
    weak_ref() = default;

  private:
    weak_ref(weak_ref_data_pool* inPool, int32_t inIndex) {
      // CTOR

      if (inPool == nullptr ||
          inPool->_itemsPool.is_out_of_range(std::size_t(inIndex))) {
        return;
      }

      _pool = inPool;
      _index = inIndex;
      basic_double_linked_list& list =
        _pool->_itemsPool.at(std::size_t(_index))._list;
      basic_double_linked_list::add(list, *this);
    }

  public:
    weak_ref(const weak_ref& other) { _doCopy(other); }

    weak_ref(weak_ref&& other) { _doMove(std::move(other)); }

    ~weak_ref() { invalidate(); }

  public:
    static weak_ref make_invalid() { return weak_ref(); }

    void invalidate() {
      if (_pool) {
        basic_double_linked_list& list =
          _pool->_itemsPool.at(std::size_t(_index))._list;
        basic_double_linked_list::remove(list, *this);
      } else {
        basic_double_linked_list::reset_link(*this);
      }

      _pool = nullptr;
      _index = -1;
    }

  public:
    weak_ref& operator=(const weak_ref& other) {
      _doCopy(other);
      return *this;
    }

    weak_ref& operator=(weak_ref&& other) {
      _doMove(std::move(other));
      return *this;
    }

  private:
    void _doCopy(const weak_ref& other) {
      _pool = other._pool;
      _index = other._index;

      if (_index < 0 || _pool == nullptr)
        return;

      basic_double_linked_list& list =
        _pool->_itemsPool.at(std::size_t(_index))._list;
      basic_double_linked_list::add(list, *this);
    }

    void _doMove(weak_ref&& other) {
      std::swap(_pool, other._pool);
      std::swap(_index, other._index);

      if (_index < 0 || _pool == nullptr)
        return;

      basic_double_linked_list& list =
        _pool->_itemsPool.at(std::size_t(_index))._list;
      basic_double_linked_list::replace(list, other, *this);
    }

  public:
    operator bool() const { return is_active(); }

    bool is_active() const {
      return _index >= 0 && _pool &&
             _pool->_itemsPool.at(std::size_t(_index))._is_active;
    }

    PublicBaseType* get() {
      return _index < 0 ? nullptr : &_pool->_itemsPool.at(std::size_t(_index));
    }
    const PublicBaseType* get() const {
      return _index < 0 ? nullptr : &_pool->_itemsPool.at(std::size_t(_index));
    }

    PublicBaseType* operator->() { return get(); }
    const PublicBaseType* operator->() const { return get(); }

    PublicBaseType& operator*() { return *get(); }
    const PublicBaseType& operator*() const { return *get(); }
  };

private:
  dynamic_heap_array<internal_data, std::allocator<internal_data>, initial_size>
    _itemsPool;

public:
  weak_ref_data_pool() = default;
  ~weak_ref_data_pool() { clear(); }

  // disable copy
  weak_ref_data_pool(const weak_ref_data_pool& other) = delete;
  weak_ref_data_pool& operator=(const weak_ref_data_pool& other) = delete;
  // disable copy

  weak_ref_data_pool(weak_ref_data_pool&& other) {
    _itemsPool = std::move(other._itemsPool);
    for (auto& item : _itemsPool)
      item.sync_all_ref_pool(this);
    for (auto& item : other._itemsPool)
      item.sync_all_ref_pool(&other);
  }

  weak_ref_data_pool& operator=(weak_ref_data_pool&& other) {
    _itemsPool = std::move(other._itemsPool);
    for (auto& item : _itemsPool)
      item.sync_all_ref_pool(this);
    for (auto& item : other._itemsPool)
      item.sync_all_ref_pool(&other);

    return *this;
  }

  void pre_allocate(std::size_t newCapacity) {
    _itemsPool.pre_allocate(newCapacity);
  }

  void clear() {
    for (internal_data& item : _itemsPool)
      item.invalidate_all_ref();

    _itemsPool.clear();
  }

  // weak_ref acquire(PublicBaseType&& toMove)
  // {
  //   if (_itemsPool.size() == _itemsPool.capacity())
  //     return weak_ref::make_invalid();

  //   const std::size_t index = _itemsPool.size();

  //   // internal_data& currData = _itemsPool.push_back(std::move(toMove));
  //   internal_data testData = internal_data(std::move(toMove));
  //   internal_data& currData = _itemsPool.push_back(std::move(testData));
  //   // currData = std::move(toMove);
  //   // internal_data movedData = std::move(toMove);
  //   // internal_data& currData = _itemsPool.push_back(std::move(movedData));

  //   currData._index = int32_t(index);
  //   currData._is_active = true;

  //   return weak_ref(this, std::size_t(index));
  // }

  // weak_ref acquire()
  // {
  //   if (no_realloc == true && _itemsPool.size() == _itemsPool.capacity())
  //     return weak_ref::make_invalid();

  //   const std::size_t index = _itemsPool.size();

  //   internal_data& currData = _itemsPool.emplace_back();

  //   currData._index = int32_t(index);
  //   currData._is_active = true;

  //   return weak_ref(this, std::size_t(index));
  // }

  // weak_ref acquire(PublicBaseType&& toMove)
  // {
  //   if (no_realloc == true && _itemsPool.size() == _itemsPool.capacity())
  //     return weak_ref::make_invalid();

  //   const std::size_t index = _itemsPool.size();

  //   internal_data tmpData = std::move()

  //   internal_data& currData = _itemsPool.push_back(std::move(toMove));

  //   currData._index = int32_t(index);
  //   currData._is_active = true;

  //   // std::cout << "acquire index=" << index << std::endl;
  //   // std::cout << "acquire _index=" << _itemsPool.at(index)._index <<
  //   std::endl;
  //   // std::cout << "acquire _is_active=" << _itemsPool.at(index)._is_active
  //   << std::endl;

  //   return weak_ref(this, std::size_t(index));
  // }

  template <typename... Args> weak_ref acquire(Args&&... args) {
    if (no_realloc == true && _itemsPool.size() == _itemsPool.capacity())
      return weak_ref::make_invalid();

    const int32_t index = int32_t(_itemsPool.size());

    internal_data& currData =
      _itemsPool.emplace_back(std::forward<Args>(args)...);

    currData._index = index;
    currData._is_active = true;

    return weak_ref(this, index);
  }

  weak_ref get(uint32_t index) { return weak_ref(this, int32_t(index)); }
  const weak_ref get(uint32_t index) const {
    return weak_ref(const_cast<weak_ref_data_pool*>(this), int32_t(index));
  }
  std::size_t size() const { return _itemsPool.size(); }
  bool empty() const { return _itemsPool.empty(); }

public:
  uint32_t getRefCount(uint32_t index) {
    return index < _itemsPool.size() ? _itemsPool.at(index)._list.size : 0;
  }

public:
  int32_t get_index(weak_ref& ref) { return ref._index; }
  int32_t get_index(PublicBaseType* pTarget) const {
    return reinterpret_cast<internal_data*>(pTarget)->_index;
  }

public:
  bool is_active(weak_ref& ref) { return ref.is_active(); }
  bool is_active(std::size_t index) const {
    return is_active(&_itemsPool.at(index));
  }
  bool is_active(PublicBaseType* pTarget) const {
    return reinterpret_cast<internal_data*>(pTarget)->_is_active;
  }

public:
  void release(weak_ref& ref) { release(ref.get()); }
  void release(std::size_t index) { release(&_itemsPool.at(index)); }

  void release(PublicBaseType* pExternalData) {

    // std::cout << "test test test value=" << pExternalData->value <<
    // std::endl;

    internal_data* pinternal_data =
      reinterpret_cast<internal_data*>(pExternalData);

    // std::cout << "test test test _index=" << pinternal_data->_index <<
    // std::endl; std::cout << "test test test _is_active=" <<
    // pinternal_data->_is_active << std::endl;

    if (pinternal_data->_is_active == false)
      return;

    pinternal_data->invalidate_all_ref();

    pinternal_data->_is_active = false;
    std::size_t index = std::size_t(pinternal_data->_index);

    // std::cout << "pre" << std::endl;

    _itemsPool.unsorted_erase(std::size_t(index));

    // std::cout << "post" << std::endl;

    if (index < _itemsPool.size()) {
      auto& item = _itemsPool.at(index);
      item._index = int32_t(index);
      item.sync_all_ref_index();
    }
  }

public:
  void filter(std::function<bool(PublicBaseType&)> callback) {
    for (std::size_t index = 0; index < _itemsPool.size();) {
      auto& item = _itemsPool.at(index);

      if (item._is_active == false) {
        ++index;
        continue;
      }

      if (callback(item) == false) {
        release(&item);
      } else {
        ++index;
      }
    }
  }

  void for_each(std::function<void(PublicBaseType&)> callback) {
    for (auto& item : _itemsPool)
      if (item._is_active == true)
        callback(item);
  }

  void for_each(std::function<void(const PublicBaseType&)> callback) const {
    for (const auto& item : _itemsPool)
      if (item._is_active == true)
        callback(item);
  }

  weak_ref find_if(std::function<bool(const PublicBaseType&)> callback) const {
    for (std::size_t index = 0; index < _itemsPool.size(); ++index) {
      auto& item = _itemsPool.at(index);

      if (item._is_active == false)
        continue;

      if (callback(item) == true)
        return weak_ref(const_cast<weak_ref_data_pool*>(this), int32_t(index));
    }
    return weak_ref::make_invalid();
  }
};
