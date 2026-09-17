#include <iostream>

int main() {
    int* p = new int(100);

    std::cout << "before delete:" << '\n';
    std::cout << "p address = " << p << '\n';
    std::cout << "*p = " << *p << '\n';

    delete p;

    std::cout << "\nafter delete:" << '\n';
    std::cout << "p address = " << p << '\n';

    // 故意访问已经释放的内存
    std::cout << "*p = " << *p << '\n';

    return 0;
}
