#include <cstddef>
#include <iostream>

class Buffer {
private:
    int* data_;
    std::size_t size_;

public:
    explicit Buffer(std::size_t size)
        : data_(new int[size]{}), size_(size) {
        std::cout << "构造 Buffer，内存地址："
                  << static_cast<void*>(data_) << std::endl;
    }

    ~Buffer() {
        std::cout << "析构 Buffer，准备释放："
                  << static_cast<void*>(data_) << std::endl;

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

    // 没有自定义拷贝构造函数，
    // 编译器只会复制 data_ 中保存的地址。
    Buffer b = a;

    std::cout << "a 的内存地址：" << a.address() << '\n';
    std::cout << "b 的内存地址：" << b.address() << '\n';
    std::cout << "b[0] = " << b.get(0) << '\n';

    return 0;
}

