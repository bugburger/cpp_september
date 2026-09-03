#include <iostream>
#include <string>

class Student
{
public:
	Student(const std::string& name, int age) : name_(name), age_(age)
	{
		std::cout << "Constructor: " << name_ << '\n';
	}

	~Student()
	{
		std::cout << "Destructor: " << name_ << '\n';
	}

	void introduce()
	{
		std::cout << name_ << " is " << age_ << " years old.\n";
	}

private:
	std::string name_;
	int age_;
};

int main()
{
    std::cout << "===== main begins =====\n";

    Student alice("Alice", 24);
    alice.introduce();

    std::cout << "\n===== enter inner scope =====\n";

    {
        Student bob("Bob", 23);
        Student carol("Carol", 22);

        bob.introduce();
        carol.introduce();

        std::cout << "===== inner scope is ending =====\n";
    }

    std::cout << "===== inner scope ended =====\n";

    std::cout << "\n===== dynamic object =====\n";

    Student* david = new Student("David", 25);
    david->introduce();

    std::cout << "Deleting David\n";

    delete david;
    david = nullptr;

    std::cout << "\n===== main is about to end =====\n";

    return 0;
};



