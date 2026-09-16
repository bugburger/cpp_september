# Day18 学习总结

## 今日学习内容

今天主要学习了 Linux 下的 GDB 调试工具，包括生成调试信息、启动 GDB、设置断点、运行程序、查看源代码、单步执行、进入函数、查看变量、查看调用栈、管理断点，以及在调试过程中临时修改变量。

实践中使用 GDB 定位了 `calculateAverage` 函数中的除零错误。程序将 `count` 设置为 `0`，导致计算平均值时出现问题。通过 GDB 将 `count` 临时修改为 `4` 后，程序成功输出 `average = 25`。

算法部分完成了 LeetCode 541「反转字符串 II」，掌握了每隔 `2k` 个字符，反转前 `k` 个字符的处理方法。

---

## 一、Day18 文件结构

```text
day18/
├── day18.md
├── debug_basic.cpp
├── debug_bug.cpp
└── reverse_string_ii.cpp
```

各文件作用：

- `debug_basic.cpp`：练习 GDB 的基本命令；
- `debug_bug.cpp`：使用 GDB 定位除零错误；
- `reverse_string_ii.cpp`：完成反转字符串 II；
- `day18.md`：记录当天知识点和易错点。

---

## 二、什么是 GDB

GDB 全称为：

```text
GNU Debugger
```

它是 Linux 环境下常用的程序调试工具。

GDB 可以帮助我们：

- 在指定位置暂停程序；
- 一行一行执行代码；
- 查看变量当前的值；
- 查看函数接收到的参数；
- 进入函数内部；
- 查看函数调用栈；
- 临时修改变量；
- 分析程序崩溃原因；
- 验证错误判断是否正确。

普通运行只能看到程序最终结果，而 GDB 可以观察程序执行过程中的状态。

---

## 三、为什么编译时要添加 `-g`

调试程序时使用：

```bash
g++ -std=c++17 -Wall -Wextra -g debug_basic.cpp -o debug_basic
```

以及：

```bash
g++ -std=c++17 -Wall -Wextra -g debug_bug.cpp -o debug_bug
```

其中：

- `-std=c++17`：使用 C++17 标准；
- `-Wall`：打开常见警告；
- `-Wextra`：打开更多额外警告；
- `-g`：在可执行文件中加入调试信息；
- `-o`：指定生成的可执行文件名称。

`-g` 会让 GDB 获取：

- 源文件名称；
- 源代码行号；
- 函数名称；
- 函数参数名称；
- 局部变量名称；
- 调用栈信息。

如果没有添加 `-g`，程序仍然可能运行，也可以被 GDB 打开，但很多源代码和变量信息无法正常显示。

---

## 四、GDB 基本调试流程

一个完整的调试流程可以概括为：

```text
使用 -g 编译程序
        ↓
使用 gdb 打开程序
        ↓
设置断点
        ↓
运行到断点
        ↓
单步执行代码
        ↓
查看变量和函数参数
        ↓
查看调用栈
        ↓
继续运行或退出
```

启动 GDB：

```bash
gdb ./debug_basic
```

退出 GDB：

```gdb
quit
```

也可以简写为：

```gdb
q
```

---

## 五、设置断点

### 1. 在函数处设置断点

```gdb
break main
```

简写：

```gdb
b main
```

也可以在其他函数设置断点：

```gdb
break calculateAverage
```

程序运行到该函数时就会暂停。

---

### 2. 在指定行设置断点

```gdb
break debug_bug.cpp:6
```

表示在 `debug_bug.cpp` 的第 6 行设置断点。

也可以直接使用行号：

```gdb
break 6
```

但在多个源文件的项目中，写出文件名更加明确。

---

### 3. 查看断点

```gdb
info breakpoints
```

可以看到：

- 断点编号；
- 是否启用；
- 断点所在地址；
- 对应函数和代码行。

---

### 4. 删除断点

删除编号为 1 的断点：

```gdb
delete 1
```

删除所有断点：

```gdb
delete
```

需要注意：

```text
delete 1
```

中的 `1` 是断点编号，不是源代码行号。

---

## 六、运行和继续执行程序

### 1. run

```gdb
run
```

简写：

```gdb
r
```

`run` 会从程序入口重新启动程序，直到：

- 遇到断点；
- 程序发生异常；
- 程序正常结束。

如果程序已经运行过，再次执行 `run` 会重新启动程序。

---

### 2. continue

```gdb
continue
```

简写：

```gdb
c
```

`continue` 会从当前暂停位置继续运行，直到遇到下一个断点、异常或程序结束。

区别如下：

| 命令 | 作用 |
|---|---|
| `run` | 从头重新启动程序 |
| `continue` | 从当前位置继续执行 |

这是 GDB 中非常容易混淆的一组命令。

---

## 七、查看源代码

使用：

```gdb
list
```

简写：

```gdb
l
```

它会显示当前位置附近的源代码。

查看指定函数：

```gdb
list calculateAverage
```

查看指定代码行附近：

```gdb
list 10
```

设置断点后使用 `list`，可以确认程序当前停在什么位置。

---

## 八、next 与 step

### 1. next

```gdb
next
```

简写：

```gdb
n
```

`next` 会执行当前这一行代码。

如果当前行调用了一个函数，`next` 通常会执行完整个函数，然后停在下一行，不进入函数内部。

---

### 2. step

```gdb
step
```

简写：

```gdb
s
```

如果当前行调用了一个带调试信息的函数，`step` 会进入函数内部。

二者区别如下：

| 命令 | 遇到函数调用时 |
|---|---|
| `next` | 执行完整个函数，不进入内部 |
| `step` | 进入函数内部继续调试 |

使用原则：

- 只关心当前函数的流程：使用 `next`；
- 怀疑被调用函数内部有问题：使用 `step`。

---

## 九、查看变量

使用 `print` 查看变量：

```gdb
print total
print count
```

简写：

```gdb
p total
p count
```

也可以计算表达式：

```gdb
print total + count
print total / count
```

`print` 显示的是程序当前暂停位置中变量的真实值。

需要注意，如果变量还没有执行初始化语句，它的值可能没有意义。

---

## 十、查看函数参数和局部变量

### 1. 查看函数参数

```gdb
info args
```

进入 `calculateAverage` 后执行该命令，可以看到：

```text
total = 100
count = 0
```

这直接说明程序将 `0` 作为除数传入了函数。

---

### 2. 查看局部变量

```gdb
info locals
```

该命令会显示当前函数中的局部变量。

如果函数刚刚进入，部分局部变量可能还没有完成初始化，因此分析变量时必须结合程序当前停留的位置。

---

## 十一、查看函数调用栈

使用：

```gdb
backtrace
```

简写：

```gdb
bt
```

调用栈会显示：

- 当前在哪个函数；
- 当前函数由谁调用；
- 上一层函数又由谁调用。

例如：

```text
calculateAverage
main
```

说明当前位于 `calculateAverage`，这个函数由 `main` 调用。

当程序调用层次比较复杂时，`backtrace` 可以帮助定位错误是从哪一层传递过来的。

---

## 十二、使用 GDB 定位除零错误

问题代码的核心逻辑为：

```cpp
int total = 100;
int count = 0;

double average = calculateAverage(total, count);
```

`calculateAverage` 需要用 `count` 作为除数。

当：

```cpp
count == 0
```

程序就会出现除零问题。

---

### 1. 设置断点并运行

```gdb
break calculateAverage
run
```

程序会在进入 `calculateAverage` 时暂停。

---

### 2. 查看参数

```gdb
info args
```

输出：

```text
total = 100
count = 0
```

因此可以判断：

```text
错误原因是 count 为 0
```

---

### 3. 查看调用栈

```gdb
backtrace
```

可以确认 `count = 0` 是由 `main` 函数传入的。

---

### 4. 临时修改变量

在 GDB 中执行：

```gdb
set variable count = 4
```

然后检查：

```gdb
print count
```

此时得到：

```text
count = 4
```

继续运行：

```gdb
continue
```

程序输出：

```text
average = 25
```

并正常退出。

这证明当前错误确实由 `count = 0` 引起。

---

## 十三、临时修改变量不等于修复源码

GDB 中执行：

```gdb
set variable count = 4
```

只会修改当前正在运行的进程。

它不会修改：

```text
debug_bug.cpp
```

退出 GDB 后，源文件中的 `count` 仍然是 `0`。

因此，临时修改变量的主要用途是：

- 验证对错误原因的判断；
- 测试不同输入下程序的行为；
- 避免每次都修改、编译源代码。

真正修复时，需要修改源代码。

例如：

```cpp
double calculateAverage(int total, int count) {
    if (count == 0) {
        throw std::invalid_argument("count cannot be zero");
    }

    return static_cast<double>(total) / count;
}
```

这里还需要注意：

```cpp
static_cast<double>(total)
```

可以避免整数除法导致小数部分丢失。

---

## 十四、LeetCode 541：反转字符串 II

### 1. 题目规律

从字符串开头开始，每 `2k` 个字符作为一组：

- 反转每组前 `k` 个字符；
- 后 `k` 个字符保持不变。

尾部需要分情况：

1. 剩余字符少于 `k` 个：全部反转；
2. 剩余字符不少于 `k`、少于 `2k`：只反转前 `k` 个；
3. 剩余字符不少于 `2k`：按照正常分组处理。

---

### 2. 本次完成的代码

```cpp
class Solution {
public:
    string reverseStr(string s, int k) {
        int n = s.length();

        if (n < k) {
            reverse(s.begin(), s.end());
        } else {
            for (int i = 0; i < n; i += 2 * k) {
                reverse(
                    s.begin() + i,
                    s.begin() + min(i + k, n)
                );
            }
        }

        return s;
    }
};
```

这段代码的逻辑是正确的。

---

## 十五、为什么步长是 `2 * k`

每一组包含 `2k` 个字符：

```text
前 k 个字符：反转
后 k 个字符：不变
```

因此，处理完一组之后，下一个需要反转的位置是：

```cpp
i += 2 * k;
```

如果误写成：

```cpp
i += k;
```

下一轮会处理原本应该保持不变的后一半字符，导致结果错误。

---

## 十六、为什么使用 min

代码中使用：

```cpp
min(i + k, n)
```

`std::reverse` 操作的是左闭右开区间：

```text
[first, last)
```

正常情况下需要反转：

```text
[i, i + k)
```

但是最后一组可能不足 `k` 个字符。

如果直接使用：

```cpp
s.begin() + i + k
```

就可能超过字符串末尾，产生越界问题。

使用：

```cpp
min(i + k, n)
```

可以保证右边界最大只能到达：

```cpp
s.end()
```

---

## 十七、可以进一步简化代码

原代码中的判断：

```cpp
if (n < k) {
    reverse(s.begin(), s.end());
}
```

是正确的，但不是必须的。

可以简化为：

```cpp
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
```

当 `n < k` 时：

```cpp
min(i + k, n)
```

会自动得到 `n`，因此会执行：

```cpp
reverse(s.begin(), s.end());
```

统一循环已经能够覆盖特殊情况。

---

## 十八、示例分析

假设：

```text
s = "abcdefg"
k = 2
```

第一次循环：

```text
i = 0
反转 [0, 2)
ab → ba
```

字符串变为：

```text
bacdefg
```

第二次循环：

```text
i = 4
反转 [4, 6)
ef → fe
```

最终结果：

```text
bacdfeg
```

---

## 十九、复杂度分析

时间复杂度：

```text
O(n)
```

每个字符最多被处理常数次。

算法辅助空间复杂度：

```text
O(1)
```

只使用少量变量，并直接在字符串上进行交换。

函数参数 `string s` 按值传入，因此调用函数时会产生一份字符串副本，这是题目接口带来的参数复制；算法内部没有额外创建另一个同等规模的辅助字符串。

---

## 二十、今日易错点

1. 编译调试版本时忘记添加 `-g`。
2. 只使用 GDB 打开程序，却没有设置断点。
3. 混淆 `run` 和 `continue`。
4. 混淆 `next` 和 `step`。
5. 变量还没有初始化时就解释它的值。
6. 程序出错后只猜测原因，没有查看实际运行状态。
7. 忘记使用 `info args` 检查函数参数。
8. 忘记使用 `backtrace` 检查调用关系。
9. 把断点编号误认为源代码行号。
10. 认为 `set variable` 会修改源代码。
11. 在 GDB 中临时改值成功，不代表源码已经修复。
12. 除法前没有检查除数是否为零。
13. 两个整数直接相除可能发生整数除法。
14. LeetCode 541 的循环步长应该是 `2 * k`，不是 `k`。
15. `std::reverse` 使用左闭右开区间。
16. 最后一组字符必须使用 `min` 防止越界。
17. 使用 `reverse` 和 `min` 时需要包含 `<algorithm>`。
18. Day18 的调试代码不要放入 Day17 的库工程。

---

## 二十一、GDB 常用命令汇总

| 命令 | 简写 | 作用 |
|---|---|---|
| `break main` | `b main` | 在函数处设置断点 |
| `break 文件:行号` | `b 文件:行号` | 在指定代码行设置断点 |
| `run` | `r` | 从头运行程序 |
| `continue` | `c` | 从当前位置继续运行 |
| `next` | `n` | 单步执行，不进入函数 |
| `step` | `s` | 单步执行，进入函数 |
| `print variable` | `p variable` | 查看变量 |
| `list` | `l` | 查看源代码 |
| `backtrace` | `bt` | 查看调用栈 |
| `info args` | 无 | 查看函数参数 |
| `info locals` | 无 | 查看局部变量 |
| `info breakpoints` | 无 | 查看断点 |
| `delete 编号` | `d 编号` | 删除断点 |
| `set variable x=值` | 无 | 临时修改变量 |
| `quit` | `q` | 退出 GDB |

---

## 二十二、今日总结

今天建立了一套完整的 GDB 基础调试流程：

1. 使用 `-g` 编译程序；
2. 使用 GDB 打开可执行文件；
3. 在关键函数或代码行设置断点；
4. 使用 `run` 运行到断点；
5. 使用 `next` 和 `step` 控制执行；
6. 使用 `print` 查看变量；
7. 使用 `info args` 查看函数参数；
8. 使用 `info locals` 查看局部变量；
9. 使用 `backtrace` 查看调用链；
10. 临时修改变量验证错误判断；
11. 回到源代码中完成真正修复。

本次通过实际调试定位了 `count = 0` 导致的除零问题，并通过将 `count` 临时修改为 `4`，验证程序能够正确输出 `average = 25`。

算法部分掌握了 LeetCode 541 的分组规律、`2k` 步长、左闭右开区间以及尾部边界处理方法。
