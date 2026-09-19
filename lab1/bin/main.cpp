#include <iostream>
#include "lib/utils.h"

int main() {
    int from, to;
    std::cin >> from >> to;
    std::cout << Count_Of_Ones_In_Binary_Representation_Of_Window(from, to) << std::endl;
    return 0;
}
