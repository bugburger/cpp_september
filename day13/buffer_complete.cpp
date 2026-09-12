#include <iostream>
#include <cstddef>
#include <utility>

class Buffer {
private:
	int* data_;
	std::size_t size_;

public:
	explicit Buffer(std::size_t size) : data_(new int[size]{}), size_(size) {
		std::cout << "Constructor, size = " << size_ 
			  << ", address = " << static_cast<void*>(data_) << '\n';

	}

	~Buffer() {
		std::cout << "Destructor, address = " << static_cast<void*>(data_) << '\n';
		delete[] data_;
	}

	Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
		for (std::size_t i = 0; i < size_; ++i) {
			data_[i] = other.data_[i];
		}
		std::cout << "Copy constructor\n";
	}

	Buffer& operator = (const Buffer& other) {
		if (this != &other) {
			delete[] data_;
			size_ = other.size_;
			data_ = new int[size_];
			for (std::size_t i = 0; i < size_; ++i) {
				data_[i] = other.data_[i];
			}
		}
		return *this;
	}

	Buffer(Buffer&& other) noexcept
		:data_(other.data_),
		 size_(other.size_) {

		other.data_ = nullptr;
		other.size_ = 0;

		std::cout << "Move constructor\n";
	}

	Buffer& operator = (Buffer&& other) noexcept {
		if (this != &other) {
			delete[] data_;
			
			data_ = other.data_;
			size_ = other.size_;

			other.data_ = nullptr;
			other.size_ = 0;
		}
		return *this;
	}

	void set(std::size_t index, int value) {
		if (index < size_) {
			data_[index] = value;
		}
	}

	int get(std::size_t index) const {
		if (index < size_) {
			return data_[index];
		}
		return 0;
	}

	const void* address() const {
		return data_;
	}
};

int main() {
    std::cout << "===== create a =====\n";

    Buffer a(3);
    a.set(0, 10);
    a.set(1, 20);
    a.set(2, 30);

    std::cout << "\n===== copy constructor =====\n";

    Buffer b(a);

    std::cout << "a address: "
              << a.address()
              << '\n';

    std::cout << "b address: "
              << b.address()
              << '\n';

    std::cout << "a[0] = "
              << a.get(0)
              << '\n';

    std::cout << "b[0] = "
              << b.get(0)
              << '\n';


    std::cout << "\n===== copy assignment =====\n";

    Buffer c(5);

    c = a;

    std::cout << "a address: "
              << a.address()
              << '\n';

    std::cout << "c address: "
              << c.address()
              << '\n';


    std::cout << "\n===== move constructor =====\n";

    Buffer d(std::move(a));

    std::cout << "a address after move: "
              << a.address()
              << '\n';

    std::cout << "d address: "
              << d.address()
              << '\n';


    std::cout << "\n===== move assignment =====\n";

    Buffer e(10);

    e = std::move(b);

    std::cout << "b address after move: "
              << b.address()
              << '\n';

    std::cout << "e address: "
              << e.address()
              << '\n';


    std::cout << "\n===== end =====\n";

    return 0;
}






