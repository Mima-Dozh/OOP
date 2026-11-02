#pragma once
#include <cstddef>
#include <list>
#include <memory>
#include <memory_resource>

class MemoryResource final : public std::pmr::memory_resource {
    struct Block {
        std::size_t offset, size, payload = 0, bytes = 0, alignment = 0;
        bool free = true;
    };
    std::unique_ptr<std::byte[]> memory_;
    std::size_t capacity_;
    std::list<Block> blocks_;

  protected:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;

  public:
    explicit MemoryResource(std::size_t bytes);
    MemoryResource(const MemoryResource &) = delete;
    MemoryResource &operator=(const MemoryResource &) = delete;
    // unique_ptr releases the entire block, including allocations not returned by clients.
    ~MemoryResource() override;
    std::size_t capacity() const noexcept;
    std::size_t free_bytes() const noexcept;
    std::size_t outstanding() const noexcept;
};
