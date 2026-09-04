# Day 05：浅拷贝、深拷贝与螺旋矩阵

## 一、今日学习目标

今天主要学习 C++ 对象复制和动态内存管理的基础知识，并完成一道二维数组模拟题。

今日目标：

1. 理解浅拷贝和深拷贝的区别
2. 理解浅拷贝为什么可能导致重复释放
3. 掌握拷贝构造函数的基本写法
4. 理解指针地址与指针所指数据的区别
5. 使用 AddressSanitizer 检查内存问题
6. 完成 LeetCode 59「螺旋矩阵 II」

---

## 二、为什么要学习在 Linux 上写代码

Linux 上的 C++ 语法和 Windows 基本相同，区别主要在开发、编译、调试和运行环境。

在 Linux 中，可以直接使用：

```bash
g++
```

编译 C++ 程序，还可以使用：

```text
Git
CMake
GDB
AddressSanitizer
Valgrind
```

完成代码管理、工程构建和问题排查。

以后学习嵌入式 Linux、服务器程序、文件 IO、进程线程、Socket、V4L2 和模型部署时，都会经常使用 Linux。

因此，在 Linux 上写代码的目的不只是熟悉命令行，而是逐渐掌握企业中完整的 C++ 开发流程。

---

## 三、浅拷贝

当类中存在指针成员时，如果没有自己编写拷贝构造函数，编译器生成的默认拷贝通常只会复制指针中保存的地址。

例如：

```cpp
class Buffer {
private:
    int* data_;
};
```

执行：

```cpp
Buffer a(3);
Buffer b = a;
```

默认拷贝近似于：

```cpp
b.data_ = a.data_;
```

复制后，两个指针保存相同的地址：

```text
a.data_ ─┐
         ├──→ 同一块堆内存
b.data_ ─┘
```

这就是浅拷贝。

浅拷贝只复制了地址，没有真正复制堆内存中的数据。

### 浅拷贝的问题

如果通过 `a`修改数据，因为 `a`和 `b`指向同一块内存，所以 `b`看到的数据也会改变。

更严重的问题是，当两个对象离开作用域时，它们都会执行析构函数：

```cpp
delete[] data_;
```

于是同一块内存会被释放两次：

```text
b 析构：第一次释放内存
a 析构：再次释放相同内存
```

这会产生：

```text
double-free
```

即重复释放。

今天在 `shallow_copy_bug.cpp`中成功复现了这个问题。程序显示 `a`和 `b`的内存地址完全相同，并在析构时尝试释放同一个地址。

---

## 四、深拷贝

深拷贝不能只复制指针地址，而是要为新对象重新申请一块内存，然后复制原对象中的数据。

深拷贝后的结果是：

```text
a.data_ ──→ 内存1 → [10][20][30]

b.data_ ──→ 内存2 → [10][20][30]
```

两个对象中的数据相同，但是内存地址不同。

这样可以保证：

- 修改 `a`不会影响 `b`
- 修改 `b`不会影响 `a`
- `a`和 `b`可以分别释放自己的内存
- 不会产生重复释放

深拷贝的基本过程是：

```cpp
data_ = new int[other.size_];

for (std::size_t i = 0; i < other.size_; ++i) {
    data_[i] = other.data_[i];
}
```

第一步重新申请内存，第二步复制数组中的每个元素。

---

## 五、拷贝构造函数

拷贝构造函数用于根据一个已经存在的对象创建新对象。

基本形式是：

```cpp
ClassName(const ClassName& other);
```

例如：

```cpp
NumberBox a(10);
NumberBox b = a;
```

第二行使用 `a`创建 `b`，因此会调用 `NumberBox`的拷贝构造函数。

今天完成的拷贝构造函数是：

```cpp
NumberBox(const NumberBox& other)
    : value_(new int(*other.value_)) {
}
```

这行代码的执行过程是：

```cpp
other.value_
```

取得原对象指针中保存的地址。

```cpp
*other.value_
```

对指针解引用，取得原对象保存的整数。

```cpp
new int(*other.value_)
```

重新申请一块堆内存，并使用原对象的数值初始化这块内存。

最后：

```cpp
value_(new int(*other.value_))
```

让新对象的 `value_`指向新申请的内存。

### 为什么不能写成按值传递

错误写法：

```cpp
NumberBox(NumberBox other);
```

`other`是一个新的 `NumberBox`对象。为了创建参数 `other`，程序需要复制原对象，而复制原对象又要调用拷贝构造函数，因此会产生递归复制问题。

正确写法使用引用：

```cpp
NumberBox(const NumberBox& other);
```

其中：

- `&`表示引用，不会为了参数再创建一个对象
- `const`表示不能通过 `other`修改原对象

---

## 六、指针地址和指针所指数据

今天进一步区分了以下表达式：

```cpp
data_
```

表示指针中保存的地址。

```cpp
*data_
```

表示该地址中的第一个整数。

```cpp
data_[0]
```

同样表示数组中的第一个整数。

```cpp
&data_
```

表示指针变量 `data_`自身所在的地址。

可以总结为：

| 表达式 | 含义 |
|---|---|
| `data_` | 数组的首地址 |
| `*data_` | 数组的第一个元素 |
| `data_[0]` | 数组的第一个元素 |
| `&data_` | 指针变量自身的地址 |
| `other.value_` | 原对象保存的地址 |
| `*other.value_` | 原对象保存的数值 |

例如：

```cpp
std::cout << data_;
```

输出的是地址。

而：

```cpp
std::cout << *data_;
```

输出的是该地址中的整数。

因此，打印地址时不能用 `*data_`代替 `data_`。

---

## 七、`static_cast<void*>`的作用

在浅拷贝实验中使用了：

```cpp
std::cout << static_cast<void*>(data_) << '\n';
```

`static_cast<void*>(data_)`的作用是把 `data_`转换为通用指针，明确要求 `cout`按照内存地址的形式输出。

对于当前的 `int*`，也可以简单写成：

```cpp
std::cout << data_ << '\n';
```

二者都会打印地址。

但是：

```cpp
std::cout << *data_ << '\n';
```

打印的是数组中的第一个整数，而不是地址。

---

## 八、只读地址函数

今天还理解了下面的成员函数：

```cpp
const void* address() const {
    return data_;
}
```

它的作用是返回 `data_`保存的内存地址，方便在类外部查看两个对象的地址是否相同。

这里存在两个 `const`：

```cpp
const void* address() const
```

前面的：

```cpp
const void*
```

表示返回的地址只用于观察，不应该通过它修改数据。

后面的：

```cpp
address() const
```

表示这个成员函数不会修改当前对象。

对于当前的 `int*`，也可以写成：

```cpp
const int* address() const {
    return data_;
}
```

---

## 九、深拷贝练习

今天完成了 `NumberBox`深拷贝练习。

核心成员变量：

```cpp
int* value_;
```

普通构造函数：

```cpp
explicit NumberBox(int value)
    : value_(new int(value)) {
}
```

拷贝构造函数：

```cpp
NumberBox(const NumberBox& other)
    : value_(new int(*other.value_)) {
}
```

析构函数：

```cpp
~NumberBox() {
    delete value_;
}
```

测试过程：

```cpp
NumberBox a(10);
NumberBox b = a;

a.set(99);
```

正确结果是：

```text
a的数值：99
b的数值：10
```

同时，`a`和`b`的内存地址不同，说明两个对象拥有独立内存，深拷贝实现正确。

---

## 十、AddressSanitizer

今天编译代码时使用了：

```bash
g++ -std=c++17 \
    -Wall -Wextra -Wpedantic \
    -g \
    -fsanitize=address,undefined \
    deep_copy_practice.cpp \
    -o deep_copy_practice
```

其中：

```bash
-Wall -Wextra -Wpedantic
```

用于开启更多编译警告。

```bash
-g
```

用于保留调试信息。

```bash
-fsanitize=address,undefined
```

用于检查：

- 数组越界
- 重复释放
- 释放后继续访问
- 部分未定义行为

浅拷贝程序中的重复释放就是通过 AddressSanitizer 检查出来的。

---

## 十一、LeetCode 59：螺旋矩阵 II

### 题目要求

输入整数 `n`，生成一个 `n × n`矩阵，并按照顺时针方向填入从 `1`到 `n²`的数字。

当：

```text
n = 3
```

结果为：

```text
1 2 3
8 9 4
7 6 5
```

### 解题思路

使用四个变量表示当前还没有填写的区域：

```cpp
int top = 0;
int bottom = n - 1;
int left = 0;
int right = n - 1;
```

按照四个方向填写：

```text
从左向右
从上向下
从右向左
从下向上
```

每填写完一条边，再收缩对应边界：

```cpp
++top;
--right;
--bottom;
++left;
```

循环条件是：

```cpp
while (top <= bottom && left <= right)
```

只要上下边界和左右边界仍然有效，就继续填写下一圈。

### 今天代码中的主要问题

最开始将：

```cpp
++top;
--right;
--bottom;
++left;
```

写在了 `for`循环内部。

这样每填写一个元素，边界就会变化一次，导致程序没有沿着同一行或同一列填写，最终可能发生数组越界。

正确做法是先完成一整行或一整列，再移动边界。

另外，最开始使用了：

```cpp
i < right
```

这会漏掉右边界对应的元素，应该使用：

```cpp
i <= right
```

反向遍历时同理，应使用：

```cpp
column >= left
row >= top
```

### 复杂度

矩阵中的每个位置只填写一次，因此时间复杂度为：

```text
O(n²)
```

返回的矩阵需要保存 `n²`个整数，因此空间复杂度为：

```text
O(n²)
```

---

## 十二、今日遇到的错误

今天主要遇到了以下错误：

### 1. 使用中文标点

错误：

```cpp
public：
```

正确：

```cpp
public:
```

### 2. 析构函数使用中文波浪线

错误：

```cpp
～NumberBox()
```

正确：

```cpp
~NumberBox()
```

### 3. 拷贝构造函数括号不完整

正确写法：

```cpp
NumberBox(const NumberBox& other)
    : value_(new int(*other.value_)) {
}
```

### 4. `get()`返回类型错误

错误地将 `get()`写成了 `void`，导致：

```cpp
std::cout << b.get();
```

无法输出。

正确写法：

```cpp
int get() const {
    return *value_;
}
```

### 5. 编译失败后无法运行

当编译失败时，不会生成新的可执行文件，因此运行：

```bash
./deep_copy_practice
```

会提示：

```text
没有那个文件或目录
```

### 6. 大量连锁报错

一个中文符号或括号错误可能导致编译器无法理解后面的类结构，从而产生大量报错。

以后应该从编译器输出的第一条错误开始修改，而不是同时处理所有报错。

---

## 十三、今日总结

今天理解了对象复制与动态内存之间的关系。浅拷贝只复制指针地址，会导致多个对象指向同一块内存，可能出现数据相互影响和重复释放。深拷贝通过重新申请内存并复制数据，使不同对象分别管理自己的资源。

同时掌握了拷贝构造函数的基本形式：

```cpp
ClassName(const ClassName& other);
```

并完成了一个 `NumberBox`深拷贝程序。

算法部分完成了 LeetCode 59「螺旋矩阵 II」，理解了使用 `top、bottom、left、right`四条边界模拟顺时针填写矩阵的方法。

今天还进一步熟悉了 Linux 下的 `g++`编译、AddressSanitizer 内存检查以及根据第一条编译错误排查代码问题的方法。
