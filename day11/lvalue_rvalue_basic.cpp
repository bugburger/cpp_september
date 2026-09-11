#include <iostream>

using namespace std;

int main()
{
	int a = 10;
	int& lref = a;
	int&& rref = 20;

	cout << "a = " << a << endl;
	cout << "lref = " << lref << endl;
	cout << "rref = " << rref << endl;

	lref = 30;
	rref = 40;

	cout << "after modify:" << endl;

	cout << "a = " << a << endl;
	cout << "lref = " << lref << endl;
	cout << "rref = " << rref << endl;

	return 0;
}
