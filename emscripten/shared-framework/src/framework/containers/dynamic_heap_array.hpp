
#pragma once

#include <string>
#include <memory>
#include <iostream>



template<typename dynamic_heap_array>
class dynamic_heap_array_iterator
{
public:
    using ValueType = typename dynamic_heap_array::ValueType;

private:
    ValueType* _ptr;

public:
    dynamic_heap_array_iterator(ValueType* ptr)
        : _ptr(ptr)
    {}

    dynamic_heap_array_iterator& operator++() // ++pre
    {
        ++_ptr;
        return *this;
    }
    dynamic_heap_array_iterator& operator++(int) // post++
    {
        dynamic_heap_array_iterator copy = *this;
        ++(*this);
        return copy;
    }

    dynamic_heap_array_iterator& operator--() // --pre
    {
        --_ptr;
        return *this;
    }
    dynamic_heap_array_iterator& operator--(int) // post--
    {
        dynamic_heap_array_iterator copy = *this;
        --(*this);
        return copy;
    }

    ValueType& operator[](int index)
    {
        return *(_ptr + index);
    }
    ValueType* operator->()
    {
        return _ptr;
    }
    ValueType& operator*()
    {
        return *_ptr;
    }

    bool operator==(const dynamic_heap_array_iterator& other) const
    {
        return _ptr == other._ptr;
    }
    bool operator!=(const dynamic_heap_array_iterator& other) const
    {
        return !(*this == other);
    }
};



template<typename Type, typename Allocator = std::allocator<Type>>
class dynamic_heap_array
{
public:
    using ValueType = Type;
    using Iterator = dynamic_heap_array_iterator<dynamic_heap_array<Type>>;

private:
    std::size_t _capacity = 0;
    std::size_t _size = 0;
    Type* _data = nullptr;

public:
    dynamic_heap_array() = default;

    // disable move/copy
    dynamic_heap_array(const dynamic_heap_array& other) = delete;
    dynamic_heap_array(dynamic_heap_array&& other) = delete;
    dynamic_heap_array& operator=(const dynamic_heap_array& other) = delete;
    dynamic_heap_array& operator=(dynamic_heap_array&& other) = delete;
    // disable move/copy

    ~dynamic_heap_array()
    {
        clear();

        // ::operator delete(_data, _capacity * sizeof(Type)); // only deallocate

        Allocator alloc;
        alloc.deallocate(_data, _capacity);
    }

    Iterator begin() { return Iterator(_data); }
    Iterator end() { return Iterator(_data + _size); }

    void push_back(const Type& value)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        _data[_size++] = value; // copy
    }

    void push_back(Type&& value)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        // new (&_data[_size++]) Type(std::move(value)); // move ctor call

        Allocator alloc;
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
        traits_t::construct(alloc, _data + _size, std::move(value)); // move ctor call

        _size += 1;
    }

    template<typename...Args>
    Type& emplace_back(Args&&... args)
    {
        if (_size == _capacity)
            _realloc(_capacity * 2);

        // new (&_data[_size]) Type(std::forward<Args>(args)...); // move ctor call
        Allocator alloc;
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
        traits_t::construct(alloc, _data + _size, std::forward<Args>(args)...); // move ctor call

        std::size_t index = _size;
        _size += 1;

        return _data[index];
    }

    void pop_back()
    {
        if (_size == 0)
            return;

        // _data[--_size].~Type();

        --_size;
        Allocator alloc;
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
        traits_t::destroy(alloc, _data + _size);
    }

    void clear()
    {
        Allocator alloc;
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait

        for (std::size_t ii = 0; ii < _size; ++ii)
            // _data[ii].~Type();
            traits_t::destroy(alloc, _data + ii);

        _size = 0;
    }

    void pre_allocate(std::size_t capacity)
    {
        _realloc(capacity);
    }

    void unsorted_erase(std::size_t index)
    {
        if (_size > 1 && index + 1 < _size)
        {
            // std::swap(, );
            // _data[index] = std::move(_data[_size - 1]);

            // std::cout << "_size " << _size << std::endl;
            // std::cout << "index " << index << std::endl;

            Allocator alloc;
            using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
            traits_t::construct(alloc, _data + index, std::move(_data[_size - 1])); // move ctor call
        }
        pop_back();
    }

    void sorted_erase(std::size_t index)
    {
        // std::swap(_data[index], _data[_size - 1]);
        // pop_back();

        Allocator alloc;
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
        traits_t::destroy(alloc, _data + index);

        for (std::size_t ii = index; ii < _size; ++ii)
            traits_t::construct(alloc, _data + ii, std::move(_data[ii + 1])); // move ctor call

        --_size;
    }

    bool empty() const
    {
        return _size == 0;
    }

    std::size_t size() const
    {
        return _size;
    }

    std::size_t capacity() const
    {
        return _capacity;
    }

    const Type& operator[](std::size_t index) const { return _data[index]; }
    Type& operator[](std::size_t index) { return _data[index]; }

    const Type& front() const { return _data[0]; }
    Type& front() { return _data[0]; }

    const Type& back() const { return _data[_size - 1]; }
    Type& back() { return _data[_size - 1]; }

private:
    void _realloc(std::size_t newCapacity)
    {
        if (newCapacity == 0) // true the first time
            newCapacity = 1;

        if (newCapacity < _capacity)
            return;

        // allocate, no ctor call
        // Type* newData = (Type*) ::operator new (newCapacity * sizeof(Type));

        Allocator alloc;
        Type* newData = alloc.allocate(newCapacity);

        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait

        for (std::size_t ii = 0; ii < _size; ++ii)
            // new (&newData[ii]) Type(std::move(_data[ii])); // move ctor call
            traits_t::construct(alloc, newData + ii, std::move(_data[ii])); // move ctor call

        for (std::size_t ii = 0; ii < _size; ++ii)
            // _data[ii].~Type(); // dtor call
            traits_t::destroy(alloc, _data + ii);

        // ::operator delete(_data, _capacity * sizeof(Type)); // only dealocate
        alloc.deallocate(_data, _capacity);

        _data = newData;
        _capacity = newCapacity;
    }
};
