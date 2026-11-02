#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

#include "decimal.h"


Decimal::Decimal() : Decimal({0}) {}

Decimal::Decimal(const std::initializer_list<unsigned char>& digits)
    : size_(digits.size()), digits_(nullptr) {
    if (digits.size() == 0) {
        throw std::invalid_argument("Decimal digit list must not be empty");
    }
    for (auto digit : digits) {
        if (digit > 9) {
            throw std::invalid_argument("Decimal digit must be in [0, 9]");
        }
    }
    digits_ = new unsigned char[size_]{};
    std::copy(digits.begin(), digits.end(), digits_);
    normalize();
}

Decimal::Decimal(const std::string& number)
    : size_(number.size()), digits_(nullptr) {
    if (number.empty()) {
        throw std::invalid_argument("Decimal string must not be empty");
    }
    for (auto digit : number) {
        if (digit < '0' || digit > '9') {
            throw std::invalid_argument("Decimal string must contain only digits");
        }
    }
    digits_ = new unsigned char[size_];
    for (std::size_t i = 0; i < size_; ++i) {
        digits_[i] = static_cast<unsigned char>(number[size_ - 1 - i] - '0');
    }
    normalize();
}

Decimal::Decimal(const Decimal& other)
    : size_(other.size()), digits_(new unsigned char[size_]) {
    for (std::size_t i = 0; i < size_; ++i) {
        digits_[i] = other[i];
    }
}

Decimal::Decimal(Decimal&& other) : Decimal(other) {}

Decimal::~Decimal() noexcept {
    delete[] digits_;
}

Decimal& Decimal::operator=(const Decimal& other) {
    if (this != &other) {
        Decimal copy(other);
        swap(copy);
    }
    return *this;
}

Decimal& Decimal::operator=(Decimal&& other) {
    return operator=(static_cast<const Decimal&>(other));
}

Decimal Decimal::operator+(const Decimal& other) const {
    const auto length = std::max(size(), other.size());
    if (length == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error("Decimal sum is too large");
    }
    Decimal result;
    auto* digits = new unsigned char[length + 1];
    auto carry = 0U;
    for (std::size_t i = 0; i < length; ++i) {
        const auto left = i < size() ? (*this)[i] : 0U;
        const auto right = i < other.size() ? other[i] : 0U;
        const auto sum = left + right + carry;
        digits[i] = static_cast<unsigned char>(sum % 10);
        carry = sum >= 10 ? 1 : 0;
    }
    if (carry != 0) {
        digits[length] = static_cast<unsigned char>(carry);
    }
    delete[] result.digits_;
    result.digits_ = digits;
    result.size_ = length + (carry != 0 ? 1 : 0);
    return result;
}

Decimal Decimal::operator-(const Decimal& other) const {
    if (*this < other) {
        throw std::underflow_error("Unsigned Decimal subtraction is negative");
    }
    Decimal result(*this);
    auto borrow = 0;
    for (std::size_t i = 0; i < result.size_; ++i) {
        const auto right = i < other.size() ? other[i] : 0;
        auto difference = result[i] - right - borrow;
        borrow = difference < 0 ? 1 : 0;
        if (borrow != 0) {
            difference += 10;
        }
        result.digits_[i] = static_cast<unsigned char>(difference);
    }
    result.normalize();
    return result;
}

Decimal& Decimal::operator+=(const Decimal& other) {
    auto result = *this + other;
    swap(result);
    return *this;
}

Decimal& Decimal::operator-=(const Decimal& other) {
    auto result = *this - other;
    swap(result);
    return *this;
}

bool Decimal::operator==(const Decimal& other) const noexcept {
    return compare(other) == 0;
}

bool Decimal::operator!=(const Decimal& other) const noexcept {
    return compare(other) != 0;
}

bool Decimal::operator<(const Decimal& other) const noexcept {
    return compare(other) < 0;
}

bool Decimal::operator>(const Decimal& other) const noexcept {
    return compare(other) > 0;
}

bool Decimal::operator<=(const Decimal& other) const noexcept {
    return compare(other) <= 0;
}

bool Decimal::operator>=(const Decimal& other) const noexcept {
    return compare(other) >= 0;
}

std::size_t Decimal::size() const noexcept {
    return size_;
}

unsigned char Decimal::operator[](std::size_t index) const {
    if (index >= size()) {
        throw std::out_of_range("Decimal digit index is out of range");
    }
    return digits_[index];
}

std::string Decimal::to_string() const {
    std::string result(size(), '0');
    for (std::size_t i = 0; i < size(); ++i) {
        result[size() - 1 - i] = static_cast<char>('0' + (*this)[i]);
    }
    return result;
}

void Decimal::swap(Decimal& other) noexcept {
    std::swap(size_, other.size_);
    std::swap(digits_, other.digits_);
}

void Decimal::normalize() noexcept {
    while (size_ > 1 && digits_[size_ - 1] == 0) {
        --size_;
    }
}

int Decimal::compare(const Decimal& other) const noexcept {
    if (size() != other.size()) {
        return size() < other.size() ? -1 : 1;
    }
    for (auto i = size(); i > 0; --i) {
        if ((*this)[i - 1] != other[i - 1]) {
            return (*this)[i - 1] < other[i - 1] ? -1 : 1;
        }
    }
    return 0;
}
