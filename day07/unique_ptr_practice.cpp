#include <iostream>
#include <memory>
#include <string>
#include <utility>

class User {
private:
	std::string name_;

public:
	explicit User(const std::string& name) : name_(name) {
		std::cout << "User " << name_ << " created\n";
	}

	~User() {
		std::cout << "User " << name_ << " destroyed\n";
	}

	void sayHello() const {
		std::cout << "Hello, I am " << name_ << '\n';
	}
};

int main() {
	std::unique_ptr<User> user1 = std::make_unique<User>("Wang");

	user1->sayHello();

	std::cout << "Before move:\n";
	std::cout << "user1 valid: " << static_cast<bool>(user1) << '\n';

	std::unique_ptr<User> user2 = std::move(user1);

	std::cout << "\nAfter moce:\n";

	std::cout << "user1 valid: " << static_cast<bool>(user1) << '\n';
	std::cout << "user2 valid: " << static_cast<bool>(user2) << '\n';

	if (user2) {
		user2->sayHello();
	}
	return 0;
}

