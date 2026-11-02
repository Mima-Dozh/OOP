#include <limits>
#include <stdexcept>
#include "array.hpp"

FigureArray::FigureArray() = default;

FigureArray::~FigureArray() { delete[] data_; }

FigureArray::FigureArray(const FigureArray &other) {
    if (other.size_) {
        data_ = new Figure *[other.size_]{};
        for (std::size_t i = 0; i < other.size_; ++i)
            data_[i] = other.data_[i];
    }
    size_ = capacity_ = other.size_;
}

FigureArray::FigureArray(FigureArray &&other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
}

FigureArray &FigureArray::operator=(const FigureArray &other) {
    if (this != &other) {
        auto replacement = other.size_ ? new Figure *[other.size_]{} : nullptr;
        for (std::size_t i = 0; i < other.size_; ++i)
            replacement[i] = other.data_[i];
        delete[] data_;
        data_ = replacement;
        size_ = capacity_ = other.size_;
    }
    return *this;
}

FigureArray &FigureArray::operator=(FigureArray &&other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }
    return *this;
}

std::size_t FigureArray::size() const noexcept { return size_; }

bool FigureArray::empty() const noexcept { return size_ == 0; }

Figure *&FigureArray::operator[](std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("index");
    return data_[index];
}

Figure *const &FigureArray::operator[](std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("index");
    return data_[index];
}

void FigureArray::reserve(std::size_t capacity) {
    if (capacity <= capacity_)
        return;
    if (capacity > std::numeric_limits<std::size_t>::max() / sizeof(Figure *))
        throw std::length_error("array too large");
    auto replacement = new Figure *[capacity]{};
    for (std::size_t i = 0; i < size_; ++i)
        replacement[i] = data_[i];
    delete[] data_;
    data_ = replacement;
    capacity_ = capacity;
}

void FigureArray::push_back(Figure *value) {
    if (size_ == capacity_) {
        const auto maximum = std::numeric_limits<std::size_t>::max() / sizeof(Figure *);
        if (capacity_ == maximum)
            throw std::length_error("array too large");
        std::size_t new_capacity;
        if (capacity_ == 0) {
            new_capacity = 1;
        } else if (capacity_ > maximum / 2) {
            new_capacity = maximum;
        } else {
            new_capacity = capacity_ * 2;
        }
        reserve(new_capacity);
    }
    data_[size_] = value;
    ++size_;
}

void FigureArray::erase(std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("index");
    for (std::size_t i = index; i + 1 < size_; ++i)
        data_[i] = data_[i + 1];
    data_[size_ - 1] = nullptr;
    --size_;
}

void FigureArray::clear() noexcept {
    delete[] data_;
    data_ = nullptr;
    size_ = capacity_ = 0;
}
