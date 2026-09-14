#include <iostream>
#include "add.hpp"

int main() {
    int result = add(10, 20);

    std::cout << "result = "
              << result
              << std::endl;

    return 0;
}
