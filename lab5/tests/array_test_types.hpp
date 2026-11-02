#pragma once
#include <string>
#include <stdexcept>

struct Record {
    std::string name;
    int age;
    double score;
};

struct ThrowCopy {
    inline static int live = 0;
    inline static bool fail = false;
    ThrowCopy() { ++live; }
    ThrowCopy(const ThrowCopy &) {
        if (fail)
            throw std::runtime_error("copy");
        ++live;
    }
    ThrowCopy(ThrowCopy &&) { ++live; }
    ~ThrowCopy() { --live; }
    ThrowCopy &operator=(ThrowCopy &&) = default;
};

struct alignas(128) Wide {
    int value;
};
