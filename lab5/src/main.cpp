#include "array.hpp"
#include "memory_resource.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>


struct Student {
    std::string name;
    int age;
    double grade;
};


int main() {
    MemoryResource resource(65536);
    Array<int> values(&resource);
    for (auto i = 1; i <= 10; ++i)
        values.push_back(i);
    std::cout << "Integers:";
    for (auto value : values)
        std::cout << ' ' << value;
    std::cout << "\nSum=" << std::accumulate(values.begin(), values.end(), 0) << '\n';
    values.erase(4);
    std::cout << "After erase(4):";
    std::for_each(values.begin(), values.end(), [](int v) { std::cout << ' ' << v; });
    Array<Student> students(&resource);
    students.push_back({"Anna", 20, 4.8});
    students.push_back({"Ivan", 19, 4.5});
    std::cout << "\nStudents:\n";
    for (const auto &v : students)
        std::cout << v.name << ' ' << v.age << ' ' << v.grade << '\n';
    {
        MemoryResource temporary_resource(1024);
        (void)temporary_resource.allocate(512, 1);
        std::cout << "Without deallocate: allocations="
                  << temporary_resource.outstanding()
                  << ", free bytes=" << temporary_resource.free_bytes() << '\n';

    }
    std::cout << "MemoryResource destroyed; its entire memory block released\n";
}
