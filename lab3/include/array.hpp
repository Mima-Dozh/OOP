#pragma once

#include <cstddef>

// Owns the storage. Pointer elements retain their own ownership semantics.
class Figure;

class FigureArray {
    Figure **data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    void reserve(std::size_t capacity);

  public:
    FigureArray();
    ~FigureArray();
    FigureArray(const FigureArray &other);
    FigureArray(FigureArray &&other) noexcept;
    FigureArray &operator=(const FigureArray &other);
    FigureArray &operator=(FigureArray &&other) noexcept;
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    Figure *&operator[](std::size_t index);
    Figure *const &operator[](std::size_t index) const;
    void push_back(Figure *value);
    void erase(std::size_t index);
    void clear() noexcept;
};
