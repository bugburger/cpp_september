#include <iostream>
#include <memory>
#include <utility>

class Resource {
private:
	int id_;

public:
	explicit Resource(int id) : id_(id) {
		std::cout << "Resource " << id_
			  << " acquired\n";
	}

	~Resource() {
		std::cout << "Resource " << id_
			  << " released\n";
	}

	void use() const {
		std::cout << "Using resource "
			  << id_ << '\n';
	}
};

int main() {
	std::unique_ptr<Resource> p1 = 
		std::make_unique<Resource>(1);

	p1->use();

	std::unique_ptr<Resource> p2 = std::move(p1);

	std::cout << std::boolalpha;
	std::cout << "p1 is empty: "
		  << (p1 == nullptr) << '\n';

	std::cout << "p2 owns resource: "
		  << (p2 != nullptr) << '\n';

	if(p2) {
		p2->use();
	}

	p2.reset();

	std::cout << "p2 is empty after reset: "
		  << (p2 == nullptr) << '\n';

	return 0;
}

