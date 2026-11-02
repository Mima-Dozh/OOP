#pragma once

struct MoveCounter {
    inline static int copies = 0;
    inline static int moves = 0;
    int value = 0;
    MoveCounter() = default;
    explicit MoveCounter(int v) : value(v) {}
    MoveCounter(const MoveCounter &other) : value(other.value) { ++copies; }
    MoveCounter(MoveCounter &&other) noexcept : value(other.value) { ++moves; }
    MoveCounter &operator=(const MoveCounter &other) {
        value = other.value;
        ++copies;
        return *this;
    }
    MoveCounter &operator=(MoveCounter &&other) noexcept {
        value = other.value;
        ++moves;
        return *this;
    }
};
