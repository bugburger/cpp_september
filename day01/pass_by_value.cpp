#include <iostream>

void changeValue(int num)
{
	num = 100;
	std::cout << "函数内部 num = " << num << '\n';
}

int main()
{
	int x = 10;
	std::cout << "修改前 x = " << x << '\n';
	changeValue(x);
	std::cout << "修改后 x = " << x << '\n';
	return 0;
}
