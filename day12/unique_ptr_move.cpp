#include <iostream>
#include <memory>
#include <utility>

using namespace std;

int main()
{
    unique_ptr<int> p1 = make_unique<int>(100);

    cout << "before move:" << endl;
    cout << "*p1 = " << *p1 << endl;
    cout << "p1 = " << p1.get() << endl;

    unique_ptr<int> p2 = std::move(p1);

    cout << "\nafter move:" << endl;

    cout << "p1 = " << p1.get() << endl;
    cout << "p2 = " << p2.get() << endl;

    if (p1 == nullptr)
    {
        cout << "p1 no longer owns the resource" << endl;
    }

    if (p2 != nullptr)
    {
        cout << "*p2 = " << *p2 << endl;
    }

    return 0;
}
