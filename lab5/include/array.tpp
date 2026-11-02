#pragma once

#include "array.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

// Included by array.hpp so template definitions are available to callers.

template <class T>
Array<T>::Array(std::pmr::memory_resource *resource)
    : allocator_(resource ? resource : throw std::invalid_argument("null resource")) {}

template <class T>
Array<T>::Array(const Array &other)
    : Array(other, other.allocator_.resource()) {}

template <class T>
Array<T>::Array(const Array &other, std::pmr::memory_resource *resource) : Array(resource) {
    try {
        reserve(other.size_);
        for (const auto &item : other)
            push_back(item);
    } catch (...) {
        release();
        throw;
    }
}

template <class T>
Array<T>::Array(Array &&other) noexcept
    : allocator_(other.allocator_.resource()) {
    swap_storage(other);
}

template <class T>
Array<T>::~Array() { release(); }

template <class T>
typename Array<T>::iterator Array<T>::begin() { return iterator(data_.get()); }

template <class T>
typename Array<T>::const_iterator Array<T>::begin() const { return const_iterator(data_.get()); }

template <class T>
std::size_t Array<T>::capacity() const noexcept { return capacity_; }

template <class T>
typename Array<T>::const_iterator Array<T>::cbegin() const { return begin(); }

template <class T>
typename Array<T>::const_iterator Array<T>::cend() const { return end(); }

template <class T>
void Array<T>::clear() noexcept {
    while (size_)
        Traits::destroy(allocator_, data_.get() + --size_);
}

template <class T>
bool Array<T>::empty() const noexcept { return size_ == 0; }

template <class T>
typename Array<T>::iterator Array<T>::end() { return iterator(size_ ? data_.get() + size_ : data_.get()); }

template <class T>
typename Array<T>::const_iterator Array<T>::end() const { return const_iterator(size_ ? data_.get() + size_ : data_.get()); }

template <class T>
void Array<T>::erase(std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("index");
    for (auto i = index; i + 1 < size_; ++i)
        data_[i] = std::move(data_[i + 1]);
    Traits::destroy(allocator_, data_.get() + --size_);
}

template <class T>
Array<T> &Array<T>::operator=(const Array &other) {
    if (this != &other) {
        Array copy(other, allocator_.resource());
        swap_storage(copy);
    }
    return *this;
}

template <class T>
Array<T> &Array<T>::operator=(Array &&other) {
    if (this == &other)
        return *this;
    if (allocator_.resource()->is_equal(*other.allocator_.resource())) {
        release();
        swap_storage(other);
    } else {
        Array temp(allocator_.resource());
        temp.reserve(other.size_);
        for (auto &value : other)
            temp.push_back(std::move(value));
        swap_storage(temp);
        other.clear();
    }
    return *this;
}

template <class T>
T &Array<T>::operator[](std::size_t i) {
    if (i >= size_)
        throw std::out_of_range("index");
    return data_[i];
}

template <class T>
const T &Array<T>::operator[](std::size_t i) const {
    if (i >= size_)
        throw std::out_of_range("index");
    return data_[i];
}

template <class T>
void Array<T>::push_back(T value) {
    if (size_ == capacity_) {
        if (capacity_ > std::numeric_limits<std::size_t>::max() / 2)
            throw std::length_error("capacity overflow");
        reserve(capacity_ ? capacity_ * 2 : 1);
    }
    Traits::construct(allocator_, data_.get() + size_, std::move(value));
    ++size_;
}

template <class T>
void Array<T>::release() noexcept {
    clear();
    data_.reset();
    capacity_ = 0;
}

template <class T>
void Array<T>::reserve(std::size_t capacity) {
    if (capacity <= capacity_)
        return;
    if (capacity > std::numeric_limits<std::size_t>::max() / sizeof(T))
        throw std::length_error("capacity overflow");
    Buffer replacement(allocator_.allocate(capacity),
                       BufferDeleter{allocator_.resource(), capacity});
    std::size_t built = 0;
    try {
        for (; built < size_; ++built)
            Traits::construct(allocator_, replacement.get() + built,
                              std::move_if_noexcept(data_[built]));
    } catch (...) {
        while (built)
            Traits::destroy(allocator_, replacement.get() + --built);
        throw;
    }
    const auto count = size_;
    release();
    data_ = std::move(replacement);
    size_ = count;
    capacity_ = capacity;
}

template <class T>
std::size_t Array<T>::size() const noexcept { return size_; }

template <class T>
void Array<T>::swap_storage(Array &other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <class T>
void Array<T>::BufferDeleter::operator()(T *pointer) const noexcept {
    Alloc(resource).deallocate(pointer, capacity);
}

template <class T>
template <bool Const>
Array<T>::Iterator<Const>::Iterator(Pointer pointer) : pointer_(pointer) {}

template <class T>
template <bool Const>
template <bool C>
    requires C
Array<T>::Iterator<Const>::Iterator(const Iterator<false> &other) : pointer_(other.pointer_) {}

template <class T>
template <bool Const>
typename Array<T>::template Iterator<Const>::reference Array<T>::Iterator<Const>::operator*() const { return *pointer_; }

template <class T>
template <bool Const>
typename Array<T>::template Iterator<Const> &Array<T>::Iterator<Const>::operator++() {
    ++pointer_;
    return *this;
}

template <class T>
template <bool Const>
typename Array<T>::template Iterator<Const> Array<T>::Iterator<Const>::operator++(int) {
    auto old = *this;
    ++*this;
    return old;
}

template <class T>
template <bool Const>
typename Array<T>::template Iterator<Const>::Pointer Array<T>::Iterator<Const>::operator->() const { return pointer_; }
