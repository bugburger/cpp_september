#include <iostream>
#include <memory>

class Resource {
private:
	int id_;

public:
	explicit Resource(int id) : id_(id) {
		std::cout << "Resource " << id_ << " created\n";
	}

	~Resource() {
		std::cout << "Resource " << id_ << " destroyed\n";
	}

	void use() const {
		std::cout << "Using Resource " << id_ << '\n';
	}
};

int main() {
	std::weak_ptr<Resource> observer;

	auto owner = std::make_shared<Resource>(1);

	std::cout << "1. owner count: " << owner.use_count() << '\n';

	observer = owner;

	std::cout << "2. after weak_ptr: "
                  << owner.use_count()
                  << '\n';

        std::cout << "3. expired: "
                  << std::boolalpha
                  << observer.expired()
                  << '\n';

	{
		auto temporary = observer.lock();

		if (temporary != nullptr) {
			temporary->use();
			std::cout << "4. inside block: "
				  << owner.use_count()
				  << '\n';
		}
	}
	std::cout << "5. after block: " << owner.use_count() << '\n';

	owner.reset();

	std::cout << "6. expired: " << observer.expired() << '\n';

	auto temporary = observer.lock();
	
	if (temporary == nullptr) {
		std::cout << "7.Resource no longer exists\n";
	}

	return 0;
}
