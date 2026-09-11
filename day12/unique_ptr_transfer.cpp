#include <iostream>
#include <memory>
#include <utility>

using namespace std;

void takeOwnership(unique_ptr<int> ptr) {
	cout << "inside function: " << *ptr << endl;
}

int main()
{
	unique_ptr<int> p = make_unique<int>(42);

	takeOwnership(std::move(p));

	if (p == nullptr) {
		cout << "ownership transferred" << endl;
	}

	return 0;
}
