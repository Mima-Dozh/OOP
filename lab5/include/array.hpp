#pragma once
#include <iterator>
#include <memory>
#include <memory_resource>
#include <type_traits>

template <class T> class Array {
    using Alloc = std::pmr::polymorphic_allocator<T>;
    using Traits = std::allocator_traits<Alloc>;
    struct BufferDeleter {
        std::pmr::memory_resource *resource = nullptr;
        std::size_t capacity = 0;
        void operator()(T *pointer) const noexcept;
    };
    using Buffer = std::unique_ptr<T[], BufferDeleter>;
    Alloc allocator_;
    Buffer data_{nullptr, BufferDeleter{}};
    std::size_t size_ = 0, capacity_ = 0;
    void release() noexcept;
    void swap_storage(Array &other) noexcept;

  public:
    template <bool Const> class Iterator {
        using Pointer = std::conditional_t<Const, const T *, T *>;
        Pointer pointer_ = nullptr;
        template <bool> friend class Iterator;

      public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Const, const T &, T &>;
        using pointer = Pointer;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        Iterator() = default;
        explicit Iterator(Pointer pointer);
        template <bool C = Const>
            requires C
        Iterator(const Iterator<false> &other);
        reference operator*() const;
        Pointer operator->() const;
        Iterator &operator++();
        Iterator operator++(int);
        bool operator==(const Iterator &) const = default;
    };
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    explicit Array(std::pmr::memory_resource *resource);
    ~Array();
    Array(const Array &other);
    Array(const Array &other, std::pmr::memory_resource *resource);
    Array(Array &&other) noexcept;
    Array &operator=(const Array &other);
    Array &operator=(Array &&other);
    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    T &operator[](std::size_t i);
    const T &operator[](std::size_t i) const;
    void clear() noexcept;
    void reserve(std::size_t capacity);
    void push_back(T value);
    void erase(std::size_t index);
};

#include "array.tpp"
