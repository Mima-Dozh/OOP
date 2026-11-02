#include "memory_resource.hpp"

#include <iterator>
#include <stdexcept>

MemoryResource::MemoryResource(std::size_t bytes)
    : memory_(std::make_unique<std::byte[]>(bytes)), capacity_(bytes) {
    if (!bytes)
        throw std::invalid_argument("zero resource capacity");
    blocks_.push_back({0, bytes});
}

MemoryResource::~MemoryResource() = default;

std::size_t MemoryResource::capacity() const noexcept { return capacity_; }

void *MemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    if (alignment == 0 || (alignment & (alignment - 1)))
        throw std::bad_alloc();
    const auto needed = bytes ? bytes : 1;
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
        if (!it->free)
            continue;
        void *pointer = memory_.get() + it->offset;
        auto space = it->size;
        if (!std::align(alignment, needed, pointer, space))
            continue;
        const auto payload = static_cast<std::byte *>(pointer) - memory_.get();
        const auto used = static_cast<std::size_t>(payload) - it->offset + needed;
        // Allocate metadata before modifying the old free block: strong guarantee.
        if (used < it->size)
            blocks_.insert(std::next(it), {it->offset + used, it->size - used});
        it->size = used;
        it->payload = static_cast<std::size_t>(payload);
        it->bytes = bytes;
        it->alignment = alignment;
        it->free = false;
        return pointer;
    }
    throw std::bad_alloc();
}

void MemoryResource::do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) {
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
        if (it->free || memory_.get() + it->payload != pointer)
            continue;
        if (bytes != it->bytes || alignment != it->alignment)
            throw std::invalid_argument("deallocation size/alignment mismatch");
        it->free = true;
        it->payload = it->bytes = it->alignment = 0;
        if (it != blocks_.begin()) {
            auto prev = std::prev(it);
            if (prev->free) {
                prev->size += it->size;
                blocks_.erase(it);
                it = prev;
            }
        }
        auto next = std::next(it);
        if (next != blocks_.end() && next->free) {
            it->size += next->size;
            blocks_.erase(next);
        }
        return;
    }
    throw std::invalid_argument("foreign pointer or double deallocation");
}

bool MemoryResource::do_is_equal(const std::pmr::memory_resource &other) const noexcept {
    return this == &other;
}

std::size_t MemoryResource::free_bytes() const noexcept {
    std::size_t n = 0;
    for (const auto &b : blocks_)
        if (b.free)
            n += b.size;
    return n;
}

std::size_t MemoryResource::outstanding() const noexcept {
    std::size_t n = 0;
    for (const auto &b : blocks_)
        if (!b.free)
            ++n;
    return n;
}
