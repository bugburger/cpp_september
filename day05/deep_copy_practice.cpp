#include <iostream>

class NumberBox {
private:
	int* value_;

public:
	explicit NumberBox(int value) : value_(new int(value)){
	}

	NumberBox(const NumberBox& other) : value_(new int(*(other.value_))) {
	}

	~NumberBox() {
		delete value_;
	}

	void set(int value) {
		*value_ = value;
	}

	int get() const {
		return *value_;
	}

	const int* address() const {
		return value_;
	}
};

int main() {
    NumberBox a(10);

    // 使用a创建b，调用拷贝构造函数
    NumberBox b = a;

    // 修改a保存的数值
    a.set(99);

    std::cout << "a的数值：" << a.get() << '\n';
    std::cout << "b的数值：" << b.get() << '\n';

    std::cout << "a的内存地址：" << a.address() << '\n';
    std::cout << "b的内存地址：" << b.address() << '\n';

    return 0;
}
