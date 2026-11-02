#pragma once

#include <cstddef>
#include <initializer_list>
#include <string>


class Decimal {
public:
    Decimal();
    Decimal(const std::initializer_list<unsigned char>& digits);
    Decimal(const std::string& number);

    Decimal(const Decimal& other);
    Decimal(Decimal&& other);
    virtual ~Decimal() noexcept;

    Decimal& operator=(const Decimal& other);
    Decimal& operator=(Decimal&& other);
    Decimal operator+(const Decimal& other) const;
    Decimal operator-(const Decimal& other) const;
    Decimal& operator+=(const Decimal& other);
    Decimal& operator-=(const Decimal& other);

    bool operator==(const Decimal& other) const noexcept;
    bool operator!=(const Decimal& other) const noexcept;
    bool operator<(const Decimal& other) const noexcept;
    bool operator>(const Decimal& other) const noexcept;
    bool operator<=(const Decimal& other) const noexcept;
    bool operator>=(const Decimal& other) const noexcept;

    std::size_t size() const noexcept;
    // Checked, read-only access prevents invalid digits from being stored.
    unsigned char operator[](std::size_t index) const;
    std::string to_string() const;
    void swap(Decimal& other) noexcept;

private:
    std::size_t size_;
    unsigned char* digits_;

    void normalize() noexcept;
    int compare(const Decimal& other) const noexcept;
};
