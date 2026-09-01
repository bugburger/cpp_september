#include <iostream>

void passByValue(int x)
{
    x = 100;
}

void passByPointer(int* x)
{
    *x = 200;
}

void passByReference(int& x)
{
    x = 300;
}

int main()
{
    int a = 10;
    int b = 10;
    int c = 10;

    passByValue(a);
    passByPointer(&b);
    passByReference(c);

    std::cout << "a = " << a << '\n';
    std::cout << "b = " << b << '\n';
    std::cout << "c = " << c << '\n';

    return 0;
}
