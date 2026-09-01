#include <iostream>

void changeValue(int& num)
{
	std::cout << "函数内部修改前 num = " << num << '\n';
	num = 100;
	std::cout << "函数内部修改后 num = " << num << '\n';
}

int main()
{
	int x = 10;

	std::cout << "修改前 x = " << x << '\n';
	std::cout << "x的地址 = " << &x << '\n';

	changeValue(x);

	std::cout << "修改后 x = " << x << '\n';

	return 0;
}
