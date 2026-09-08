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
};

int main() {
    std::shared_ptr<Resource> p1 =
        std::make_shared<Resource>(1);

    std::cout << "1. p1 count: "
              << p1.use_count()
              << '\n';

    std::shared_ptr<Resource> p2 = p1;

    std::cout << "2. p1 count: "
              << p1.use_count()
              << '\n';

    {
        std::shared_ptr<Resource> p3 = p2;

        std::cout << "3. inside block: "
                  << p1.use_count()
                  << '\n';
    }

    std::cout << "4. after block: "
              << p1.use_count()
              << '\n';

    p2.reset();

    std::cout << "5. after p2.reset(): "
              << p1.use_count()
              << '\n';

    p1.reset();

    std::cout << "6. p1 is empty: "
              << (p1 == nullptr)
              << '\n';

    return 0;
}
