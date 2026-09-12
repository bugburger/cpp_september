# Day13 学习总结

## 今日学习内容

今天是 C++ 内存管理与移动语义阶段的综合练习，主要学习并实践了以下内容：

1. 浅拷贝与深拷贝的区别
2. 拷贝构造函数的作用和触发时机
3. 拷贝赋值运算符的作用和触发时机
4. 移动构造函数的实现
5. 移动赋值运算符的实现
6. Rule of Three、Rule of Five 和 Rule of Zero
7. `std::move` 的真实作用
8. 移动后源对象的安全状态
9. `noexcept` 对移动操作的重要性
10. `this`、`*this` 和自赋值判断
11. `unique_ptr`、`shared_ptr`、`weak_ptr` 所有权复习
12. 使用 AddressSanitizer 检查内存错误
13. C++ 编译错误的排查方法
14. LeetCode 349「两个数组的交集」
15. `unordered_set` 的插入、查找和去重
16. `find()`、`end()` 和迭代器
17. 将 `unordered_set` 转换为 `vector`

今天编写并测试了完整的 `Buffer` 资源管理类。程序成功验证了深拷贝、拷贝赋值、移动构造和移动赋值，AddressSanitizer 没有报告内存泄漏、重复释放或越界访问。

---

# 一、为什么资源管理类需要特殊处理

普通成员变量可以直接复制，例如：

```cpp
class Student {
private:
    int age_;
};
```

当复制 `Student` 对象时，直接复制 `age_` 的值通常没有问题。

但是，如果类中保存了一个裸指针，并且这个指针指向动态申请的内存：

```cpp
class Buffer {
private:
    int* data_;
    std::size_t size_;
};
```

情况就不一样了。

假设构造函数中申请了动态数组：

```cpp
data_ = new int[size]{};
```

析构函数中释放：

```cpp
delete[] data_;
```

这时 `Buffer` 对象不仅保存了一个指针，而且承担了管理这块内存的责任。

因此必须明确：

- 谁拥有这块内存
- 复制对象时是否复制内存
- 移动对象时如何转移资源
- 对象析构时由谁释放资源
- 如何避免两个对象释放同一块内存

这些问题正是 Rule of Three、Rule of Five 和 RAII 需要解决的问题。

---

# 二、默认拷贝为什么会产生浅拷贝

如果没有自己实现拷贝构造函数，编译器会生成一个默认拷贝构造函数。

默认拷贝会逐个复制成员变量。

例如：

```cpp
Buffer a(3);
Buffer b = a;
```

如果使用默认拷贝，实际效果类似于：

```cpp
b.data_ = a.data_;
b.size_ = a.size_;
```

这里复制的只是 `data_` 中保存的地址，而不是重新申请一块内存。

因此：

```cpp
a.data_ == b.data_
```

两个对象会指向同一块堆内存。

这就是浅拷贝。

## 浅拷贝的主要问题

假设 `a` 和 `b` 指向同一块内存：

```text
a.data_ ─────┐
             ├──> 同一块动态数组
b.data_ ─────┘
```

那么会出现以下问题：

### 1. 修改相互影响

如果执行：

```cpp
b.set(0, 100);
```

由于 `a` 和 `b` 指向同一块内存，`a` 中看到的数据也会发生变化。

### 2. 重复释放

当 `a` 析构时：

```cpp
delete[] a.data_;
```

这块内存已经被释放。

当 `b` 析构时，它还会再次执行：

```cpp
delete[] b.data_;
```

这就会对同一块内存释放两次，产生 double-free。

### 3. 悬空指针

当其中一个对象释放资源后，另一个对象中的指针仍然保存原来的地址，但该地址已经失效。

这种指针称为悬空指针。

继续访问它会导致未定义行为。

---

# 三、深拷贝

深拷贝的核心不是复制指针地址，而是：

1. 根据原对象的大小申请一块新内存
2. 将原对象的数据复制到新内存
3. 让两个对象分别管理自己的资源

示例：

```cpp
Buffer(const Buffer& other)
    : data_(new int[other.size_]{}),
      size_(other.size_) {

    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}
```

深拷贝完成后：

```cpp
a.data_ != b.data_
```

但是它们保存的数据内容相同。

```text
a.data_ ───> 动态数组 A：[10, 20, 30]

b.data_ ───> 动态数组 B：[10, 20, 30]
```

此时修改 `b` 不会影响 `a`。

两个对象析构时，也只会释放各自拥有的资源。

## 判断是否实现深拷贝

可以从两个方面判断：

### 地址不同

```cpp
std::cout << a.address() << '\n';
std::cout << b.address() << '\n';
```

如果两个对象的地址不同，说明它们没有共享同一块动态内存。

### 内容相同

```cpp
std::cout << a.get(0) << '\n';
std::cout << b.get(0) << '\n';
```

复制完成后，两者的数据内容应该相同。

今天运行 `buffer_complete.cpp` 时，拷贝对象和原对象的地址不同，但保存的数据相同，说明深拷贝实现正确。

---

# 四、Rule of Three

如果一个类需要自己管理资源，并且手动定义了以下三个函数中的一个，通常就需要考虑另外两个：

1. 析构函数
2. 拷贝构造函数
3. 拷贝赋值运算符

这就是 Rule of Three。

```cpp
class Buffer {
public:
    ~Buffer();

    Buffer(const Buffer& other);

    Buffer& operator=(const Buffer& other);
};
```

之所以需要同时考虑这三个函数，是因为它们共同决定了：

- 对象销毁时如何释放资源
- 创建副本时如何复制资源
- 已有对象之间赋值时如何处理资源

如果只写析构函数，却没有处理拷贝操作，默认浅拷贝仍然可能导致重复释放。

因此，并不是“只要写了析构函数就是深拷贝”，也不是“只要写了拷贝构造函数才算深拷贝”。

是否属于深拷贝，关键要看复制对象时有没有重新申请独立资源，并复制原来的数据。

---

# 五、析构函数

析构函数在对象生命周期结束时自动调用，用于释放对象管理的资源。

```cpp
~Buffer() {
    delete[] data_;
}
```

由于构造函数使用的是：

```cpp
new int[size]{}
```

因此析构函数必须对应使用：

```cpp
delete[] data_;
```

不能使用：

```cpp
delete data_;
```

`new[]` 必须和 `delete[]` 配对。

## 对 nullptr 执行 delete[] 是否安全

下面的操作是安全的：

```cpp
int* ptr = nullptr;
delete[] ptr;
```

因此，移动完成后将源对象的指针设置为 `nullptr`，源对象仍然可以正常进入析构函数。

---

# 六、拷贝构造函数

拷贝构造函数用于根据一个已有对象创建新对象。

函数形式：

```cpp
Buffer(const Buffer& other);
```

常见触发方式：

```cpp
Buffer a(3);

Buffer b(a);
```

或者：

```cpp
Buffer b = a;
```

这两种写法都会调用拷贝构造函数。

## 为什么参数必须使用引用

如果写成：

```cpp
Buffer(Buffer other);
```

为了把实参传给形参 `other`，程序需要先复制一次对象。

而复制对象又需要调用拷贝构造函数，于是会不断递归调用自己。

正确写法是：

```cpp
Buffer(const Buffer& other);
```

使用引用可以避免传参时再次复制对象。

使用 `const` 表示拷贝过程中不会修改原对象。

## 拷贝构造的完整逻辑

```cpp
Buffer(const Buffer& other)
    : data_(new int[other.size_]{}),
      size_(other.size_) {

    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}
```

执行过程为：

1. 读取 `other.size_`
2. 申请新的动态数组
3. 将数组元素逐个复制
4. 新对象获得独立资源

---

# 七、拷贝赋值运算符

拷贝赋值发生在两个对象都已经存在的情况下。

```cpp
Buffer a(3);
Buffer b(5);

b = a;
```

这里不会调用拷贝构造函数，而是调用拷贝赋值运算符：

```cpp
Buffer& operator=(const Buffer& other);
```

## 拷贝构造与拷贝赋值的区别

```cpp
Buffer b = a;
```

此时 `b` 正在创建，所以调用拷贝构造函数。

```cpp
Buffer b(5);
b = a;
```

执行赋值前 `b` 已经存在，所以调用拷贝赋值运算符。

可以记忆为：

- 创建新对象时复制：拷贝构造
- 已有对象之间赋值：拷贝赋值

## 为什么拷贝赋值更复杂

执行：

```cpp
b = a;
```

之前，`b` 可能已经拥有一块动态内存。

因此拷贝赋值不仅要复制 `a`，还要正确处理 `b` 原来的资源。

一种基本实现方式是：

```cpp
Buffer& operator=(const Buffer& other) {
    if (this == &other) {
        return *this;
    }

    int* newData = new int[other.size_]{};

    for (std::size_t i = 0; i < other.size_; ++i) {
        newData[i] = other.data_[i];
    }

    delete[] data_;

    data_ = newData;
    size_ = other.size_;

    return *this;
}
```

这里先申请和复制新资源，再释放旧资源。

这样可以避免在申请新内存失败时过早破坏当前对象的原有数据。

---

# 八、自赋值问题

下面的代码是合法的：

```cpp
a = a;
```

这称为自赋值。

如果拷贝赋值中先执行：

```cpp
delete[] data_;
```

然后再从 `other.data_` 中复制数据，就会出现问题。

因为此时：

```cpp
this == &other
```

`data_` 和 `other.data_` 实际上是同一个指针。

释放 `data_` 后，`other.data_` 也已经失效。

因此赋值运算符需要进行自赋值检查：

```cpp
if (this == &other) {
    return *this;
}
```

移动赋值同样应该考虑：

```cpp
a = std::move(a);
```

虽然实际程序中很少主动进行自移动赋值，但资源管理类仍然应该保证这种情况下不会破坏对象。

---

# 九、this 与 *this

在非静态成员函数中，编译器会隐式提供一个 `this` 指针。

`this` 指向调用当前成员函数的对象。

例如：

```cpp
Buffer& operator=(const Buffer& other) {
    return *this;
}
```

其中：

```cpp
this
```

表示当前对象的地址，类型类似于：

```cpp
Buffer*
```

而：

```cpp
*this
```

表示当前对象本身。

由于赋值运算符的返回类型是：

```cpp
Buffer&
```

所以最后需要：

```cpp
return *this;
```

## 为什么赋值运算符返回引用

返回当前对象的引用，可以支持连续赋值：

```cpp
a = b = c;
```

执行顺序为：

```cpp
b = c;
a = b;
```

因此赋值运算符通常返回 `*this`。

---

# 十、Rule of Five

C++11 引入移动语义后，在 Rule of Three 的基础上增加了两个特殊成员函数：

4. 移动构造函数
5. 移动赋值运算符

这五个函数合称 Rule of Five。

```cpp
class Buffer {
public:
    ~Buffer();

    Buffer(const Buffer& other);

    Buffer& operator=(const Buffer& other);

    Buffer(Buffer&& other) noexcept;

    Buffer& operator=(Buffer&& other) noexcept;
};
```

完整的资源管理类必须明确处理：

- 对象销毁
- 对象复制
- 对象赋值
- 资源移动
- 移动赋值

---

# 十一、移动构造函数

移动构造函数的目标不是复制资源，而是转移资源所有权。

函数形式：

```cpp
Buffer(Buffer&& other) noexcept;
```

示例：

```cpp
Buffer a(3);
Buffer b(std::move(a));
```

移动构造的基本实现：

```cpp
Buffer(Buffer&& other) noexcept
    : data_(other.data_),
      size_(other.size_) {

    other.data_ = nullptr;
    other.size_ = 0;
}
```

执行过程为：

1. 将 `other.data_` 交给新对象
2. 将 `other.size_` 交给新对象
3. 把源对象的指针设置为 `nullptr`
4. 把源对象的大小设置为 `0`

移动前：

```text
a.data_ ───> 动态数组
```

移动后：

```text
a.data_ = nullptr
b.data_ ───> 原来的动态数组
```

整个过程不需要重新申请数组，也不需要逐个复制元素。

因此，对于管理大量数据的对象，移动操作通常比深拷贝效率更高。

---

# 十二、使用 std::exchange 实现移动

还可以使用 `std::exchange` 完成“取走旧值，同时设置新值”的操作。

需要包含：

```cpp
#include <utility>
```

移动构造可以写成：

```cpp
Buffer(Buffer&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)),
      size_(std::exchange(other.size_, 0)) {
}
```

下面两种写法的作用相同：

```cpp
data_ = other.data_;
other.data_ = nullptr;
```

以及：

```cpp
data_ = std::exchange(other.data_, nullptr);
```

`std::exchange` 会：

1. 返回 `other.data_` 原来的值
2. 将 `other.data_` 设置为 `nullptr`

它可以让资源转移的意图更加清楚。

---

# 十三、移动赋值运算符

移动赋值发生在两个已经存在的对象之间。

```cpp
Buffer a(3);
Buffer b(5);

b = std::move(a);
```

移动赋值的函数形式：

```cpp
Buffer& operator=(Buffer&& other) noexcept;
```

基本实现：

```cpp
Buffer& operator=(Buffer&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;

    other.data_ = nullptr;
    other.size_ = 0;

    return *this;
}
```

移动赋值需要注意三个关键问题。

## 1. 先处理目标对象原来的资源

目标对象 `b` 可能已经拥有资源。

如果直接覆盖：

```cpp
data_ = other.data_;
```

那么 `b` 原来的地址就丢失了，原来的内存再也无法释放，造成内存泄漏。

所以要先执行：

```cpp
delete[] data_;
```

## 2. 转移源对象的资源

```cpp
data_ = other.data_;
size_ = other.size_;
```

此时目标对象接管源对象原来的资源。

## 3. 清空源对象

```cpp
other.data_ = nullptr;
other.size_ = 0;
```

如果不清空源对象，那么两个对象都会认为自己拥有同一块内存，析构时仍然会发生重复释放。

---

# 十四、std::move 的真实作用

`std::move` 这个名字容易让人误以为它会自动搬运数据。

实际上，`std::move` 本身：

- 不会复制数据
- 不会转移指针
- 不会释放资源
- 不会自动清空原对象

它的作用是进行类型转换，让一个左值可以匹配右值引用参数。

```cpp
Buffer b = std::move(a);
```

这里 `std::move(a)` 表示：

> `a` 的资源允许被转移，后面不再依赖它原来的内容。

真正执行资源转移的是：

```cpp
Buffer(Buffer&& other)
```

或者：

```cpp
Buffer& operator=(Buffer&& other)
```

## std::move 与 unique_ptr

`unique_ptr` 表示独占所有权，所以不能复制：

```cpp
std::unique_ptr<int> p1 =
    std::make_unique<int>(10);

std::unique_ptr<int> p2 = p1;  // 错误
```

但是可以移动：

```cpp
std::unique_ptr<int> p2 = std::move(p1);
```

移动后：

- `p2` 获得资源
- `p1` 变为空指针
- 资源仍然只有一个所有者

这和今天手动实现的 `Buffer` 移动构造思想一致。

---

# 十五、移动后的对象还能不能使用

对象被移动后仍然存在，也仍然会进入析构函数。

但是它原来的资源已经被转移。

对于今天实现的 `Buffer`，移动后主动设置为：

```cpp
other.data_ = nullptr;
other.size_ = 0;
```

因此移动后的源对象：

- 可以安全析构
- 可以重新赋值
- 可以调用专门支持空状态的成员函数
- 不应该继续依赖移动前的数据

移动后对象需要处于“有效但内容不再保证”的状态。

---

# 十六、noexcept

移动构造和移动赋值通常应该声明为：

```cpp
Buffer(Buffer&& other) noexcept;

Buffer& operator=(Buffer&& other) noexcept;
```

`noexcept` 表示函数承诺不会抛出异常。

## noexcept 为什么重要

`std::vector` 扩容时，需要把原有对象转移到新的存储空间。

如果移动构造带有 `noexcept`，容器可以放心使用移动操作。

如果移动构造可能抛出异常，为了保证异常安全，容器有时会使用拷贝构造。

因此：

```cpp
Buffer(Buffer&& other) noexcept
```

不仅是语法标记，也会影响标准容器是否愿意使用移动操作。

## 为什么今天的移动操作适合 noexcept

今天的移动构造主要执行：

- 复制指针
- 复制大小
- 将源指针设置为 `nullptr`
- 将源大小设置为 `0`

这些操作不需要重新申请内存，通常不会抛出异常，因此适合声明为 `noexcept`。

---

# 十七、Rule of Zero

今天手动实现 Rule of Five，是为了理解动态资源管理的底层原理。

但是在实际项目中，更推荐使用标准库中的 RAII 类型管理资源。

例如，可以将：

```cpp
int* data_;
std::size_t size_;
```

替换为：

```cpp
std::vector<int> data_;
```

此时：

```cpp
class Buffer {
private:
    std::vector<int> data_;
};
```

`std::vector` 已经正确实现了：

- 析构
- 深拷贝
- 拷贝赋值
- 移动构造
- 移动赋值

外层类通常不需要再自己编写这些特殊成员函数。

这就是 Rule of Zero。

## 三条规则的区别

### Rule of Three

适用于需要手动管理资源的传统 C++ 类：

- 析构函数
- 拷贝构造函数
- 拷贝赋值运算符

### Rule of Five

在 Rule of Three 基础上增加移动语义：

- 移动构造函数
- 移动赋值运算符

### Rule of Zero

优先使用 `vector`、`string`、智能指针等 RAII 类型，不直接管理裸资源，从而不需要手动实现上述函数。

实际开发中优先考虑 Rule of Zero。

---

# 十八、完整 Buffer 的核心结构

今天完成的 `Buffer` 资源管理类核心结构如下：

```cpp
class Buffer {
private:
    int* data_;
    std::size_t size_;

public:
    explicit Buffer(std::size_t size)
        : data_(new int[size]{}),
          size_(size) {
    }

    ~Buffer() {
        delete[] data_;
    }

    Buffer(const Buffer& other)
        : data_(new int[other.size_]{}),
          size_(other.size_) {

        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) {
            return *this;
        }

        int* newData = new int[other.size_]{};

        for (std::size_t i = 0;
             i < other.size_;
             ++i) {
            newData[i] = other.data_[i];
        }

        delete[] data_;

        data_ = newData;
        size_ = other.size_;

        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : data_(other.data_),
          size_(other.size_) {

        other.data_ = nullptr;
        other.size_ = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data_;

        data_ = other.data_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.size_ = 0;

        return *this;
    }
};
```

这个类体现了完整的 Rule of Five。

---

# 十九、Buffer 测试内容

今天不仅实现了特殊成员函数，还分别测试了它们是否会在正确的场景触发。

## 1. 普通构造

```cpp
Buffer a(3);
```

验证：

- 是否成功申请内存
- `size_` 是否为 `3`
- 数组是否完成初始化

## 2. 拷贝构造

```cpp
Buffer b = a;
```

验证：

- 是否调用拷贝构造函数
- `a` 和 `b` 的地址是否不同
- `a` 和 `b` 的数据是否相同
- 修改 `b` 后是否不会影响 `a`

## 3. 拷贝赋值

```cpp
Buffer c(2);
c = a;
```

验证：

- 是否释放 `c` 原来的资源
- 是否为 `c` 申请独立内存
- 是否正确复制 `a` 的数据
- 返回值是否为当前对象引用

## 4. 移动构造

```cpp
Buffer d = std::move(a);
```

验证：

- `d` 是否接管 `a` 原来的地址
- `a.data_` 是否变为 `nullptr`
- `a.size_` 是否变为 `0`

## 5. 移动赋值

```cpp
Buffer e(4);
e = std::move(b);
```

验证：

- `e` 原来的资源是否释放
- `e` 是否接管 `b` 的资源
- `b` 是否进入空状态
- 是否没有内存泄漏和重复释放

---

# 二十、Buffer 运行结果分析

今天运行 `buffer_complete` 后观察到：

- 构造对象时成功申请动态内存
- 拷贝构造后的两个对象地址不同
- 拷贝对象与原对象内容相同
- 移动构造后，目标对象接管原地址
- 移动后源对象地址显示为 `0`
- 移动后的源对象大小被设置为 `0`
- 所有对象最终都正常进入析构函数
- 每块动态内存只被释放一次
- AddressSanitizer 没有报告错误

这说明当前实现满足了资源管理类的基本要求：

1. 拷贝时使用深拷贝
2. 移动时转移所有权
3. 移动后源对象处于安全状态
4. 对象销毁时正确释放资源

---

# 二十一、智能指针所有权复习

今天还复习了前几天学习的三种智能指针。

## unique_ptr：独占所有权

```cpp
std::unique_ptr<int> ptr =
    std::make_unique<int>(10);
```

特点：

- 同一时间只能有一个所有者
- 不能复制
- 可以移动
- 离开作用域后自动释放资源

错误写法：

```cpp
std::unique_ptr<int> p2 = p1;
```

正确转移方式：

```cpp
std::unique_ptr<int> p2 = std::move(p1);
```

`unique_ptr` 不能复制的原因是：如果复制后两个指针都认为自己独占资源，就会破坏独占所有权，甚至可能导致重复释放。

## shared_ptr：共享所有权

```cpp
std::shared_ptr<int> p1 =
    std::make_shared<int>(10);

std::shared_ptr<int> p2 = p1;
```

特点：

- 多个 `shared_ptr` 可以共同拥有资源
- 使用引用计数记录所有者数量
- 最后一个所有者销毁时释放资源

## weak_ptr：非拥有型观察者

```cpp
std::weak_ptr<int> observer = p1;
```

特点：

- 不拥有资源
- 不增加引用计数
- 不能直接解引用
- 使用前需要通过 `lock()` 获得临时 `shared_ptr`
- 常用于解决 `shared_ptr` 循环引用

## 原始指针和引用

如果函数只需要临时访问对象，不需要管理其生命周期，可以使用：

```cpp
T*
```

或者：

```cpp
T&
```

它们不表示资源所有权。

选择指针类型时，首先应该判断：

> 这个变量是否拥有资源，以及资源是否允许共享。

---

# 二十二、AddressSanitizer

手动管理内存时，即使程序表面上能够正常运行，也可能存在隐藏问题。

今天使用 AddressSanitizer 和 UndefinedBehaviorSanitizer 进行检查：

```bash
g++ -std=c++17 \
    -Wall \
    -Wextra \
    -g \
    -fsanitize=address,undefined \
    buffer_complete.cpp \
    -o buffer_complete
```

运行：

```bash
./buffer_complete
```

AddressSanitizer 可以帮助发现：

- heap-buffer-overflow
- stack-buffer-overflow
- use-after-free
- double-free
- 部分内存泄漏
- 非法内存访问

UndefinedBehaviorSanitizer 可以帮助发现部分未定义行为。

今天程序运行结束后没有出现 Sanitizer 报错，说明当前测试范围内没有发现：

- 内存泄漏
- 重复释放
- 数组越界
- 释放后继续访问

不过，Sanitizer 没有报错只说明当前执行路径没有发现问题，仍然需要从代码逻辑上正确处理资源所有权。

---

# 二十三、编译错误排查方法

今天还遇到了由括号或类结构不完整引起的编译错误。

C++ 编译器出现一个语法错误后，后面可能产生很多连锁报错。

因此调试编译错误时，应该：

1. 优先看第一条错误
2. 检查错误所在行的上一行
3. 检查大括号是否匹配
4. 检查圆括号是否匹配
5. 检查类定义结尾是否有分号
6. 检查函数是否正确结束
7. 检查头文件是否完整
8. 修复第一处错误后重新编译

## 常见类结尾错误

错误：

```cpp
class Buffer {
};
```

正确的类定义结尾必须保留分号：

```cpp
class Buffer {
};
```

如果写成只有右大括号而没有分号：

```cpp
class Buffer {
}
```

编译器会报错。

## 常见括号问题

成员函数内部少一个右大括号，可能导致编译器把后面的函数错误理解为嵌套定义。

因此，看到很多函数同时报错时，不要立刻逐条修改，要先检查最前面的括号结构。

---

# 二十四、LeetCode 349：两个数组的交集

今天完成的算法题是 LeetCode 349「两个数组的交集」。

题目要求：

给定两个整数数组 `nums1` 和 `nums2`，返回它们的交集。结果中的每个元素必须是唯一的，返回顺序不限。

示例：

```text
输入：
nums1 = [1, 2, 2, 1]
nums2 = [2, 2]

输出：
[2]
```

另一个示例：

```text
输入：
nums1 = [4, 9, 5]
nums2 = [9, 4, 9, 8, 4]

输出：
[9, 4]
```

因为题目要求结果不能重复，所以适合使用集合。

---

# 二十五、unordered_set

`std::unordered_set` 是基于哈希表实现的集合容器。

需要包含头文件：

```cpp
#include <unordered_set>
```

主要特点：

- 元素不能重复
- 元素没有固定顺序
- 平均插入复杂度为 `O(1)`
- 平均查找复杂度为 `O(1)`
- 不支持通过下标访问
- 适合快速判断一个元素是否存在

声明方式：

```cpp
std::unordered_set<int> numsSet;
```

## insert()

向集合中插入元素：

```cpp
numsSet.insert(10);
```

如果再次插入相同元素：

```cpp
numsSet.insert(10);
```

集合中仍然只保存一个 `10`。

因此 `unordered_set` 天然具有去重功能。

---

# 二十六、今天算法代码中的第一个错误：push_back

最初写法：

```cpp
unordered_set<int> inter;

inter.push_back(nums1[i]);
```

这里的问题是 `unordered_set` 没有 `push_back()`。

`push_back()` 表示把元素放到容器尾部，主要用于：

- `vector`
- `deque`
- `list`

但是 `unordered_set` 没有固定顺序，也没有“尾部”的概念。

所以集合使用：

```cpp
inter.insert(nums1[i]);
```

结果集合也应该使用：

```cpp
resultSet.insert(nums2[j]);
```

不能写：

```cpp
resultSet.push_back(nums2[j]);
```

---

# 二十七、find() 返回什么

最初判断写成：

```cpp
if (inter.find(nums2[j])) {
}
```

这里的问题是 `find()` 返回的不是 `bool`，而是迭代器。

正确写法：

```cpp
if (inter.find(nums2[j]) != inter.end()) {
}
```

## 找到元素时

```cpp
auto it = inter.find(2);
```

如果集合中存在 `2`，`it` 会指向这个元素。

此时：

```cpp
it != inter.end()
```

## 没找到元素时

如果集合中不存在目标元素：

```cpp
auto it = inter.find(100);
```

返回：

```cpp
inter.end()
```

此时：

```cpp
it == inter.end()
```

因此判断元素是否存在的标准写法是：

```cpp
if (inter.find(value) != inter.end()) {
    // value 存在
}
```

---

# 二十八、什么是迭代器

迭代器是一个表示容器中某个位置的对象。

它的使用方式和指针有些相似。

例如：

```cpp
auto it = inter.begin();
```

`it` 指向集合中的一个元素。

可以使用：

```cpp
std::cout << *it;
```

访问它指向的元素。

可以使用：

```cpp
++it;
```

移动到下一个位置。

## begin()

```cpp
inter.begin()
```

表示容器中的第一个元素位置。

## end()

```cpp
inter.end()
```

表示最后一个元素之后的位置，也叫尾后位置。

`end()` 本身不指向有效元素，因此不能写：

```cpp
std::cout << *inter.end();
```

这会产生未定义行为。

## 为什么使用尾后位置

使用 `[begin, end)` 这种左闭右开区间，可以统一表示容器的有效范围：

```cpp
for (auto it = inter.begin();
     it != inter.end();
     ++it) {
    std::cout << *it << '\n';
}
```

只要迭代器等于 `end()`，就说明已经遍历完所有有效元素。

---

# 二十九、今天算法代码中的第三个错误：返回类型

函数返回类型为：

```cpp
vector<int>
```

但最初保存结果使用的是：

```cpp
unordered_set<int> resultSet;
```

因此不能直接：

```cpp
return resultSet;
```

因为 `unordered_set<int>` 和 `vector<int>` 是两个不同的类型。

正确方式是使用迭代器区间构造一个 `vector`：

```cpp
return vector<int>(
    resultSet.begin(),
    resultSet.end()
);
```

这里会将集合中的所有元素复制到一个新的 `vector<int>` 中。

因为 `unordered_set` 内部没有固定顺序，所以返回结果的顺序也不固定。

题目允许以任意顺序返回结果，因此不需要额外排序。

---

# 三十、LeetCode 349 完整解题逻辑

完整核心代码：

```cpp
class Solution {
public:
    vector<int> intersection(
        vector<int>& nums1,
        vector<int>& nums2) {

        unordered_set<int> inter;
        unordered_set<int> resultSet;

        for (int num : nums1) {
            inter.insert(num);
        }

        for (int num : nums2) {
            if (inter.find(num) != inter.end()) {
                resultSet.insert(num);
            }
        }

        return vector<int>(
            resultSet.begin(),
            resultSet.end()
        );
    }
};
```

执行过程：

1. 创建 `inter`，保存 `nums1` 的元素
2. 使用 `insert()` 自动删除 `nums1` 中的重复元素
3. 遍历 `nums2`
4. 使用 `find()` 查找当前元素是否在 `inter` 中
5. 如果存在，将它插入 `resultSet`
6. `resultSet` 自动保证结果没有重复元素
7. 将 `resultSet` 转换为 `vector<int>` 返回

---

# 三十一、算法复杂度分析

假设：

```text
nums1 长度为 n
nums2 长度为 m
```

将 `nums1` 插入哈希集合，平均需要：

```text
O(n)
```

遍历 `nums2` 并进行哈希查找，平均需要：

```text
O(m)
```

因此平均时间复杂度为：

```text
O(n + m)
```

空间中需要保存哈希集合和结果集合，所以空间复杂度为：

```text
O(n + m)
```

哈希表在极端哈希冲突情况下可能退化，但通常按照平均复杂度分析。

---

# 三十二、今天的主要易错点

## 1. 误以为编译器默认复制就是深拷贝

默认复制裸指针时只会复制地址，属于浅拷贝。

## 2. 认为写了析构函数就解决了资源管理问题

析构函数只解决释放问题，复制时仍然需要处理深拷贝。

## 3. 混淆拷贝构造和拷贝赋值

创建新对象时调用拷贝构造，已有对象之间赋值时调用拷贝赋值。

## 4. 拷贝赋值中忘记处理原资源

目标对象可能已经拥有资源，不能直接覆盖它的指针。

## 5. 忘记自赋值判断

需要判断：

```cpp
if (this == &other)
```

## 6. 误以为 std::move 会自动搬运数据

`std::move` 只负责转换表达式类型，真正的移动由移动构造或移动赋值完成。

## 7. 移动后没有清空源对象

需要将源对象设置为：

```cpp
other.data_ = nullptr;
other.size_ = 0;
```

## 8. 移动赋值前没有释放目标对象的旧资源

否则目标对象原来的内存会丢失，造成泄漏。

## 9. 忘记给移动操作添加 noexcept

对于不会抛出异常的资源转移操作，应该使用 `noexcept`。

## 10. 对 unordered_set 使用 push_back

集合使用：

```cpp
insert()
```

而不是：

```cpp
push_back()
```

## 11. 把 find() 返回值当成 bool

`find()` 返回迭代器，必须与 `end()` 比较。

## 12. 把 end() 当成最后一个元素

`end()` 是最后一个元素之后的位置，不能解引用。

## 13. 直接返回 unordered_set

函数要求返回 `vector<int>` 时，需要使用迭代器范围完成转换。

## 14. 被大量编译错误误导

语法错误可能产生连锁报错。应该先修复编译器输出的第一条错误，并检查它前面的括号和分号。

---

# 三十三、今日知识之间的联系

前几天学习的内容在今天的 `Buffer` 中连接了起来：

```text
new/delete
    ↓
资源所有权
    ↓
RAII
    ↓
深拷贝
    ↓
Rule of Three
    ↓
左值与右值
    ↓
std::move
    ↓
移动构造与移动赋值
    ↓
Rule of Five
    ↓
智能指针与 Rule of Zero
```

手写 `Buffer` 是为了理解这些底层机制。

实际项目中则应该尽量使用：

```cpp
std::vector
std::string
std::unique_ptr
std::shared_ptr
```

让标准库自动管理资源。

---

# 三十四、今日代码文件

```text
day13/
├── buffer_complete.cpp
├── intersection_arrays.cpp
└── day13.md
```

## buffer_complete.cpp

用于练习和验证：

- 动态数组的申请与释放
- 析构函数
- 深拷贝
- 拷贝构造
- 拷贝赋值
- 移动构造
- 移动赋值
- 自赋值判断
- `std::move`
- `noexcept`
- 移动后对象状态
- AddressSanitizer 检查

## intersection_arrays.cpp

用于练习：

- LeetCode 349
- `unordered_set`
- `insert()`
- `find()`
- `end()`
- 迭代器
- 集合去重
- 容器类型转换
- 时间和空间复杂度分析

## day13.md

记录今天实际学习、编写、排错和验证过的全部主要内容。

编译生成的以下文件不上传 GitHub：

```text
buffer_complete
intersection_arrays
```

GitHub 中只保存 `.cpp` 源码和 `.md` 学习总结。

---

# 今日总结

今天完成了 C++ 内存管理阶段的一次综合练习。

通过手写完整的 `Buffer` 类，进一步理解了对象复制和资源所有权之间的关系。默认拷贝裸指针只会复制地址，形成浅拷贝；为了让每个对象拥有独立资源，需要在拷贝构造和拷贝赋值中实现深拷贝。

在移动语义部分，理解了移动操作并不是复制资源，而是转移资源所有权。`std::move` 本身只进行类型转换，真正的资源转移由移动构造函数或移动赋值运算符完成。移动完成后，源对象仍然存在，因此必须将它设置为可以安全析构的空状态。

通过 Rule of Three、Rule of Five 和 Rule of Zero，可以根据类的资源管理方式决定是否需要手动实现特殊成员函数。学习底层原理时可以手写 Rule of Five，但实际开发中应该优先使用 `vector`、`string` 和智能指针，让类尽量遵循 Rule of Zero。

今天还通过 AddressSanitizer 验证了完整 `Buffer` 的内存安全。程序中拷贝后的对象地址不同、移动后的源对象地址为 `0`、析构过程正常，Sanitizer 没有报告内存泄漏或重复释放。

在 LeetCode 349 中，掌握了使用 `unordered_set` 完成快速查找和自动去重。今天实际纠正了三个关键错误：

1. `unordered_set` 应使用 `insert()`，不能使用 `push_back()`
2. `find()` 返回迭代器，必须与 `end()` 比较
3. `unordered_set` 不能直接作为 `vector` 返回，需要通过迭代器区间转换

今天最重要的理解是：

> 资源管理的核心不是简单地保存一个指针，而是明确资源由谁拥有、复制时如何创建独立资源、移动时如何转移所有权，以及对象销毁时由谁负责释放。
