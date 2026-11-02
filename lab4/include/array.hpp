#pragma once

#include <cstddef>
#include <memory>

// Owns the storage. Pointer elements retain their own ownership semantics.
template <class T> class FigureArray {
    std::shared_ptr<T[]> data_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

  public:
    FigureArray();
    FigureArray(const FigureArray &other);
    FigureArray(FigureArray &&other) noexcept;
    FigureArray &operator=(const FigureArray &other);
    FigureArray &operator=(FigureArray &&other) noexcept;
    void swap(FigureArray &other) noexcept;
    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
    T &operator[](std::size_t index);
    const T &operator[](std::size_t index) const;
    void reserve(std::size_t capacity);
    void push_back(T value);
    void erase(std::size_t index);
    void clear() noexcept;
};
