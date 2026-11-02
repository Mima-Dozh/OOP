#include "utils.h"

int Count_Of_Ones_In_Binary_Representation(int number) {
    auto count = 0;
    while (number > 0) {
        count += number & 1;
        number >>= 1;
    }
    return count;
}

int Count_Of_Ones_In_Binary_Representation_Of_Window(int from, int to) {
    auto count = 0;
    for (auto i = from; i <= to; i++) {
        count += Count_Of_Ones_In_Binary_Representation(i);
    }
    return count;
}
