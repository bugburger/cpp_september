# Day17 学习总结

## 今日学习内容

今天主要学习了 Linux/C++ 工程中的静态库与动态库，使用同一套 `calculator` 源代码分别生成静态库 `.a` 和动态库 `.so`，并通过实际运行、`file`、`ar`、`nm`、`ldd` 以及临时移走动态库等方式，验证两种链接方式的区别。

算法部分完成了 LeetCode 344「反转字符串」，学习了使用双指针在原数组上交换字符。

---

## 一、为什么需要库

在实际项目中，多个程序可能需要使用相同的函数。

例如，加法和减法函数可能会被多个程序调用：

```cpp
int add(int a, int b);
int subtract(int a, int b);
```

如果每个程序都重复编写这些函数，会产生大量重复代码。

更合理的做法是：

1. 把函数声明放进头文件；
2. 把函数实现放进源文件；
3. 将源文件编译成库；
4. 其他程序通过链接库使用这些函数。

库可以实现代码复用，并将“接口”和“实现”分离。

本次工程的大致目录如下：

```text
day17/
├── day17.md
├── reverse_string.cpp
└── static_dynamic_lib/
    ├── CMakeLists.txt
    ├── include/
    │   └── calculator.hpp
    ├── src/
    │   └── calculator.cpp
    ├── examples/
    │   └── main.cpp
    └── build/
```

其中：

- `calculator.hpp`：声明函数接口；
- `calculator.cpp`：实现加法和减法；
- `main.cpp`：调用库中的函数；
- `CMakeLists.txt`：描述工程的构建方式；
- `build/`：保存编译产生的文件，不上传 GitHub。

---

## 二、静态库

### 1. 什么是静态库

Linux 中的静态库通常以 `.a` 结尾，例如：

```text
libcalculator.a
```

静态链接时，链接器会从静态库中取出程序需要的目标代码，并放入最终的可执行文件。

可以理解为：

```text
calculator.cpp
      ↓ 编译
calculator.cpp.o
      ↓ 归档
libcalculator.a
      ↓ 静态链接
static_demo
```

程序完成静态链接以后，运行时通常不再需要原来的 `.a` 文件，因为需要的代码已经进入了可执行文件。

---

### 2. 使用 CMake 创建静态库

```cmake
add_library(
    calculator_static
    STATIC
    src/calculator.cpp
)
```

其中：

- `add_library`：创建一个库目标；
- `calculator_static`：CMake 内部使用的目标名；
- `STATIC`：表示创建静态库；
- `src/calculator.cpp`：参与编译的源文件。

最终可以生成：

```text
libcalculator.a
```

---

### 3. 查看静态库信息

可以使用以下命令检查静态库：

```bash
file libcalculator.a
```

`file` 用于判断文件类型。静态库通常会显示为：

```text
current ar archive
```

查看静态库中包含的目标文件：

```bash
ar -t libcalculator.a
```

可能看到：

```text
calculator.cpp.o
```

查看库中包含的符号：

```bash
nm -C libcalculator.a
```

其中：

- `nm`：查看目标文件或库中的符号；
- `-C`：将经过 C++ 名字修饰的符号恢复成便于阅读的函数名。

---

## 三、动态库

### 1. 什么是动态库

Linux 中的动态库也叫共享库，通常以 `.so` 结尾，例如：

```text
libcalculator.so
```

动态链接时，函数代码不会完整复制进当前可执行文件。程序运行时，动态加载器需要找到对应的 `.so` 文件。

因此，使用动态库的程序通常同时依赖：

```text
可执行程序 + 动态库
```

如果运行时找不到需要的动态库，程序就可能无法启动。

---

### 2. 使用 CMake 创建动态库

```cmake
add_library(
    calculator_shared
    SHARED
    src/calculator.cpp
)
```

其中：

- `SHARED`：表示创建动态库；
- 最终生成的文件通常为 `libcalculator.so`。

查看动态库类型：

```bash
file libcalculator.so
```

通常会显示它是 ELF shared object。

---

## 四、配置头文件搜索路径

头文件位于 `include/` 目录中，因此需要告诉编译器去哪里寻找头文件：

```cmake
target_include_directories(
    calculator_static
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(
    calculator_shared
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

这里的 `PUBLIC` 表示：

1. 库本身在编译时需要这个头文件目录；
2. 链接该库的其他目标也会继承这个目录。

因此，链接该库的可执行程序不需要再次重复配置头文件路径。

---

## 五、设置库的输出名称

CMake 中的目标名不能重复，因此分别使用：

```text
calculator_static
calculator_shared
```

但是可以将最终输出名称都设置成 `calculator`：

```cmake
set_target_properties(
    calculator_static
    PROPERTIES
    OUTPUT_NAME calculator
)

set_target_properties(
    calculator_shared
    PROPERTIES
    OUTPUT_NAME calculator
)
```

最终分别生成：

```text
libcalculator.a
libcalculator.so
```

CMake 内部目标名与最终文件名是两个不同的概念：

- 目标名必须唯一；
- 输出文件名可以通过属性修改。

---

## 六、链接可执行程序

创建可执行程序：

```cmake
add_executable(
    static_demo
    examples/main.cpp
)

add_executable(
    shared_demo
    examples/main.cpp
)
```

分别链接静态库和动态库：

```cmake
target_link_libraries(
    static_demo
    PRIVATE
    calculator_static
)

target_link_libraries(
    shared_demo
    PRIVATE
    calculator_shared
)
```

其中：

- `static_demo` 链接静态库；
- `shared_demo` 链接动态库；
- `PRIVATE` 表示该链接关系只属于当前可执行目标，不继续向其他目标传播。

---

## 七、使用 CMake 构建工程

进入项目目录：

```bash
cd ~/cpp_september/day17/static_dynamic_lib
```

创建独立的构建目录：

```bash
mkdir -p build
cd build
```

生成构建文件：

```bash
cmake ..
```

执行编译：

```bash
cmake --build .
```

构建完成后，可以得到：

```text
libcalculator.a
libcalculator.so
static_demo
shared_demo
```

使用独立的 `build/` 目录叫作 out-of-source build。

优点是：

- 源代码目录更加整洁；
- 编译产物集中存放；
- 清理项目时只需删除 `build/`；
- 不容易把构建产物误上传到 GitHub。

---

## 八、运行程序

运行静态链接版本：

```bash
./static_demo
```

运行动态链接版本：

```bash
./shared_demo
```

示例输出：

```text
add = 30
subtract = 10
```

虽然两个程序的运行结果相同，但是获得函数实现的方式不同：

- `static_demo` 在链接阶段获得函数代码；
- `shared_demo` 在运行阶段加载共享库。

---

## 九、使用 ldd 检查动态依赖

检查静态版本：

```bash
ldd ./static_demo
```

检查动态版本：

```bash
ldd ./shared_demo
```

观察结果：

- `static_demo` 的依赖中没有 `libcalculator.so`；
- `shared_demo` 明确依赖 `libcalculator.so`。

需要注意，`static_demo` 仍然可能依赖：

```text
libstdc++.so
libc.so
libm.so
```

这是正常现象。

本次所谓“静态链接”，指的是 `calculator` 这部分代码采用静态链接，并不表示整个程序的所有系统库都采用静态链接。

---

## 十、移走动态库的对照实验

为了验证程序是否依赖动态库，可以临时移动 `.so` 文件：

```bash
mv libcalculator.so libcalculator.so.backup
```

然后分别运行：

```bash
./static_demo
./shared_demo
```

实验结果：

- `static_demo` 仍然可以运行；
- `shared_demo` 无法正常启动；
- 系统提示找不到 `libcalculator.so`。

原因是：

- 静态版本需要的代码已经进入可执行文件；
- 动态版本在运行时仍然需要加载 `.so`。

实验完成后恢复动态库：

```bash
mv libcalculator.so.backup libcalculator.so
```

再次运行：

```bash
./shared_demo
```

程序恢复正常。

这个实验直观证明了静态链接和动态链接在运行阶段的区别。

---

## 十一、静态库与动态库对比

| 对比项 | 静态库 `.a` | 动态库 `.so` |
|---|---|---|
| 英文名称 | Static Library | Shared Library |
| 链接方式 | 需要的代码进入可执行文件 | 可执行文件记录动态库依赖 |
| 运行时依赖 | 通常不再需要原 `.a` | 必须能找到对应 `.so` |
| 可执行文件 | 通常相对较大 | 通常相对较小 |
| 多程序共享 | 每个程序可能包含一份代码 | 多个程序可以共享同一个库 |
| 部署方式 | 相对简单 | 需要一起部署动态库 |
| 更新方式 | 库变化后通常重新链接 | ABI 兼容时可以替换动态库 |
| 常见问题 | 可执行文件较大 | 找不到库、版本不兼容 |

动态库并不是任何时候都可以直接替换，还需要考虑：

- 函数接口是否发生改变；
- ABI 是否兼容；
- 动态库版本是否匹配；
- SONAME 是否正确；
- 运行时搜索路径是否正确。

---

## 十二、LeetCode 344：反转字符串

### 1. 题目要求

给定一个字符数组，将其中的字符原地反转。

例如：

```text
输入：['h','e','l','l','o']
输出：['o','l','l','e','h']
```

题目要求原地修改数组，不能额外创建一个同样大小的新数组。

---

### 2. 双指针思路

定义两个指针：

```text
left  → 指向字符串开头
right → 指向字符串末尾
```

每次交换两个位置的字符：

```cpp
swap(s[left], s[right]);
```

然后让两个指针向中间移动：

```cpp
++left;
--right;
```

当：

```cpp
left >= right
```

说明所有字符都已经完成交换。

---

### 3. 完整代码

```cpp
#include <iostream>
#include <utility>
#include <vector>

class Solution {
public:
    void reverseString(std::vector<char>& s) {
        int left = 0;
        int right = static_cast<int>(s.size()) - 1;

        while (left < right) {
            std::swap(s[left], s[right]);
            ++left;
            --right;
        }
    }
};

int main() {
    std::vector<char> s{'h', 'e', 'l', 'l', 'o'};

    Solution solution;
    solution.reverseString(s);

    for (char ch : s) {
        std::cout << ch << ' ';
    }

    std::cout << '\n';
    return 0;
}
```

运行结果：

```text
o l l e h
```

---

### 4. 为什么使用引用参数

函数参数为：

```cpp
std::vector<char>& s
```

这里的 `&` 表示引用。

使用引用有两个作用：

1. 函数直接修改原来的 `vector`；
2. 避免复制整个 `vector`。

如果没有引用：

```cpp
void reverseString(std::vector<char> s)
```

函数得到的是原数组的副本，对副本的修改不会影响外部原数组。

---

### 5. 为什么条件是 `left < right`

循环条件写成：

```cpp
while (left < right)
```

原因是：

- 偶数长度：两个指针交错后结束；
- 奇数长度：两个指针会在中间字符相遇；
- 中间字符不需要和自己交换。

如果写成：

```cpp
left <= right
```

奇数长度时会让中间字符和自己交换一次。结果虽然不一定错误，但属于没有必要的操作。

---

### 6. 复杂度分析

时间复杂度：

```text
O(n)
```

因为每个字符最多参与一次交换。

额外空间复杂度：

```text
O(1)
```

只使用了两个指针变量，没有创建随输入规模增长的新容器。

---

## 十三、今日易错点

1. 不要把 CMake 中的 `STATIC` 和 `SHARED` 写反。
2. `.a` 是静态库，不是可以直接运行的程序。
3. `.so` 是动态库，也不能当作普通可执行文件直接运行。
4. `add_library` 创建库，`add_executable` 创建可执行程序。
5. CMake 内部目标名不能重复。
6. `OUTPUT_NAME` 修改的是最终输出文件名，不是 CMake 目标名。
7. `ldd static_demo` 中出现系统动态库是正常的。
8. 判断本次静态链接是否成功，要看是否依赖 `libcalculator.so`。
9. 临时移动 `.so` 后一定要恢复。
10. `build/`、`.o`、`.a`、`.so` 和可执行文件不要上传 GitHub。
11. 双指针交换后必须同时修改 `left` 和 `right`。
12. `vector::size()` 返回无符号类型，转换成 `int` 后再减一更加清楚。
13. 修改原数组时，函数参数需要使用引用。
14. 循环条件应使用 `left < right`。

---

## 十四、今日总结

今天完成了从源文件、目标文件、库到可执行程序的完整工程实践。

我已经能够：

- 理解为什么要把公共代码封装成库；
- 区分静态库 `.a` 和动态库 `.so`；
- 使用 CMake 的 `STATIC` 和 `SHARED` 创建库；
- 使用 `target_include_directories` 配置头文件目录；
- 使用 `target_link_libraries` 链接库；
- 使用 `file`、`ar`、`nm` 和 `ldd` 检查构建结果；
- 通过移走 `.so` 的实验验证动态库的运行时依赖；
- 使用双指针原地反转字符串；
- 分析算法的时间复杂度和空间复杂度。

静态库和动态库是 Linux C++ 工程开发的重要基础，后续学习第三方库、项目构建和大型工程组织时都会继续使用这些知识。
