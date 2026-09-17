#include <iostream>
#include <limits>

int main() {
    std::cout << "===== UBSan test =====" << '\n';

    int x = std::numeric_limits<int>::max();

    std::cout << "before overflow: "
              << x << '\n';

    // 故意制造 signed integer overflow
    x = x + 1;

    std::cout << "after overflow: "
              << x << '\n';


    std::cout << "\n===== ASan test =====" << '\n';

    int* data = new int[3];

    data[0] = 10;
    data[1] = 20;
    data[2] = 30;

    // 故意制造 heap-buffer-overflow
    data[3] = 40;

    delete[] data;

    return 0;
}
