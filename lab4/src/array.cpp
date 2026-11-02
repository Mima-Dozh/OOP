#include "figure.hpp"
#ifdef ARRAY_TEST_TYPES
#include "array_test_types.hpp"
#endif

#include <limits>
#include <stdexcept>
#include <utility>
#include "array.hpp"

template <class T>
FigureArray<T>::FigureArray() = default;

template <class T>
FigureArray<T>::FigureArray(const FigureArray &other) {
    if (other.size_) {
        data_ = std::make_shared<T[]>(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i)
            data_[i] = other.data_[i];
    }
    size_ = capacity_ = other.size_;
}

template <class T>
FigureArray<T>::FigureArray(FigureArray &&other) noexcept { swap(other); }

template <class T>
FigureArray<T> &FigureArray<T>::operator=(const FigureArray &other) {
    if (this != &other) {
        FigureArray copy(other);
        swap(copy);
    }
    return *this;
}

template <class T>
FigureArray<T> &FigureArray<T>::operator=(FigureArray &&other) noexcept {
    if (this != &other) {
        FigureArray moved(std::move(other));
        swap(moved);
    }
    return *this;
}

template <class T>
void FigureArray<T>::swap(FigureArray &other) noexcept {
    data_.swap(other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <class T>
std::size_t FigureArray<T>::size() const noexcept { return size_; }

template <class T>
std::size_t FigureArray<T>::capacity() const noexcept { return capacity_; }

template <class T>
bool FigureArray<T>::empty() const noexcept { return size_ == 0; }

template <class T>
T &FigureArray<T>::operator[](std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("index");
    return data_[index];
}

template <class T>
const T &FigureArray<T>::operator[](std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("index");
    return data_[index];
}

template <class T>
void FigureArray<T>::reserve(std::size_t capacity) {
    if (capacity <= capacity_)
        return;
    if (capacity > std::numeric_limits<std::size_t>::max() / sizeof(T))
        throw std::length_error("array too large");
    auto replacement = std::make_shared<T[]>(capacity);
    for (std::size_t i = 0; i < size_; ++i)
        replacement[i] = std::move(data_[i]);
    data_.swap(replacement);
    capacity_ = capacity;
}

template <class T>
void FigureArray<T>::push_back(T value) {
    if (size_ == capacity_) {
        const auto maximum = std::numeric_limits<std::size_t>::max() / sizeof(T);
        if (capacity_ == maximum)
            throw std::length_error("array too large");
        reserve(capacity_ == 0 ? 1 : capacity_ > maximum / 2 ? maximum : capacity_ * 2);
    }
    data_[size_] = std::move(value);
    ++size_;
}

template <class T>
void FigureArray<T>::erase(std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("index");
    for (std::size_t i = index; i + 1 < size_; ++i)
        data_[i] = std::move(data_[i + 1]);
    data_[size_ - 1] = T{};
    --size_;
}

template <class T>
void FigureArray<T>::clear() noexcept {
    data_.reset();
    size_ = capacity_ = 0;
}

#ifdef ARRAY_TEST_TYPES
template class FigureArray<MoveCounter>;
#else
template class FigureArray<Square<int>>;
template class FigureArray<std::shared_ptr<Figure<int>>>;
template class FigureArray<std::shared_ptr<Figure<double>>>;
#endif
