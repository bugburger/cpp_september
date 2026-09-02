#include <iostream>

int main()
{
    int numbers[5] = {10, 20, 30, 40, 50};

    int* p = numbers;

    std::cout << "numbers[0] = " << numbers[0] << '\n';
    std::cout << "*p = " << *p << '\n';

    std::cout << "numbers[2] = " << numbers[2] << '\n';
    std::cout << "*(p + 2) = " << *(p + 2) << '\n';

    *(p + 2) = 300;

    std::cout << "After modification:\n";
    std::cout << "numbers[2] = " << numbers[2] << '\n';

    std::cout << "numbers address = " << numbers << '\n';
    std::cout << "&numbers[0] = " << &numbers[0] << '\n';
    std::cout << "p = " << p << '\n';

    std::cout << "sizeof(numbers) = "
              << sizeof(numbers) << '\n';

    std::cout << "sizeof(p) = "
              << sizeof(p) << '\n';

    return 0;
}
