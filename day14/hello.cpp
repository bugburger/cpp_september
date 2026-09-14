#include <iostream>

#define APP_NAME "Day14"

int add(int a, int b) {
    return a + b;
}

int main() {
    std::cout << APP_NAME << ": "
              << add(2, 3)
              << std::endl;

    return 0;
}
