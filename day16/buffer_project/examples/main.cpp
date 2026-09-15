#include <iostream>
#include <utility>

#include "buffer.hpp"

int main() {
    std::cout << "===== create a ====="
              << std::endl;

    Buffer a(3);

    a.set(0, 10);
    a.set(1, 20);
    a.set(2, 30);

    std::cout << "a[0] = "
              << a.get(0)
              << std::endl;

    std::cout << "\n===== copy ====="
              << std::endl;

    Buffer b = a;

    std::cout << "b[1] = "
              << b.get(1)
              << std::endl;

    std::cout << "\n===== move ====="
              << std::endl;

    Buffer c = std::move(a);

    std::cout << "c[2] = "
              << c.get(2)
              << std::endl;

    std::cout << "a.size() after move = "
              << a.size()
              << std::endl;

    std::cout << "\n===== end ====="
              << std::endl;

    return 0;
}
