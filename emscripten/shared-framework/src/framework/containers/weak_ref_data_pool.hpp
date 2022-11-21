
#pragma once

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
template<typename InternalBaseType, typename PublicBaseType = InternalBaseType>
class weak_ref_data_pool
{
public:
  struct weak_ref;

public:

  // struct InternalLinkData
  // {
  //   weak_ref_data_pool* _pool = nullptr;
  //   int _index = -1;
  // };

  struct internal_data
    : public InternalBaseType
  {
  public:
    int _index = -1;
    bool _is_active = false;
    weak_ref* _refHead = nullptr;

  public:
    using InternalBaseType::InternalBaseType; // reuse parent InternalBaseType class ctor(s)

  public:
    internal_data(const internal_data& other) = delete;
    internal_data(internal_data&& other)
      : InternalBaseType(std::move(other))
    {
      _doMove(other);
    }

  public:
    internal_data& operator=(const internal_data& other) = delete;
    internal_data& operator=(internal_data&& other)
    {
      _doMove(other);
      return InternalBaseType::operator=(std::move(other));
    }

  private:
    void _doCopy(const internal_data& other)
    {
      // weak_ref_data_pool::weak_ref_data_pool(other);
      _index = other._index;
      _is_active = other._is_active;
      _refHead = other._refHead;
    }

    // void _doMove(internal_data&& other)
    void _doMove(internal_data& other)
    {
      // weak_ref_data_pool::weak_ref_data_pool(std::move(other));
      // _index = std::move(other._index);
      // _is_active = std::move(other._is_active);
      // _refHead = std::move(other._refHead);
      std::swap(_index, other._index);
      std::swap(_is_active, other._is_active);
      std::swap(_refHead, other._refHead);

      // std::cout << " -> _index=" << _index << std::endl;
      // std::cout << " -> _is_active=" << _is_active << std::endl;
      // std::cout << " -> other._index=" << other._index << std::endl;
      // std::cout << " -> other._is_active=" << other._is_active << std::endl;
    }

  public:
    void invalidate_all_ref()
    {
      weak_ref* currRef = _refHead;
      while (currRef)
      {
        weak_ref* tmpRef = currRef;
        currRef = currRef->_nextRef;

        tmpRef->_pool = nullptr;
        tmpRef->_index = -1;
        tmpRef->_nextRef = nullptr;
        tmpRef->_prevRef = nullptr;
      }
    }

    void sync_all_ref_index()
    {
      weak_ref* currRef = _refHead;
      while (currRef)
      {
        currRef->_index = _index;
        currRef = currRef->_nextRef;
        // weak_ref* tmpRef = currRef;

        // tmpRef->_pool = nullptr;
        // tmpRef->_nextRef = nullptr;
        // tmpRef->_prevRef = nullptr;
      }
    }
  };

public:
  struct weak_ref
  {
    friend weak_ref_data_pool;

  private:
    weak_ref_data_pool* _pool = nullptr;
    int32_t _index = -1;
    weak_ref* _prevRef = nullptr;
    weak_ref* _nextRef = nullptr;

  public:
    weak_ref() = default;

  private:
    weak_ref(weak_ref_data_pool* pool, std::size_t index)
      : _pool(pool)
      , _index(int32_t(index))
    {
      // CTOR

      if (_index < 0 || _pool == nullptr || _index > int32_t(_pool->_itemsPool.size()))
      {
        _index = -1;
        _pool = nullptr;
        return;
      }

      // add to the list
      _nextRef = _pool->_itemsPool[std::size_t(_index)]._refHead;
      if (_pool->_itemsPool[std::size_t(_index)]._refHead)
        _pool->_itemsPool[std::size_t(_index)]._refHead->_prevRef = this;
      _pool->_itemsPool[std::size_t(_index)]._refHead = this;
    }

  public:
    weak_ref(const weak_ref& other)
    {
      _doCopy(other);
    }

    weak_ref(weak_ref&& other)
    {
      _doMove(std::move(other));
    }

    ~weak_ref()
    {
      // remove from list
      if (_prevRef) _prevRef->_nextRef = _nextRef;
      if (_nextRef) _nextRef->_prevRef = _prevRef;
      if (_pool && _index >= 0 && _pool->_itemsPool[std::size_t(_index)]._refHead == this)
        _pool->_itemsPool[std::size_t(_index)]._refHead = _nextRef;

      _nextRef = nullptr;
      _prevRef = nullptr;
      _pool = nullptr;
      _index = -1;
    }

  public:
    weak_ref& operator=(const weak_ref& other)
    {
      _doCopy(other);
      return *this;
    }

    weak_ref& operator=(weak_ref&& other)
    {
      _doMove(std::move(other));
      return *this;
    }

  private:
    void _doCopy(const weak_ref& other)
    {
      _pool = other._pool;
      _index = other._index;

      if (_index < 0)
        return;

      // add to the list
      _nextRef = _pool->_itemsPool[std::size_t(_index)]._refHead;
      if (_pool->_itemsPool[std::size_t(_index)]._refHead)
        _pool->_itemsPool[std::size_t(_index)]._refHead->_prevRef = this;
      _pool->_itemsPool[std::size_t(_index)]._refHead = this;
    }

    void _doMove(weak_ref&& other)
    {
      std::swap(_pool, other._pool);
      std::swap(_index, other._index);

      if (_index < 0)
        return;

      // replace in list
      std::swap(_nextRef, other._nextRef);
      std::swap(_prevRef, other._prevRef);
      if (_nextRef) _nextRef->_prevRef = this;
      if (_prevRef) _prevRef->_nextRef = this;
      if (_pool->_itemsPool[std::size_t(_index)]._refHead == &other)
        _pool->_itemsPool[std::size_t(_index)]._refHead = this;
    }

  public:
    operator bool() const { return is_active(); }

    bool is_active() const
    {
      // // std::cout << "is_active()" << std::endl;
      // // std::cout << " -> _index=" << _index << std::endl;
      // if (_index >= 0)
      // {
      //   // std::cout << " -> _is_active=" << _pool->_itemsPool[std::size_t(_index)]._is_active << std::endl;
      // }

      return _index >= 0 && _pool->_itemsPool[std::size_t(_index)]._is_active;
    }

    PublicBaseType* get() { return _index < 0 ? nullptr : &_pool->_itemsPool[std::size_t(_index)]; }
    const PublicBaseType* get() const { return _index < 0 ? nullptr : &_pool->_itemsPool[std::size_t(_index)]; }

    PublicBaseType* operator->() { return get(); }
    const PublicBaseType* operator->() const { return get(); }

    PublicBaseType& operator*() { return *get(); }
    const PublicBaseType& operator*() const { return *get(); }
  };

private:
  dynamic_heap_array<internal_data> _itemsPool;

public:
  weak_ref_data_pool() = default;
  ~weak_ref_data_pool()
  {
    clear();
  }

  // disable move/copy
  weak_ref_data_pool(const weak_ref_data_pool& other) = delete;
  weak_ref_data_pool(weak_ref_data_pool&& other) = delete;
  weak_ref_data_pool& operator=(const weak_ref_data_pool& other) = delete;
  weak_ref_data_pool& operator=(weak_ref_data_pool&& other) = delete;
  // disable move/copy

  void pre_allocate(std::size_t newCapacity) { _itemsPool.pre_allocate(newCapacity); }

  void clear()
  {
    for (internal_data& item : _itemsPool)
      item.invalidate_all_ref();

    _itemsPool.clear();
  }

  template<typename...Args>
  weak_ref acquire(Args&&... args)
  {
    if (_itemsPool.size() == _itemsPool.capacity())
      return weak_ref(nullptr, std::size_t(-1));

    const std::size_t index = _itemsPool.size();

    internal_data& currData = _itemsPool.emplace_back(std::forward<Args>(args)...);

    currData._index = int32_t(index);
    currData._is_active = true;

    // std::cout << "acquire index=" << index << std::endl;
    // std::cout << "acquire _index=" << _itemsPool[index]._index << std::endl;
    // std::cout << "acquire _is_active=" << _itemsPool[index]._is_active << std::endl;

    return weak_ref(this, std::size_t(index));
  }

  weak_ref get(uint32_t index) { return weak_ref(this, std::size_t(index)); }
  const weak_ref get(uint32_t index) const { return weak_ref(const_cast<weak_ref_data_pool<InternalBaseType, PublicBaseType>*>(this), std::size_t(index)); }
  std::size_t size() const { return _itemsPool.size(); }
  bool empty() const { return _itemsPool.empty(); }

public:
  int32_t get_index(weak_ref& ref) { return ref._index; }
  int32_t get_index(PublicBaseType* pTarget) const { return reinterpret_cast<internal_data*>(pTarget)->_index; }

public:
  bool is_active(weak_ref& ref) { return ref.is_active(); }
  bool is_active(std::size_t index) const { return is_active(&_itemsPool[std::size_t(index)]); }
  bool is_active(PublicBaseType* pTarget) const { return reinterpret_cast<internal_data*>(pTarget)->_is_active; }

public:
  void release(weak_ref& ref) { release(ref.get()); }
  void release(std::size_t index) { release(&_itemsPool[index]); }

  void release(PublicBaseType* pExternalData)
  {

    // std::cout << "test test test value=" << pExternalData->value << std::endl;

    internal_data* pinternal_data = reinterpret_cast<internal_data*>(pExternalData);

    // std::cout << "test test test _index=" << pinternal_data->_index << std::endl;
    // std::cout << "test test test _is_active=" << pinternal_data->_is_active << std::endl;
    // std::cout << "test test test _refHead=" << pinternal_data->_refHead << std::endl;

    if (pinternal_data->_is_active == false)
      return;

    pinternal_data->invalidate_all_ref();

    pinternal_data->_is_active = false;
    int32_t index = pinternal_data->_index;

    // std::cout << "pre" << std::endl;

    _itemsPool.unsorted_erase(std::size_t(index));

    // std::cout << "post" << std::endl;

    if (std::size_t(index) < _itemsPool.size())
    {
      auto& item = _itemsPool[std::size_t(index)];
      item._index = int32_t(index);
      // item._refHead
      item.sync_all_ref_index();
    }
  }

public:
  void for_each(std::function<bool(PublicBaseType&)> callback)
  {
    for (std::size_t ii = 0; ii < _itemsPool.size(); )
    {
      auto& item = _itemsPool[ii];

      if (item._is_active == false)
        continue;

      if (callback(item) == false)
      {
        release(&item);
      }
      else
      {
        ++ii;
      }
    }
  }

};
