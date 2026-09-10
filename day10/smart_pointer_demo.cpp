#include <iostream>
#include <memory>
#include <string>

using namespace std;

class Resource {
private:
	string name;

public:
	Resource(const string& n) : name(n) {
		cout << "Resource " << name << " created" << endl;
	}

	~Resource() {
		cout << "Resource " << name << " destroyed" << endl;
	}

	void use() const {
		cout << "Using Resource " << name << endl;
	}
};

int main() {
	cout << "===== 1. unique_ptr =====" << endl;

	{
		unique_ptr<Resource> p1 = make_unique<Resource>("Unique");

		p1->use();

		cout << "Before move:" << endl;
		cout << "p1: " << (p1 ? "valid" : "null") << endl;

		unique_ptr<Resource> p2 = move(p1);

		cout << "After move:" << endl;
       	        cout << "p1: " << (p1 ? "valid" : "null") << endl;
       	        cout << "p2: " << (p2 ? "valid" : "null") << endl;

		p2->use();
	}

	cout << endl;

	cout << "===== 2. shared_ptr =====" << endl;

	{
		shared_ptr<Resource> p1 = make_shared<Resource>("Shared");

		cout << "owner count: " << p1.use_count() << endl;

		{
			shared_ptr<Resource> p2 = p1;

			cout << "After creating p2:" << endl;
			cout << "owner count: " << p1.use_count() << endl;

			p2->use();
		}

		cout << "After p2 destroyed:" << endl;
		cout << "owner count: " << p1.use_count() << endl;
	}

	cout << endl;

	cout << "===== 3. weak_ptr =====" << endl;

	weak_ptr<Resource> observer;

	{
		shared_ptr<Resource> owner = make_shared<Resource>("Observed");

		observer = owner;

		cout << "owner count: " << owner.use_count() << endl;

		if (shared_ptr<Resource> temp = observer.lock()) {
			temp->use();
		}
	}

	 cout << "Owner destroyed" << endl;
	if (observer.expired()) {
		cout << "Resource no longer exists" << endl;
	}

	return 0;
}
