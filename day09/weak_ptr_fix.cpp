#include <iostream>
#include <memory>
#include <string>

class Person {
private:
    std::string name_;
    std::weak_ptr<Person> partner_;

public:
    explicit Person(const std::string& name)
        : name_(name) {
        std::cout << name_
                  << " created\n";
    }

    ~Person() {
        std::cout << name_
                  << " destroyed\n";
    }

    void setPartner(
        const std::shared_ptr<Person>& partner) {

        partner_ = partner;
    }

    void showPartner() const {
	    std::shared_ptr<Person> partner = partner_.lock();
	    if (partner != nullptr) {
		    std::cout << name_ 
			      << "'s partner is " 
			      << partner->name_ 
			      << '\n';
	    } else {
		    std::cout << name_ << "'s partner no longer exists\n";
	    }
    }
};

int main() {
    std::weak_ptr<Person> aliceObserver;

    {
        auto alice =
            std::make_shared<Person>("Alice");

        auto bob =
            std::make_shared<Person>("Bob");

        aliceObserver = alice;

        alice->setPartner(bob);
        bob->setPartner(alice);

        std::cout << "Alice count: "
                  << alice.use_count()
                  << '\n';

        std::cout << "Bob count: "
                  << bob.use_count()
                  << '\n';

        alice->showPartner();
        bob->showPartner();

        std::cout << "Alice expired: "
                  << std::boolalpha
                  << aliceObserver.expired()
                  << '\n';
    }

    std::cout << "Local shared_ptr ended\n";

    std::cout << "Alice expired: "
              << std::boolalpha
              << aliceObserver.expired()
              << '\n';

    return 0;
}
