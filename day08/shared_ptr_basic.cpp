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
		std::cout << "Using resource " << id_ << '\n';
	}
};

int main() {
	std::shared_ptr<Resource> p1 = std::make_shared<Resource>(1);
	
	std::cout << "After p1 created: " << p1.use_count() << '\n';

	{
		std::shared_ptr<Resource> p2 = p1;

		std::cout << "Inside block: " << p1.use_count() << '\n';

		p2->use();
	}

	std::cout << "After block: " << p1.use_count() << '\n';

	return 0;
}
