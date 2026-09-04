#include <cstddef>
#include <iostream>

class Buffer {
private:
    int* data_;
    std::size_t size_;

public:
    explicit Buffer(std::size_t size)
        : data_(new int[size]{}), size_(size) {
        std::cout << "普通构造，内存地址："
                  << static_cast<void*>(data_) << '\n';
    }

    // 拷贝构造函数：实现深拷贝
    Buffer(const Buffer& other)
        : data_(new int[other.size_]{}),
          size_(other.size_) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }

        std::cout << "拷贝构造，原内存地址："
                  << static_cast<const void*>(other.data_)
                  << "，新内存地址："
                  << static_cast<void*>(data_) << '\n';
    }

    ~Buffer() {
        std::cout << "析构，释放内存："
                  << static_cast<void*>(data_) << '\n';

        delete[] data_;
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
    Buffer a(3);

    a.set(0, 10);
    a.set(1, 20);
    a.set(2, 30);

    // 调用拷贝构造函数
    Buffer b = a;

    std::cout << "a 的内存地址：" << a.address() << '\n';
    std::cout << "b 的内存地址：" << b.address() << '\n';

    std::cout << "修改前：a[0] = " << a.get(0)
              << "，b[0] = " << b.get(0) << '\n';

    a.set(0, 100);

    std::cout << "修改后：a[0] = " << a.get(0)
              << "，b[0] = " << b.get(0) << '\n';

    return 0;
}
