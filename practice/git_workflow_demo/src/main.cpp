#include <iostream>

int add(int a, int b)
{
    return a + b;
}

int main()
{
    int x = 10;
    int y = 20;

    int result = add(x, y);

    std::cout << "x = " << x << '\n';
    std::cout << "y = " << y << '\n';
    std::cout << "result = " << result << '\n';

    return 0;
}
