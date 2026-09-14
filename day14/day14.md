# Day14 学习总结

## 今日学习内容

今天主要学习了 C++ 程序从 `.cpp` 源代码到最终可执行文件的完整构建过程，并在 Linux 中亲手拆解了：

```text
预处理
↓
编译
↓
汇编
↓
链接
```

同时进一步学习了：

* `.cpp`、`.i`、`.s`、`.o` 和可执行文件之间的关系
* `g++ -E`、`g++ -S`、`g++ -c`
* 编译错误与链接错误的区别
* 函数声明与函数定义
* 多个 `.cpp` 文件如何完成链接
* `.hpp` 与 `.cpp` 的基本职责
* `nm` 查看目标文件中的符号
* `unordered_map` 的基本使用
* LeetCode 1「两数之和」

---

## 一、C++ 程序的完整构建过程

平时编译一个程序时经常直接写：

```bash
g++ hello.cpp -o hello
```

看起来只有一条命令，但实际上 C++ 程序会经历四个阶段：

```text
hello.cpp
    ↓
Preprocessing
预处理
    ↓
hello.i
    ↓
Compilation
编译
    ↓
hello.s
    ↓
Assembly
汇编
    ↓
hello.o
    ↓
Linking
链接
    ↓
hello
```

最重要的顺序：

```text
预处理 → 编译 → 汇编 → 链接
```

对应英文：

```text
Preprocessing
Compilation
Assembly
Linking
```

---

## 二、预处理 Preprocessing

使用：

```bash
g++ -E hello.cpp -o hello.i
```

其中：

```text
-E
```

表示：

```text
只执行预处理，然后停止
```

预处理阶段主要处理：

```cpp
#include
#define
#if
#ifdef
#ifndef
#endif
```

例如源代码：

```cpp
#define APP_NAME "Day14"
```

后面使用：

```cpp
std::cout << APP_NAME;
```

预处理完成以后，会变成类似：

```cpp
std::cout << "Day14";
```

说明宏已经被展开。

---

## 三、为什么 `.i` 文件非常大

原来的：

```cpp
#include <iostream>
```

并不是简单保留这一行。

预处理器会把相关头文件内容展开，因此一个只有十几行的：

```text
hello.cpp
```

经过预处理以后：

```text
hello.i
```

可能会达到几万行。

可以使用：

```bash
wc -l hello.cpp hello.i
```

观察行数变化。

因此：

```text
.cpp
↓
处理宏和头文件
↓
.i
```

---

## 四、编译 Compilation

接下来使用：

```bash
g++ -S hello.i -o hello.s
```

其中：

```text
-S
```

表示：

```text
生成汇编代码后停止
```

编译阶段主要进行：

```text
语法分析
类型检查
语义分析
代码优化
生成汇编代码
```

于是：

```text
hello.i
↓
Compiler
↓
hello.s
```

打开 `.s` 文件以后，会看到类似：

```asm
main:
    pushq %rbp
    movq %rsp, %rbp
```

这时已经不是正常的 C++ 源代码，而是汇编代码。

因此要注意：

> 编译不等于生成最终可执行程序。

严格拆分时：

```text
Compilation
```

只是整个构建过程中的其中一个阶段。

---

## 五、汇编 Assembly

执行：

```bash
g++ -c hello.s -o hello.o
```

生成：

```text
hello.o
```

`.o` 称为：

```text
Object File
目标文件
```

可以执行：

```bash
file hello.o
```

通常能够看到类似：

```text
ELF 64-bit LSB relocatable
```

其中：

```text
relocatable
```

表示：

```text
可重定位目标文件
```

此时已经包含机器代码，但是它通常还不是一个完整的可执行程序。

因此：

```text
hello.s
↓
Assembler
↓
机器码
↓
hello.o
```

---

## 六、`.o` 为什么不能直接运行

虽然 `.o` 中已经存在机器代码，但它仍然只是：

```text
目标文件
```

而不是：

```text
完整的可执行程序
```

其中可能还存在：

```text
没有解析的函数
库函数引用
其他目标文件中的符号
```

所以还必须经过：

```text
Linking
```

---

## 七、链接 Linking

执行：

```bash
g++ hello.o -o hello
```

最终生成：

```text
hello
```

运行：

```bash
./hello
```

例如：

```text
Day14: 5
```

完整流程可以记为：

```text
hello.cpp
    ↓ g++ -E
hello.i
    ↓ g++ -S
hello.s
    ↓ g++ -c
hello.o
    ↓ g++
hello
```

---

## 八、使用 `nm` 查看符号

可以使用：

```bash
nm -C hello.o
```

查看目标文件中的符号。

例如：

```text
T add(int, int)
T main
```

其中：

```text
T
```

表示：

```text
该符号在当前目标文件中已经定义
并位于代码段中
```

如果看到：

```text
U add(int, int)
```

其中：

```text
U
```

表示：

```text
Undefined
```

也就是：

```text
当前目标文件使用了 add()
但是当前目标文件没有 add() 的定义
```

链接器需要到：

```text
其他 .o 文件
或者库
```

中寻找它。

---

# 九、编译错误与链接错误

这是今天最重要的知识点之一。

---

## 1. 编译错误

例如：

```cpp
int number = 10
std::cout << number;
```

因为：

```cpp
int number = 10
```

后面缺少：

```cpp
;
```

编译：

```bash
g++ -c compile_error.cpp -o compile_error.o
```

会直接报错。

此时：

```text
.cpp
↓
编译失败
↓
.o 无法正常产生
```

常见编译错误包括：

```text
语法错误
变量未声明
类型错误
函数参数错误
类名拼写错误
成员访问错误
```

---

## 2. 链接错误

例如：

```cpp
int add(int a, int b);

int main() {
    int result = add(10, 20);
}
```

这里：

```cpp
int add(int a, int b);
```

只有声明。

没有：

```cpp
int add(int a, int b) {
    return a + b;
}
```

因此：

```bash
g++ -c link_error.cpp -o link_error.o
```

可能成功。

通过：

```bash
nm -C link_error.o | grep add
```

可以看到：

```text
U add(int, int)
```

但是执行：

```bash
g++ link_error.o -o link_error
```

链接阶段会出现：

```text
undefined reference to `add(int, int)'
```

因此：

```text
编译成功
↓
.o 已经产生
↓
链接器寻找 add()
↓
找不到真正定义
↓
链接失败
```

---

# 十、声明 Declaration 与定义 Definition

函数声明：

```cpp
int add(int a, int b);
```

表示：

```text
告诉编译器存在 add 函数
返回值是 int
参数是两个 int
```

但是没有具体实现。

函数定义：

```cpp
int add(int a, int b) {
    return a + b;
}
```

才真正提供实现。

可以简单理解为：

```text
Declaration
声明
=
告诉编译器“它长什么样”

Definition
定义
=
告诉程序“它真正怎么做”
```

---

# 十一、多个 `.cpp` 文件如何工作

假设有：

```text
main.cpp
add.cpp
```

分别编译：

```bash
g++ -c main.cpp -o main.o
g++ -c add.cpp -o add.o
```

此时可能出现：

```bash
nm -C main.o | grep add
```

输出：

```text
U add(int, int)
```

表示：

```text
main.o 需要 add()
```

而：

```bash
nm -C add.o | grep add
```

输出：

```text
T add(int, int)
```

表示：

```text
add.o 提供 add()
```

最后：

```bash
g++ main.o add.o -o app
```

链接器就会将：

```text
main.o 中的 U add()
```

与：

```text
add.o 中的 T add()
```

匹配起来。

最终得到完整程序。

---

# 十二、头文件与实现文件分离

一个简单工程可以组织成：

```text
add.hpp
add.cpp
main.cpp
```

## add.hpp

主要放：

```text
声明
接口
```

例如：

```cpp
#ifndef ADD_HPP
#define ADD_HPP

int add(int a, int b);

#endif
```

---

## add.cpp

主要放：

```text
定义
实现
```

例如：

```cpp
#include "add.hpp"

int add(int a, int b) {
    return a + b;
}
```

---

## main.cpp

使用：

```cpp
#include <iostream>
#include "add.hpp"

int main() {
    std::cout << add(10, 20) << std::endl;
    return 0;
}
```

这种：

```text
.hpp
+
.cpp
```

的结构实现了：

```text
接口与实现分离
```

也是以后工程化开发的重要基础。

---

# 十三、`#include <>` 与 `#include ""`

标准库通常使用：

```cpp
#include <iostream>
#include <vector>
#include <unordered_map>
```

自己的项目头文件通常使用：

```cpp
#include "add.hpp"
#include "buffer.hpp"
```

当天遇到过：

```cpp
#include <buffer.hpp>
```

导致：

```text
fatal error: buffer.hpp: 没有那个文件或目录
```

对于当前目录中的自定义头文件，应该使用：

```cpp
#include "buffer.hpp"
```

---

# 十四、LeetCode 1：两数之和

## 1. 问题

需要寻找：

```cpp
nums[i] + nums[j] == target
```

并返回：

```text
i 和 j
```

---

## 2. 暴力方法

使用两层循环：

```text
时间复杂度 O(n²)
```

但可以通过哈希表降低到平均：

```text
O(n)
```

---

# 十五、`unordered_map`

定义：

```cpp
std::unordered_map<int, int> mp;
```

两数之和中存储的是：

```text
数字 → 下标
```

例如：

```cpp
mp[2] = 0;
mp[7] = 1;
```

代表：

```text
2 → index 0
7 → index 1
```

---

## 查找

```cpp
mp.find(key)
```

如果：

```cpp
mp.find(key) != mp.end()
```

说明：

```text
key 存在
```

---

## 遍历

可以写：

```cpp
for (const auto& pair : mp) {
    std::cout << pair.first
              << " "
              << pair.second
              << std::endl;
}
```

其中：

```text
pair.first  = key
pair.second = value
```

C++17 也可以：

```cpp
for (const auto& [key, value] : mp) {
    std::cout << key << " " << value << std::endl;
}
```

注意：

```text
unordered_map 不保证遍历顺序
```

---

# 十六、两数之和的核心思路

当前数字：

```cpp
nums[i]
```

需要寻找：

```cpp
target - nums[i]
```

因此：

```cpp
int need = target - nums[i];
```

如果：

```cpp
mp.find(need) != mp.end()
```

说明之前已经出现过：

```text
need
```

于是：

```cpp
return {mp[need], i};
```

核心代码：

```cpp
unordered_map<int, int> mp;

for (int i = 0; i < nums.size(); ++i) {
    int need = target - nums[i];

    if (mp.find(need) != mp.end()) {
        return {mp[need], i};
    }

    mp[nums[i]] = i;
}

return {};
```

---

# 十七、为什么要“先查再插”

正确：

```cpp
if (mp.find(need) != mp.end()) {
    return {mp[need], i};
}

mp[nums[i]] = i;
```

这样避免：

```text
当前元素与自己匹配
```

例如：

```text
nums = [3, 3]
target = 6
```

第一次：

```text
nums[0] = 3
need = 3
```

哈希表没有 3。

然后：

```text
3 → 0
```

第二次：

```text
nums[1] = 3
need = 3
```

找到：

```text
3 → 0
```

返回：

```text
{0, 1}
```

---

# 今日易错点

## 1. 四个阶段不要漏掉“汇编”

错误：

```text
预处理 → 编译 → 链接
```

正确：

```text
预处理 → 编译 → 汇编 → 链接
```

---

## 2. `.o` 不是最终可执行文件

`.o` 是：

```text
Object File
可重定位目标文件
```

仍然需要链接。

---

## 3. `undefined reference` 通常不是语法错误

看到：

```text
undefined reference
```

应该优先想到：

```text
链接阶段
```

例如：

```text
有声明
没有定义
```

或者：

```text
对应 .cpp 没有参与链接
```

---

## 4. 声明不等于定义

```cpp
int add(int, int);
```

只是声明。

真正定义：

```cpp
int add(int a, int b) {
    return a + b;
}
```

---

## 5. 自定义头文件通常用双引号

```cpp
#include "buffer.hpp"
```

而不是：

```cpp
#include <buffer.hpp>
```

---

## 6. 循环变量不要写错

例如循环使用：

```cpp
i
```

就不要误写成：

```cpp
nums[j]
```

这类错误在算法题中特别常见。

---

## 7. `vector<int>` 返回值不能写 `return 0`

错误：

```cpp
return 0;
```

正确可以：

```cpp
return {};
```

---

## 8. Two Sum 中 map 保存的是“数字 → 下标”

今天要特别区分：

```text
Two Sum
数字 → 下标
```

后面 Day15 的 454 会变成：

```text
两数之和 → 次数
```

---

# 今日核心复盘

需要能够脱离笔记回答：

1. C++ 源代码到可执行程序经历哪四个阶段？
2. `.i`、`.s`、`.o` 分别是什么？
3. `g++ -E` 的作用是什么？
4. `g++ -S` 的作用是什么？
5. `g++ -c` 的作用是什么？
6. `.o` 为什么还不能直接当作完整程序？
7. 编译错误和链接错误有什么区别？
8. `undefined reference` 通常是什么问题？
9. 声明和定义有什么区别？
10. 为什么工程中通常使用 `.hpp + .cpp`？
11. `nm -C` 可以用来观察什么？
12. `T` 和 `U` 符号分别表示什么？
13. `unordered_map::find()` 如何判断元素是否存在？
14. 两数之和为什么可以从 `O(n²)` 降低到平均 `O(n)`？
15. 为什么 Two Sum 要先查找，再把当前数字插入哈希表？

---

## 今日总结

Day14 的重点不仅是记住几个 `g++` 参数，而是第一次真正理解：

```text
C++ 源代码
为什么不能直接变成程序
```

以及：

```text
编译器负责什么
汇编器负责什么
链接器负责什么
```

以后再看到：

```text
syntax error
was not declared
undefined reference
```

就可以先判断问题属于构建流程中的哪一层，再进行调试。

这为下一步学习 CMake 打下了基础，因为 CMake 本质上就是帮助我们组织和管理这些编译、目标文件和链接过程。

