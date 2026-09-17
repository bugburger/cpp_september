#include <iostream>
#include <limits>

int main() {
    int x = std::numeric_limits<int>::max();

    std::cout << "before overflow:" << '\n';
    std::cout << "x = " << x << '\n';

    // 故意制造 signed integer overflow
    x = x + 1;

    std::cout << "\nafter overflow:" << '\n';
    std::cout << "x = " << x << '\n';

    return 0;
}
