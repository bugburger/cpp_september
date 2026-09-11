#include <iostream>
#include <utility>

using namespace std;

class Buffer
{
private:
	int* data_;
	int size_;

public:
	Buffer(int size) : data_(new int[size]), size_(size) {
		cout << "Constructor: size = " << size_ << endl;
	}

	Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
		for (int i = 0; i < size_; ++i) {
			data_[i] = other.data_[i];
		}
		cout << "Copy constructor" << endl;
	}

	Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
		other.data_ = nullptr;
		other.size_ = 0;
		cout << "Move constructor" << endl;
	}

	Buffer& operator = (Buffer&& other) noexcept {
		if (this != &other)
		{
			delete[] data_;

			data_ = other.data_;
			size_ = other.size_;

			other.data_ = nullptr;
			other.size_ = 0;
		}

		cout << "Move assignment" << endl;
		return *this;
	}

	~Buffer() {
		delete[] data_;
		cout << "Destructor: size = " << size_ << endl;
	}

	int size() const{
		return size_;
	}
};

int main()
{
    cout << "===== Move constructor =====" << endl;

    Buffer b1(5);

    Buffer b2 = std::move(b1);

    cout << "b1 size = "
         << b1.size() << endl;

    cout << "b2 size = "
         << b2.size() << endl;


    cout << "\n===== Move assignment =====" << endl;

    Buffer b3(10);
    Buffer b4(20);

    cout << "before move assignment:" << endl;

    cout << "b3 size = "
         << b3.size() << endl;

    cout << "b4 size = "
         << b4.size() << endl;

    b4 = std::move(b3);

    cout << "after move assignment:" << endl;

    cout << "b3 size = "
         << b3.size() << endl;

    cout << "b4 size = "
         << b4.size() << endl;

    return 0;
}
