Day17 学习总结

今日学习内容

今天主要学习了 Linux/C++ 工程中的静态库与动态库，使用同一套 calculator 代码分别生成 .a 和 .so，并通过 file、ar、nm、ldd 以及临时移走动态库的实验，验证两种链接方式在构建阶段和运行阶段的区别。

算法部分完成了 LeetCode 344「反转字符串」，使用双指针在原数组上交换字符。

一、库的作用

当多个程序都要使用同一组函数时，可以把函数声明放在头文件中，把实现编译成库，再由不同程序链接该库。这样可以复用代码，并把“库的实现”和“程序的使用方式”分开。

本次工程使用同一套源代码：

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
    └── build/                 # 构建目录，不提交 GitHub

calculator.hpp 负责声明接口：

int add(int a, int b);
int subtract(int a, int b);

calculator.cpp 负责实现接口，examples/main.cpp 通过头文件调用两个函数。示例中 a = 20、b = 10，最终输出：

add = 30
subtract = 10

二、静态库 .a

Linux 中静态库通常以 .a 结尾。本次通过 CMake 创建静态库：

add_library(
    calculator_static
    STATIC
    src/calculator.cpp
)

形成过程可以理解为：

calculator.cpp
      ↓ 编译
calculator.cpp.o
      ↓ 归档
libcalculator.a

静态链接时，链接器会从 .a 中取出程序需要的目标代码，放入最终可执行文件。因此程序构建完成后，运行时通常不再需要原来的 .a 文件。

本次使用以下命令验证静态库：

file libcalculator.a
ar -t libcalculator.a
nm -C libcalculator.a

file 显示它是 current ar archive。

ar -t 查看归档中包含的目标文件，本次能看到 calculator.cpp.o。

nm -C 查看库中的符号，-C 会把 C++ 名字修饰后的符号还原成较易阅读的函数名。

三、动态库 .so

Linux 中动态库也叫共享库，通常以 .so 结尾。本次通过 CMake 创建动态库：

add_library(
    calculator_shared
    SHARED
    src/calculator.cpp
)

file libcalculator.so 显示其类型为 ELF shared object。与静态链接不同，链接动态库的程序会保留对 .so 的运行时依赖。程序启动时，动态加载器需要找到对应的共享库，程序才能正常运行。

使用下面的命令可以检查可执行文件的动态依赖：

ldd ./static_demo
ldd ./shared_demo

实际观察结果：

static_demo 的依赖列表中没有 libcalculator.so。

shared_demo 明确依赖构建目录中的 libcalculator.so。

static_demo 仍可能依赖 libstdc++.so、libc.so 等系统动态库。这不代表 calculator 静态链接失败，只说明程序的其他系统组件仍采用动态链接。

四、CMake 中的关键写法

1. STATIC 与 SHARED

add_library(calculator_static STATIC src/calculator.cpp)
add_library(calculator_shared SHARED src/calculator.cpp)

STATIC 生成静态库。

SHARED 生成动态库。

2. 公开头文件目录

target_include_directories(
    calculator_static
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

PUBLIC 表示库自身编译时需要该目录，链接这个库的目标也会继承该头文件搜索路径。因此 static_demo 和 shared_demo 不需要重复配置 include/。

3. 设置输出文件名

set_target_properties(
    calculator_static
    PROPERTIES
    OUTPUT_NAME calculator
)

CMake 的目标名分别是 calculator_static 和 calculator_shared，用来保证目标唯一；通过相同的 OUTPUT_NAME calculator，最终可以生成符合 Linux 命名习惯的：

libcalculator.a
libcalculator.so

4. 链接可执行程序

target_link_libraries(static_demo PRIVATE calculator_static)
target_link_libraries(shared_demo PRIVATE calculator_shared)

这里的 PRIVATE 表示链接依赖只属于当前可执行目标，不需要继续向下传播。

五、构建与验证过程

采用 out-of-source build，把 CMake 生成文件和编译产物集中放入 build/：

cd ~/cpp_september/day17/static_dynamic_lib/build
cmake ..
cmake --build .

生成的核心文件为：

libcalculator.a
libcalculator.so
static_demo
shared_demo

两个程序运行结果相同，但取得函数实现的方式不同：

static_demo 在链接阶段获得所需代码。

shared_demo 在运行阶段加载 libcalculator.so。

六、移走 .so 的对照实验

为了直观验证运行时依赖，临时把动态库改名：

mv libcalculator.so libcalculator.so.backup
./static_demo
./shared_demo

实验结果：

static_demo 仍能正常运行，因为需要的 calculator 代码已经进入可执行文件。

shared_demo 无法启动，并提示找不到 libcalculator.so，因为它在运行时仍依赖该共享库。

实验完成后恢复文件：

mv libcalculator.so.backup libcalculator.so
./shared_demo

这个实验是理解 .a 与 .so 区别最直接的证据。

七、静态库与动态库对比

对比项

静态库 .a

动态库 .so

英文

Static Library

Shared Library

链接方式

所需代码在链接阶段进入可执行文件

可执行文件保留共享库依赖

运行时

通常不再需要原 .a

必须能找到对应 .so

文件大小

可执行文件通常相对更大

可执行文件通常相对更小

多程序共享

每个程序可能各自包含一份库代码

多个程序可以共享同一动态库

部署

相对直接

需要处理共享库搜索路径和版本

更新

库变化后通常要重新链接程序

ABI 兼容时可能只替换动态库

动态库不是“随便替换 .so 都能兼容”。真实工程还要考虑 ABI、接口兼容性、库版本和 SONAME。

八、LeetCode 344：反转字符串

题目要求原地反转 vector<char>，不能额外创建一个同等大小的数组。

核心思路是双指针：

left 指向首字符。

right 指向尾字符。

交换 s[left] 与 s[right]。

left 向右移动，right 向左移动。

当 left >= right 时结束。

核心代码：

void reverseString(vector<char>& s) {
    int left = 0;
    int right = static_cast<int>(s.size()) - 1;

    while (left < right) {
        swap(s[left], s[right]);
        ++left;
        --right;
    }
}

为什么循环条件是 left < right

偶数长度字符串：两个指针交错后结束。

奇数长度字符串：两个指针相遇时，中间字符不需要交换。

如果写成 left <= right，中间字符会与自身交换，虽然结果不一定错误，但多做了一次无意义操作。

为什么使用引用参数

vector<char>& s

引用让函数直接修改原数组，也避免复制整个 vector，符合题目要求的原地操作。

复杂度

时间复杂度：O(n)。

额外空间复杂度：O(1)。

九、易错点

不要把 STATIC 和 SHARED 写反。

.a 是目标文件的归档，不是可以直接运行的程序。

ldd static_demo 出现系统 .so 很正常，判断本次实验是否静态链接，要看其中是否存在 libcalculator.so。

两个 CMake 目标不能重名，所以内部目标名使用 calculator_static、calculator_shared；输出文件名可以都设置成 calculator。

临时移走 .so 后一定要恢复，否则后续运行 shared_demo 会继续失败。

build/、.o、.a、.so 和可执行文件都是可重新生成的构建产物，不应上传 GitHub。

反转字符串时，交换后必须同时执行 ++left 和 --right，否则循环可能无法结束。

s.size() 返回无符号类型；转换为 int 后再减一，代码意图更清楚。

十、今日总结

今天完成了从“源文件”到“库”再到“可执行程序”的完整工程实践。能够解释 .a 和 .so 的基本区别，能够使用 CMake 的 STATIC、SHARED、target_include_directories 和 target_link_libraries 构建两种库，也能用 file、ar、nm、ldd 和移走 .so 的实验验证判断，而不只是记忆概念。

算法方面掌握了双指针原地反转字符串：通过左右指针交换并向中间收缩，在 O(n) 时间和 O(1) 额外空间内完成操作。
