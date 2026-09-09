#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Person {
private:
	std::string name_;
	std::shared_ptr<Person> partner_;

public:
	explicit Person(std::string name) : name_(std::move(name)) {
		std::cout << name_ << " craeted\n";
	}

	~Person() {
		std::cout << name_ << " destroyed\n";
	}

	void setPartner(const std::shared_ptr<Person>& partner) {
		partner_ = partner;
	}

	const std::string& getName() const {
		return name_;
	}
};

int main() {
	{
	auto alice = std::make_shared<Person>("Alice");

	auto bob = std::make_shared<Person>("Bob");

	alice->setPartner(bob);
	bob->setPartner(alice);

	std::cout << "Alice count: " << alice.use_count() << '\n';
	std::cout << "Bob count: " << bob.use_count() << '\n';
	}

	std::cout << "Local shared_ptr ended\n";

	return 0;
}
