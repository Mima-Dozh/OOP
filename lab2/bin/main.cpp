#include <iostream>
#include <string>

#include "lib/decimal.h"


int main() {
    std::string from, to;
    std::cin >> from >> to;
    std::cout << (Decimal(from) + Decimal(to)).to_string() << std::endl;
    return 0;
}
