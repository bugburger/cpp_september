#include <iostream>

int main()
{
    int a = 10;
    int b = 20;

    // 第一种：指向常量的指针
    const int* p1 = &a;

    std::cout << "Initially, *p1 = " << *p1 << '\n';

    p1 = &b;

    std::cout << "After p1 = &b, *p1 = " << *p1 << '\n';

    //*p1 = 100;  // 错误：不能通过 p1 修改变量

    // 第二种：常量指针
    int* const p2 = &a;

    *p2 = 100;

    std::cout << "After *p2 = 100, a = " << a << '\n';

    //p2 = &b;  // 错误：p2 不能改变指向

    // 第三种：常量引用
    const int& ref = a;

    std::cout << "Initially, ref = " << ref << '\n';

    a = 200;

    std::cout << "After a = 200, ref = " << ref << '\n';

    //ref = 300;  // 错误：不能通过常量引用修改变量

    return 0;
}
