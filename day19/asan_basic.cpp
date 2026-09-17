#include <iostream>

int main() {
    int* data = new int[3];

    data[0] = 10;
    data[1] = 20;
    data[2] = 30;

    std::cout << "data[0] = " << data[0] << '\n';
    std::cout << "data[1] = " << data[1] << '\n';
    std::cout << "data[2] = " << data[2] << '\n';

    // 故意制造数组越界
    data[3] = 40;

    std::cout << "data[3] = " << data[3] << '\n';

    delete[] data;

    return 0;
}
