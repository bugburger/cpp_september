# Day15 学习总结

## 今日学习内容

今天开始正式接触 C++ 工程化构建工具 CMake。

在 Day14 已经理解：

```text
预处理
编译
汇编
链接
```

的基础上，今天进一步学习如何使用 CMake 管理这些构建过程。

主要学习了：

* CMake 的作用
* `CMakeLists.txt`
* `cmake_minimum_required`
* `project`
* `add_executable`
* target 的概念
* C++17 标准配置
* `cmake ..`
* `cmake --build .`
* out-of-source build
* 增量构建
* 多个 `.cpp` 文件的构建
* CMake 与链接错误之间的关系
* Buffer 类工程拆分
* 阅读编译日志的方法
* LeetCode 454「四数相加 II」
* `unordered_map` 统计出现次数

---

# 一、为什么需要 CMake

简单程序可以直接：

```bash
g++ main.cpp -o app
```

多个源文件也可以：

```bash
g++ main.cpp buffer.cpp -o app
```

但项目越来越大以后，可能出现：

```text
main.cpp
buffer.cpp
logger.cpp
config.cpp
network.cpp
process.cpp
...
```

如果全部手写 `g++`：

```text
命令越来越长
容易漏掉源文件
编译参数不好统一
库不好管理
不同平台维护困难
工程结构越来越乱
```

因此需要构建系统。

CMake 就是目前 C/C++ 工程中非常常见的构建配置工具。

---

# 二、CMake 和 g++ 的关系

需要特别注意：

> CMake 本身并不是 C++ 编译器。

可以理解为：

```text
CMakeLists.txt
       ↓
     CMake
       ↓
生成构建规则
       ↓
底层构建工具
       ↓
调用 g++
       ↓
编译 / 链接
       ↓
最终程序
```

因此：

```text
g++
=
真正负责 C++ 编译和链接

CMake
=
描述、配置和组织工程应该怎样构建
```

---

# 三、最简单的 CMake 工程

工程：

```text
day15/
├── CMakeLists.txt
└── main.cpp
```

最简单的：

```cmake
cmake_minimum_required(VERSION 3.10)

project(day15_cmake)

add_executable(day15_app main.cpp)
```

---

# 四、`cmake_minimum_required`

```cmake
cmake_minimum_required(VERSION 3.10)
```

表示：

```text
这个项目要求 CMake 最低版本为 3.10
```

可以查看自己的 CMake：

```bash
cmake --version
```

只要本机版本：

```text
>= 3.10
```

就满足要求。

---

# 五、`project`

```cmake
project(day15_cmake)
```

用于声明：

```text
当前 CMake 项目
```

其中：

```text
day15_cmake
```

就是项目名称。

---

# 六、`add_executable`

例如：

```cmake
add_executable(
    buffer_demo
    main.cpp
    buffer.cpp
)
```

表示：

```text
创建一个可执行程序
名称：buffer_demo

参与构建的源文件：
main.cpp
buffer.cpp
```

最终会得到：

```text
buffer_demo
```

---

# 七、target 是什么

这里的：

```text
buffer_demo
```

称为：

```text
target
构建目标
```

当前 target 是：

```text
可执行文件
```

以后还可能遇到：

```text
静态库 target
动态库 target
测试 target
```

因此以后看到：

```cmake
target_include_directories(...)
target_link_libraries(...)
target_compile_options(...)
```

这些命令中的 `target`，就是在配置某个具体构建目标。

---

# 八、`cmake ..`

典型做法：

```bash
mkdir -p build
cd build

cmake ..
```

这里：

```text
..
```

表示：

```text
上一级目录
```

CMake 会读取：

```text
../CMakeLists.txt
```

然后：

```text
解析 CMakeLists
↓
检查环境
↓
生成构建规则
```

通常可以看到：

```text
Configuring done
Generating done
```

可以简单记：

```text
cmake ..
=
告诉构建系统“应该怎么编”
```

---

# 九、`cmake --build .`

配置完成以后：

```bash
cmake --build .
```

这时才真正开始：

```text
.cpp
↓
编译
↓
.o
↓
链接
↓
target
```

因此：

```text
cmake ..
=
配置

cmake --build .
=
真正构建
```

这两个不要混淆。

---

# 十、out-of-source build

推荐结构：

```text
project/
├── CMakeLists.txt
├── main.cpp
├── buffer.cpp
├── buffer.hpp
└── build/
```

然后：

```bash
cd build
cmake ..
cmake --build .
```

这种方式叫：

```text
Out-of-source Build
```

也就是：

```text
源代码
和
构建生成文件
分离
```

优点：

```text
源码目录干净
生成文件集中
删除 build 即可重新构建
方便 Git 管理
大型工程更规范
```

---

# 十一、指定 C++17

可以写：

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

其中：

```cmake
set(CMAKE_CXX_STANDARD 17)
```

类似于：

```bash
g++ -std=c++17
```

而：

```cmake
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

表示：

```text
要求项目必须使用指定的 C++ 标准
```

---

# 十二、Buffer CMake 工程

今天把之前学习的 Buffer 类整理成了一个更加工程化的结构：

```text
buffer_cmake/
├── CMakeLists.txt
├── buffer.hpp
├── buffer.cpp
├── main.cpp
└── build/
```

这种结构比把所有代码写在：

```text
main.cpp
```

中更加符合真实 C++ 工程的组织方式。

---

# 十三、`buffer.hpp`

头文件主要负责：

```text
类声明
成员函数声明
接口定义
```

例如：

```cpp
class Buffer {
private:
    int* data_;
    std::size_t size_;

public:
    explicit Buffer(std::size_t size);

    ~Buffer();

    Buffer(const Buffer& other);

    Buffer& operator=(const Buffer& other);

    Buffer(Buffer&& other) noexcept;

    Buffer& operator=(Buffer&& other) noexcept;

    void set(std::size_t index, int value);

    int get(std::size_t index) const;

    std::size_t size() const;
};
```

这里也复习到了之前学过的：

```text
构造函数
析构函数
拷贝构造
拷贝赋值
移动构造
移动赋值
Rule of Five
```

---

# 十四、`buffer.cpp`

真正实现写在：

```cpp
buffer.cpp
```

例如：

```cpp
Buffer::Buffer(...)
```

```cpp
Buffer::~Buffer()
```

```cpp
Buffer::Buffer(const Buffer& other)
```

```cpp
Buffer& Buffer::operator=(...)
```

```cpp
void Buffer::set(...)
```

```cpp
int Buffer::get(...) const
```

其中：

```cpp
Buffer::
```

表示：

```text
后面的函数属于 Buffer 类作用域
```

---

# 十五、CMake 如何处理多个 cpp

例如：

```cmake
add_executable(
    buffer_demo
    main.cpp
    buffer.cpp
)
```

CMake 大致会组织成：

```text
main.cpp
↓
main.cpp.o

buffer.cpp
↓
buffer.cpp.o
```

然后：

```text
main.cpp.o
+
buffer.cpp.o
↓
链接
↓
buffer_demo
```

这正是 Day14 手工练习：

```bash
g++ -c main.cpp
g++ -c buffer.cpp
g++ main.o buffer.o
```

的工程化版本。

---

# 十六、增量构建 Incremental Build

第一次构建：

```text
Building main.cpp.o
Building buffer.cpp.o
Linking buffer_demo
```

如果后来只修改：

```text
buffer.cpp
```

再执行：

```bash
cmake --build .
```

可能只会看到：

```text
Building buffer.cpp.o
Linking buffer_demo
```

而：

```text
main.cpp.o
```

不会重新编译。

因为：

```text
main.cpp 没有变化
↓
原来的 main.cpp.o 可以继续使用
```

而：

```text
buffer.cpp 发生变化
↓
重新产生 buffer.cpp.o
↓
重新链接
```

这就是：

```text
Incremental Build
增量构建
```

对于大型 C++ 项目非常重要。

---

# 十七、修改 cpp 后是否需要重新 `cmake ..`

如果只是修改：

```text
.cpp
.hpp
```

通常：

```bash
cmake --build .
```

即可。

如果修改的是：

```text
CMakeLists.txt
```

或者改变构建配置，则通常需要重新：

```bash
cmake ..
cmake --build .
```

---

# 十八、CMake 与链接错误

这是 Day14 与 Day15 最重要的联系之一。

假设错误写成：

```cmake
add_executable(
    buffer_demo
    main.cpp
)
```

漏掉：

```text
buffer.cpp
```

但是 `main.cpp` 中：

```cpp
#include "buffer.hpp"
```

编译器仍然知道：

```text
Buffer 类存在哪些函数
```

因此：

```text
main.cpp
↓
main.cpp.o
```

可能成功。

但是 Buffer 的真正实现位于：

```text
buffer.cpp
```

因为它没有加入 target：

```text
buffer.cpp 没有编译
↓
buffer.cpp.o 不存在
↓
Buffer 的定义没有参与链接
```

最终会出现：

```text
undefined reference to `Buffer::Buffer(...)'
undefined reference to `Buffer::set(...)'
undefined reference to `Buffer::get(...)'
undefined reference to `Buffer::~Buffer()'
```

因此以后如果：

```text
声明明明存在
但是出现 undefined reference
```

要检查：

```text
对应的 .cpp 有没有加入 CMake target
```

---

# 十九、今天遇到的实际错误

## 1. 自定义头文件写错

错误：

```cpp
#include <buffer.hpp>
```

导致：

```text
fatal error: buffer.hpp: 没有那个文件或目录
```

当前工程正确：

```cpp
#include "buffer.hpp"
```

---

## 2. 成员变量与成员函数混淆

类中同时存在：

```cpp
std::size_t size_;
```

以及：

```cpp
std::size_t size() const;
```

曾经误写：

```cpp
size_(other.size)
```

这里：

```cpp
other.size
```

指向的是：

```text
成员函数
```

而真正需要拷贝的是：

```cpp
other.size_
```

所以正确：

```cpp
size_(other.size_)
```

这也说明：

```text
size
和
size_
```

虽然名字接近，但完全是两个成员。

---

## 3. 成员访问符写错

错误：

```cpp
data_[i] = other,data_[i];
```

这里错误使用：

```text
,
```

正确：

```cpp
data_[i] = other.data_[i];
```

对象访问成员要使用：

```cpp
.
```

---

## 4. 类型名拼写错误

错误：

```cpp
Buffr&& other
```

正确：

```cpp
Buffer&& other
```

这种非常小的拼写错误可能引起：

```text
连续很多条编译错误
```

因此不能看到十几条报错就认为程序有十几个独立问题。

---

# 二十、阅读编译日志的正确习惯

构建失败以后，终端可能出现：

```text
fatal error
compiler error
gmake error
target failed
程序不存在
```

真正应该首先寻找：

> 最早出现的有效错误。

例如：

```text
fatal error: buffer.hpp: No such file or directory
```

这是根因。

后面的：

```text
gmake: *** Error
```

或者：

```text
./buffer_demo: No such file or directory
```

通常只是：

```text
前面失败以后造成的连锁反应
```

因此以后调试：

```text
不要从最后一条错误开始猜
↓
从第一个真正的编译错误开始解决
```

---

# 二十一、LeetCode 454：四数相加 II

题目需要统计：

```text
nums1[i]
+
nums2[j]
+
nums3[k]
+
nums4[l]
=
0
```

的四元组数量。

如果直接使用四层循环：

```text
O(n⁴)
```

效率比较低。

---

# 二十二、核心拆分思想

原式：

```text
A + B + C + D = 0
```

可以转换成：

```text
A + B = -(C + D)
```

因此把四个数组分成两组：

```text
nums1 + nums2
```

和：

```text
nums3 + nums4
```

这样就可以利用哈希表把：

```text
O(n⁴)
```

降低到：

```text
O(n²)
```

---

# 二十三、这次 unordered_map 存什么

定义：

```cpp
unordered_map<int, int> mp;
```

但是和 Two Sum 不一样。

Day14 Two Sum：

```text
数字 → 下标
```

Day15 Four Sum Count：

```text
两数之和 → 这个和出现的次数
```

例如：

```text
-1 → 1
 0 → 2
 1 → 1
```

其中：

```text
0 → 2
```

表示：

```text
nums1 + nums2 中
一共有两种组合的和等于 0
```

---

# 二十四、统计前两个数组的两数和

```cpp
for (int i = 0; i < nums1.size(); ++i) {
    for (int j = 0; j < nums2.size(); ++j) {
        int sum = nums1[i] + nums2[j];
        mp[sum]++;
    }
}
```

其中：

```cpp
mp[sum]++;
```

表示：

```text
sum 每出现一次
对应次数 +1
```

如果：

```text
sum
```

之前不存在，使用：

```cpp
mp[sum]
```

时会先创建，并进行默认初始化。

对于：

```cpp
int
```

默认值为：

```text
0
```

然后再执行：

```text
++
```

---

# 二十五、查找相反数

遍历：

```cpp
nums3[k] + nums4[l]
```

需要寻找：

```cpp
-(nums3[k] + nums4[l])
```

因此：

```cpp
int tar = -(nums3[k] + nums4[l]);
```

然后：

```cpp
if (mp.find(tar) != mp.end()) {
    count += mp[tar];
}
```

---

# 二十六、为什么不能只写 `count++`

这是这道题最重要的地方。

假设：

```text
mp[0] = 2
```

表示前两个数组存在：

```text
两组不同的 (i, j)
```

使得：

```text
nums1[i] + nums2[j] = 0
```

现在某个：

```text
nums3[k] + nums4[l] = 0
```

那么当前这一组：

```text
(k, l)
```

可以分别和前面的两组：

```text
(i, j)
```

组合。

因此会产生：

```text
2 个合法四元组
```

所以必须：

```cpp
count += mp[tar];
```

不能：

```cpp
count++;
```

---

# 二十七、为什么不能 `mp[tar]--`

曾经容易产生这样的想法：

```cpp
if (mp.find(tar) != mp.end()) {
    mp[tar]--;
    count++;
}
```

这是错误的。

因为：

```text
前面的一组 (A, B)
```

并不是：

```text
使用一次之后就不能再使用
```

同一个：

```text
(A, B)
```

可以与不同的：

```text
(C, D)
```

分别组成不同的合法四元组。

因此：

```text
mp
```

保存的是：

```text
组合数量统计
```

而不是：

```text
可消耗库存
```

所以不能减。

---

# 二十八、完整核心代码

```cpp
unordered_map<int, int> mp;
int count = 0;

// 统计 nums1 + nums2 的所有和及出现次数
for (int i = 0; i < nums1.size(); ++i) {
    for (int j = 0; j < nums2.size(); ++j) {
        int sum = nums1[i] + nums2[j];
        mp[sum]++;
    }
}

// 遍历 nums3 + nums4，查找相反数
for (int k = 0; k < nums3.size(); ++k) {
    for (int l = 0; l < nums4.size(); ++l) {
        int tar = -(nums3[k] + nums4[l]);

        if (mp.find(tar) != mp.end()) {
            count += mp[tar];
        }
    }
}

return count;
```

---

# 二十九、时间复杂度

假设每个数组长度均为：

```text
n
```

第一部分：

```text
nums1 × nums2
```

需要：

```text
O(n²)
```

第二部分：

```text
nums3 × nums4
```

也是：

```text
O(n²)
```

哈希查询平均：

```text
O(1)
```

所以最终：

```text
时间复杂度：O(n²)
```

---

# 三十、空间复杂度

哈希表最多保存：

```text
nums1 × nums2
```

产生的大量两数和。

因此：

```text
空间复杂度：O(n²)
```

相比暴力：

```text
O(n⁴)
```

已经有很明显的提升。

---

# 今日易错点

## 1. 不理解 target

```cmake
add_executable(buffer_demo main.cpp buffer.cpp)
```

其中：

```text
buffer_demo
```

就是一个：

```text
target
```

---

## 2. 混淆 `cmake ..` 与 `cmake --build .`

记忆：

```text
cmake ..
=
配置：准备怎么编

cmake --build .
=
构建：真正开始编
```

---

## 3. 修改 cpp 后不一定需要重新 cmake

只修改：

```text
.cpp
.hpp
```

一般：

```bash
cmake --build .
```

即可。

---

## 4. CMakeLists 改动后应重新配置

修改：

```text
CMakeLists.txt
```

以后建议：

```bash
cmake ..
cmake --build .
```

---

## 5. 不要把构建生成文件和源码混在一起

推荐：

```text
project/
├── CMakeLists.txt
├── main.cpp
└── build/
```

而不是把：

```text
.o
CMakeCache.txt
Makefile
可执行程序
```

全部堆在源码目录。

---

## 6. CMakeLists 少写 cpp 可能造成链接错误

如果：

```text
头文件存在
函数声明存在
```

但是：

```text
实现所在 cpp 没有加入 target
```

那么可能：

```text
编译成功
链接失败
```

并产生：

```text
undefined reference
```

---

## 7. 不要只看最后一条编译错误

应优先处理：

```text
第一个真正的 error / fatal error
```

后面的错误可能只是连锁反应。

---

## 8. Four Sum Count 中 map 存的是“和 → 次数”

不要和 Day14 混淆：

```text
Two Sum：
数字 → 下标

Four Sum Count：
两数之和 → 次数
```

---

## 9. 不能写 `count++`

正确：

```cpp
count += mp[tar];
```

因为可能存在：

```text
多个前半组合
```

---

## 10. 不能写 `mp[tar]--`

这些组合不是：

```text
消耗品
```

哈希表只是：

```text
统计数量
```

---

## 11. 两层循环中的数组不要写错

第一部分必须：

```cpp
nums1[i] + nums2[j]
```

第二部分：

```cpp
nums3[k] + nums4[l]
```

四个数组、四个下标很多，非常容易复制代码时写错。

---

# Day14 与 Day15 的连接

这两天实际上是一条完整学习路线。

Day14 手工学习：

```text
源代码
↓
编译
↓
目标文件
↓
链接
↓
程序
```

例如：

```bash
g++ -c main.cpp -o main.o
g++ -c buffer.cpp -o buffer.o
g++ main.o buffer.o -o app
```

Day15：

```cmake
add_executable(
    app
    main.cpp
    buffer.cpp
)
```

让 CMake 自动管理这一过程。

所以：

```text
Day14
=
理解底层原理

Day15
=
学习工程化管理工具
```

二者并不是两块独立知识。

---

# 今日口述复盘

需要能够回答：

1. CMake 是什么？
2. CMake 是不是编译器？
3. CMake 和 g++ 是什么关系？
4. `CMakeLists.txt` 是做什么的？
5. `cmake_minimum_required()` 有什么作用？
6. `project()` 是什么？
7. `add_executable()` 做了什么？
8. target 是什么意思？
9. `cmake ..` 和 `cmake --build .` 有什么区别？
10. 什么是 out-of-source build？
11. 为什么推荐使用 `build/`？
12. 什么是增量构建？
13. 为什么只修改 `buffer.cpp` 时 `main.cpp` 不需要重新编译？
14. 为什么 CMakeLists 少写 `buffer.cpp` 会出现链接错误？
15. `undefined reference` 一般属于哪个阶段？
16. Buffer 的 `.hpp` 与 `.cpp` 分别负责什么？
17. 阅读大量编译错误时应该先看哪里？
18. 454 为什么可以把 `O(n⁴)` 降到 `O(n²)`？
19. Four Sum Count 的哈希表保存什么？
20. 为什么使用 `count += mp[tar]`？
21. 为什么不能 `mp[tar]--`？
22. Two Sum 和 Four Sum Count 的哈希表设计有什么区别？

---

## 今日总结

Day15 的重点不是只学会几个 CMake 命令，而是开始建立：

```text
C++ 工程构建
```

的概念。

Day14 已经知道：

```text
.cpp
↓
.o
↓
链接
↓
程序
```

Day15 则进一步知道：

```text
真正的工程不能永远手写几十个 g++ 命令
```

所以需要 CMake 来描述：

```text
哪些源文件属于程序
使用什么 C++ 标准
需要生成什么 target
怎样组织整个构建过程
```

算法方面则进一步理解了哈希表不仅可以：

```text
保存下标
```

还可以：

```text
统计出现次数
```

并通过：

```text
A + B + C + D = 0

↓

A + B = -(C + D)
```

学习了典型的“空间换时间”和“拆分问题”思想。

