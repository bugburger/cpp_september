#include <iostream>
#include <limits>

int main() {
    int signed_value = std::numeric_limits<int>::max();

    unsigned int unsigned_value =
        std::numeric_limits<unsigned int>::max();

    std::cout << "signed max   = "
              << signed_value << '\n';

    std::cout << "unsigned max = "
              << unsigned_value << '\n';

    std::cout << "\n===== signed overflow =====\n";

    signed_value = signed_value + 1;

    std::cout << "signed result = "
              << signed_value << '\n';

    std::cout << "\n===== unsigned wraparound =====\n";

    unsigned_value = unsigned_value + 1;

    std::cout << "unsigned result = "
              << unsigned_value << '\n';

    return 0;
}
