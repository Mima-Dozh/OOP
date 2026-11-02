#include "array_test_types.hpp"
#include "array.hpp"
#include "memory_resource.hpp"
#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>
#include <numeric>
#include <string>
static_assert(std::forward_iterator<Array<int>::iterator>);
static_assert(std::forward_iterator<Array<int>::const_iterator>);
static_assert(std::is_constructible_v<Array<int>, std::pmr::memory_resource *>);

TEST(Array, UsesProvidedFixedCapacityResource) {
    MemoryResource resource(65536);
    Array<int> values(&resource);
    values.push_back(42);
    EXPECT_THROW(values.reserve(65536 / sizeof(int) + 1), std::bad_alloc);
    EXPECT_EQ(values[0], 42);
}

TEST(Array, AcceptsExternalPmrResourceAndRejectsNull) {
    EXPECT_THROW(Array<int>(nullptr), std::invalid_argument);

    Array<int> unavailable(std::pmr::null_memory_resource());
    EXPECT_THROW(unavailable.push_back(1), std::bad_alloc);

    std::pmr::unsynchronized_pool_resource pool;
    Array<int> values(&pool);
    values.push_back(7);
    Array<int> copy(values);
    EXPECT_EQ(copy[0], 7);
    Array<int> target(std::pmr::new_delete_resource());
    target = values;
    EXPECT_EQ(target[0], 7);
    target = std::move(copy);
    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(target[0], 7);
}

TEST(Array, CopiesAndMovesUseExternalResource) {
    MemoryResource resource(65536);
    auto copy = [&resource] {
        Array<int> source(&resource);
        source.push_back(7);
        return Array<int>(source);
    }();
    copy.push_back(8);
    EXPECT_EQ(copy[0], 7);
    EXPECT_EQ(copy[1], 8);

    auto moved = [&resource] {
        Array<int> source(&resource);
        source.push_back(9);
        Array<int> target(std::move(source));
        source.push_back(10);
        EXPECT_EQ(source[0], 10);
        return target;
    }();
    moved.push_back(11);
    EXPECT_EQ(moved[0], 9);
    EXPECT_EQ(moved[1], 11);
    copy = std::move(moved);
    EXPECT_EQ(copy[0], 9);
    EXPECT_TRUE(moved.empty());
}

TEST(Resource, ReuseAndCoalescing) {
    MemoryResource r(1024);
    auto a = r.allocate(100, 1);
    auto b = r.allocate(200, 1);
    EXPECT_EQ(r.outstanding(), 2);
    r.deallocate(a, 100, 1);
    EXPECT_EQ(r.allocate(100, 1), a);
    r.deallocate(a, 100, 1);
    r.deallocate(b, 200, 1);
    EXPECT_EQ(r.free_bytes(), 1024);
    auto all = r.allocate(1024, 1);
    r.deallocate(all, 1024, 1);
    EXPECT_EQ(r.outstanding(), 0);
}
TEST(Resource, AlignmentAndExhaustion) {
    MemoryResource r(1024);
    auto p = r.allocate(64, 128);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % 128, 0);
    EXPECT_THROW((void)r.allocate(2048, 1), std::bad_alloc);
    r.deallocate(p, 64, 128);
    EXPECT_EQ(r.free_bytes(), 1024);
    EXPECT_THROW(r.deallocate(p, 64, 128), std::invalid_argument);
}
TEST(Resource, IdentityAndCleanup) {
    MemoryResource a(100), b(100);
    EXPECT_TRUE(a.is_equal(a));
    EXPECT_FALSE(a.is_equal(b));
    {
        MemoryResource r(1024);
        (void)r.allocate(512, 1);
        EXPECT_EQ(r.outstanding(), 1);
    }
    EXPECT_THROW(MemoryResource(0), std::invalid_argument);
}
TEST(Array, AlgorithmsAndConstIterator) {
    MemoryResource r(4096);
    Array<int> a(&r);
    for (int i = 1; i <= 10; ++i)
        a.push_back(i);
    EXPECT_EQ(std::accumulate(a.begin(), a.end(), 0), 55);
    const auto &c = a;
    EXPECT_EQ(std::distance(c.begin(), c.end()), 10);
    EXPECT_EQ(*std::find(c.begin(), c.end(), 7), 7);
    a.erase(4);
    EXPECT_EQ(a[4], 6);
    EXPECT_THROW(a[100], std::out_of_range);
}

TEST(Array, ComplexTypesCopyMove) {
    MemoryResource r(8192), other(8192);
    {
        Array<Record> a(&r);
        a.push_back({"Anna", 20, 4.8});
        Array<Record> b = a;
        b[0].name = "Ivan";
        EXPECT_EQ(a[0].name, "Anna");
        Array<Record> c(&other);
        c = std::move(b);
        EXPECT_TRUE(b.empty());
        EXPECT_EQ(c[0].name, "Ivan");
    }
    EXPECT_EQ(r.free_bytes(), r.capacity());
    EXPECT_EQ(other.free_bytes(), other.capacity());
}
TEST(Array, AllocationFailureKeepsValues) {
    MemoryResource r(256);
    Array<int> a(&r);
    a.push_back(42);
    EXPECT_THROW(a.reserve(1000), std::bad_alloc);
    EXPECT_EQ(a.size(), 1);
    EXPECT_EQ(a[0], 42);
}
TEST(Array, OwnedBufferCopyMoveAndReuse) {
    MemoryResource resource(8192), other(8192);
    {
        Array<std::string> source(&resource);
        source.push_back("first");
        Array<std::string> moved(std::move(source));
        EXPECT_TRUE(source.empty());
        source.push_back("reused");

        Array<std::string> target(&resource);
        target.push_back("old");
        target = std::move(moved);
        EXPECT_TRUE(moved.empty());
        EXPECT_EQ(target[0], "first");
        moved.push_back("reused again");

        Array<std::string> copy(&other);
        copy.push_back("old copy");
        copy = target;
        target.clear();
        EXPECT_EQ(copy[0], "first");
        copy.reserve(16);
        EXPECT_EQ(copy[0], "first");
    }
    EXPECT_EQ(resource.outstanding(), 0U);
    EXPECT_EQ(other.outstanding(), 0U);
    EXPECT_EQ(resource.free_bytes(), resource.capacity());
    EXPECT_EQ(other.free_bytes(), other.capacity());
}

TEST(Array, ConstructionFailureDoesNotLeak) {
    MemoryResource r(4096);
    {
        Array<ThrowCopy> a(&r);
        a.push_back(ThrowCopy{});
        ThrowCopy::fail = true;
        EXPECT_THROW(a.reserve(20), std::runtime_error);
        ThrowCopy::fail = false;
        EXPECT_EQ(a.size(), 1);
        EXPECT_EQ(ThrowCopy::live, 1);
    }
    EXPECT_EQ(ThrowCopy::live, 0);
    EXPECT_EQ(r.free_bytes(), r.capacity());
}
TEST(Array, EmptyAndOverAligned) {

    MemoryResource r(8192);
    Array<Wide> a(&r);
    EXPECT_EQ(a.begin(), a.end());
    a.push_back({7});
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(&a[0]) % 128, 0);
    EXPECT_EQ(a[0].value, 7);
}

TEST(Resource, OwnerDestroysArraysBeforeResource) {
    struct CheckReturnedMemory {
        MemoryResource &resource;
        ~CheckReturnedMemory() {
            EXPECT_EQ(resource.outstanding(), 0U);
            EXPECT_EQ(resource.free_bytes(), resource.capacity());
        }
    };
    struct ResourceData {
        MemoryResource resource{4096};
        CheckReturnedMemory check{resource};
        Array<ThrowCopy> objects{&resource};
        Array<int> values{&resource};
    };

    ASSERT_EQ(ThrowCopy::live, 0);
    auto data = std::make_unique<ResourceData>();
    data->objects.push_back(ThrowCopy{});
    data->values.push_back(42);
    EXPECT_EQ(ThrowCopy::live, 1);
    EXPECT_EQ(data->resource.outstanding(), 2U);
    data.reset();
    EXPECT_EQ(ThrowCopy::live, 0);
}

TEST(ResourceDeathTest, ManualDeletionInvalidatesLiveArray) {
#if defined(LAB5_HAS_ASAN) && GTEST_HAS_DEATH_TEST
    // Run the intentional invalid access in a child process.
    EXPECT_DEATH(
        {
            auto *resource = new MemoryResource(1024);
            Array<int> values(resource);
            values.push_back(42);

            delete resource; // Frees the buffer; values itself still exists.

            volatile int value = values[0]; // ASan must detect the freed memory.
            (void)value;
        },
        "heap-use-after-free");
#else
    GTEST_SKIP() << "Requires AddressSanitizer and death-test support";
#endif
}
