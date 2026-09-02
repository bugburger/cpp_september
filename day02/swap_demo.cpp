#include <iostream>

void swap_by_pointer(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swap_by_reference(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}

int main()
{
	int x = 10, y = 20;

	std::cout << "Before pointer swap:\n";
	std::cout << "x = " << x << ",y = " << y << '\n';

	swap_by_pointer(&x, &y);

	std::cout << "After pointer swap:\n";
        std::cout << "x = " << x << ", y = " << y << '\n';	

	x = 10, y = 20;

	std::cout << "Before reference swap:\n";
	std::cout << "x = " << x << ",y = " << y << '\n';

	swap_by_reference(x, y);
	
	std::cout << "After reference swap:\n";
	std::cout << "x = " << x << ",y = " << y << '\n';

	return 0;
}
