#include <iostream>

#include "calculator.hpp"

int main() {
    int a = 20;
    int b = 10;

    std::cout << "add = "
              << add(a, b)
              << std::endl;

    std::cout << "subtract = "
              << subtract(a, b)
              << std::endl;

    return 0;
}
