#include <iostream>

int main() {
    int* p = new int(100);

    std::cout << "allocated address = " << p << '\n';
    std::cout << "value = " << *p << '\n';

    std::cout << "\nfirst delete..." << '\n';
    delete p;

    std::cout << "first delete finished" << '\n';

    // 故意重复释放同一块内存
    std::cout << "\nsecond delete..." << '\n';
    delete p;

    std::cout << "second delete finished" << '\n';

    return 0;
}
