Day18 学习总结

今日学习内容

今天主要学习了 Linux 下的 GDB 基础调试流程，包括生成调试信息、启动 GDB、设置断点、运行程序、查看源码、单步执行、进入函数、查看变量、查看调用栈、查看参数和局部变量、管理断点，以及临时修改变量继续验证程序。

实践中使用 GDB 定位了 calculateAverage 中由 count = 0 导致的除零错误，并在调试过程中把 count 临时改为 4，验证程序能够输出 average = 25 并正常退出。

算法部分完成了 LeetCode 541「反转字符串 II」，掌握按每 2k 个字符分组、只反转每组前 k 个字符的写法。

一、Day18 文件结构

Day18 与 Day17 严格分开，GDB 文件不放在静态库/动态库工程中：

day18/
├── day18.md
├── debug_basic.cpp
├── debug_bug.cpp
└── reverse_string_ii.cpp

debug_basic.cpp 用于熟悉基本调试命令，debug_bug.cpp 用于定位人为制造的运行时错误，reverse_string_ii.cpp 用于完成当天算法练习。

二、为什么编译时要使用 -g

调试程序时使用：

g++ -std=c++17 -Wall -Wextra -g debug_basic.cpp -o debug_basic
g++ -std=c++17 -Wall -Wextra -g debug_bug.cpp -o debug_bug

-g 会在可执行文件中加入调试信息，使 GDB 能够显示：

源文件名和源代码行号；

函数名；

局部变量名；

函数参数；

调用栈。

没有 -g 时程序仍可能运行，也可以进入 GDB，但很多源码和变量信息会缺失，调试效率会明显下降。

-Wall -Wextra 用于打开常见编译警告。编译器警告和 GDB 的作用不同：警告帮助发现编译阶段可疑代码，GDB 用于观察程序运行时的真实状态。

三、GDB 基本工作流程

启动调试器：

gdb ./debug_basic

基本流程可以概括为：

使用 -g 编译
    ↓
gdb 启动程序
    ↓
break 设置断点
    ↓
run 运行到断点
    ↓
next / step 单步执行
    ↓
print / info 查看状态
    ↓
backtrace 查看调用关系
    ↓
continue 继续运行

退出 GDB：

quit

常用命令通常也可以使用缩写，例如 break 可写成 b，run 可写成 r，next 可写成 n，step 可写成 s，print 可写成 p，continue 可写成 c。

四、断点与程序运行

1. break

在函数处设置断点：

break main
break calculateAverage

也可以在某一行设置断点：

break debug_bug.cpp:6

断点的作用是让程序在指定位置暂停，从而检查当时的参数、局部变量和调用关系。

2. run

run

从程序入口开始运行，直到遇到断点、异常或程序结束。再次执行 run 会重新启动程序，而不是从当前位置继续。

3. continue

continue

从当前暂停位置继续执行，直到下一个断点、异常或程序结束。

run 与 continue 的区别：

run：重新启动程序。

continue：从当前现场继续运行。

五、查看源码与单步执行

1. list

list
list calculateAverage

用于查看当前位置附近的源代码，便于把断点位置和代码逻辑对应起来。

2. next

next

执行当前源代码行。如果当前行调用了函数，next 会把整个函数调用看成一步，通常不会进入函数内部。

3. step

step

执行当前源代码行。如果当前行调用了带调试信息的函数，step 会进入函数内部。

两者的核心区别：

命令

遇到函数调用时

next

执行完函数并停在下一行

step

进入函数内部继续调试

当只关心当前函数的整体流程时使用 next；当怀疑被调用函数内部存在问题时使用 step。

六、查看变量、参数和调用栈

1. print

print total
print count
print total / count

print 用于查看变量或表达式当前的值。它观察的是程序此刻的状态，而不是变量在源代码中的初始写法。

2. info args

info args

显示当前函数接收到的参数。本次在 calculateAverage 中实际看到：

total = 100
count = 0

这直接暴露了除数为零的问题。

3. info locals

info locals

显示当前栈帧中的局部变量。如果函数刚进入、某个变量还没有执行初始化语句，它的值可能尚未有效，不能脱离当前执行位置解释输出。

4. backtrace

backtrace

也可简写为：

bt

用于查看函数调用栈。调试 calculateAverage 时，可以看出当前函数由 main 调用。程序层次更深时，调用栈能帮助判断错误是从哪一层传入的。

七、断点管理

查看所有断点：

info breakpoints

删除指定编号的断点：

delete 1

删除所有断点时可以使用：

delete

断点编号由 GDB 分配，应先通过 info breakpoints 确认编号，不要把源码行号误当成断点编号。

八、用 GDB 定位除零错误

debug_bug.cpp 中人为设置了错误输入：

int total = 100;
int count = 0;
double average = calculateAverage(total, count);

calculateAverage 使用 count 作为除数。当程序运行到该函数时，通过以下命令检查现场：

break calculateAverage
run
info args
backtrace

info args 显示：

total = 100
count = 0

因此可以确定错误原因不是 GDB、编译器或输出语句，而是调用者把非法的 count = 0 传给了除法函数。

为了验证判断，可以在本次调试会话中临时修改变量：

set variable count = 4
print count
continue

程序随后输出：

average = 25

并正常退出。这说明“除数为零”就是当前故障的直接原因。

需要注意：set variable 只修改当前正在运行的进程状态，不会修改 debug_bug.cpp 源文件。真正修复代码时，仍然应该在源码中校验 count：

if (count == 0) {
    // 根据程序需求返回错误、抛出异常或采用其他处理方式
}

调试时临时改值的意义是验证推断，而不是代替源代码修复。

九、LeetCode 541：反转字符串 II

题目规则可以归纳为：从字符串开头开始，每 2k 个字符作为一组，只反转该组的前 k 个字符。

最终核心代码：

class Solution {
public:
    string reverseStr(string s, int k) {
        int n = static_cast<int>(s.size());

        for (int i = 0; i < n; i += 2 * k) {
            reverse(
                s.begin() + i,
                s.begin() + min(i + k, n)
            );
        }

        return s;
    }
};

1. 为什么步长是 2 * k

每组包含 2k 个字符：前 k 个反转，后 k 个保持不变。处理完一组后，下一组起点正好向后移动 2k：

i += 2 * k;

如果误写成 i += k，下一轮会把题目要求保持不动的后一半再次处理，逻辑就会错误。

2. 为什么右边界使用 min(i + k, n)

std::reverse 操作的是左闭右开区间 [first, last)。正常情况下要反转 [i, i + k)；最后一组可能不足 k 个字符，因此右边界不能超过 n：

min(i + k, n)

这样既满足题目“剩余字符不足 k 个时全部反转”的要求，也避免构造越界迭代器。

3. 为什么不需要单独判断 n < k

原写法先判断：

if (n < k) {
    reverse(s.begin(), s.end());
}

这段逻辑是正确的，但可以省略。因为当 n < k 时，第一次循环中的：

min(i + k, n)

会直接得到 n，自然执行 reverse(s.begin(), s.end())。统一循环能覆盖普通情况和尾部不足 k 个字符的情况，代码更简洁。

4. 示例分析

当 s = "abcdefg"、k = 2 时：

i = 0，反转 [0, 2)，ab 变为 ba。

i = 4，反转 [4, 6)，ef 变为 fe。

最终得到 bacdfeg。

5. 复杂度

时间复杂度：O(n)。每个字符最多被处理常数次。

额外空间复杂度：O(1)。字符串按值传入后直接在该字符串上修改，没有创建随输入规模增长的辅助容器。

十、易错点

编译调试版本时忘记加 -g，导致 GDB 缺少源码行号和变量信息。

把 run 和 continue 混淆：前者重新启动，后者从当前位置继续。

把 next 和 step 混淆：前者通常越过函数，后者进入函数。

只看到程序崩溃就猜原因，没有先用 info args、print 和 bt 检查运行现场。

set variable count = 4 只修改当前进程，不会保存到源码。

只在 GDB 中把 count 改成非零并不算真正修复；源码仍需处理零值输入。

delete 1 中的 1 是断点编号，不一定是源码第 1 行。

LeetCode 541 的循环步长应是 2 * k，不是 k。

std::reverse 的右边界不参与反转，因此反转前 k 个字符要写 begin() + i + k，并用 min 限制边界。

reverse 和 min 需要包含 <algorithm>。

Day18 的调试源码和算法代码必须放在 day18/，不要再混入 Day17 的库工程。

十一、今日总结

今天建立了基础但完整的 GDB 调试思路：先用 -g 生成调试信息，通过断点让程序停在关键位置，再用 next、step 控制执行，用 print、info args、info locals 观察数据，用 backtrace 理解调用链，最后通过临时修改变量验证错误判断。

本次没有停留在记忆命令，而是真正定位了 calculateAverage(100, 0) 的除零问题，并验证 count = 4 时结果为 25。算法方面掌握了 LeetCode 541 的分组规律、循环步长和尾部边界处理，为后续更复杂的字符串操作和程序调试打下基础。
