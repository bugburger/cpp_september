# Day19 学习总结

## 一、今日学习目标

今天主要学习 C++ 中非常重要的运行时错误检测工具：

- AddressSanitizer（ASan）
- UndefinedBehaviorSanitizer（UBSan）

并通过实际代码主动制造和定位以下错误：

- heap-buffer-overflow
- heap-use-after-free
- double-free
- signed integer overflow

同时完成 LeetCode 151：

```text
反转字符串中的单词
```

并学习了两种解法：

1. `vector<string>` 保存单词后倒序拼接
2. 双指针去空格 + 整体反转 + 单词局部反转

今天最重要的目标不是记住命令，而是建立：

```text
资源生命周期 + 运行时调试 + 字符串边界处理
```

这三种思维。

---

# 二、为什么需要 Sanitizer

C++ 中一个非常危险的问题是：

```text
程序能正常运行
不等于
程序一定正确
```

例如：

```cpp
int* data = new int[3];

data[0] = 10;
data[1] = 20;
data[2] = 30;

data[3] = 40;
```

其中：

```cpp
data[3]
```

已经发生越界。

但是普通编译运行时，程序有可能仍然输出：

```text
data[3] = 40
```

甚至不会立即崩溃。

因此：

```text
没有崩溃
≠
没有 Bug
```

这也是 ASan、UBSan 这类工具非常重要的原因。

---

# 三、AddressSanitizer（ASan）

## 1. ASan 是什么

ASan 全称：

```text
AddressSanitizer
```

主要用于检测：

```text
内存地址访问相关错误
```

常见包括：

- heap-buffer-overflow
- stack-buffer-overflow
- heap-use-after-free
- double-free
- 部分非法内存访问

---

# 四、ASan 编译方式

最常用命令：

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

各参数含义如下。

---

## 1. `-std=c++17`

指定：

```text
使用 C++17 标准
```

---

## 2. `-Wall`

打开一批常用编译警告。

---

## 3. `-Wextra`

进一步开启更多警告。

实际开发中应该养成：

```text
尽量让编译器警告保持干净
```

的习惯。

---

## 4. `-g`

加入调试信息。

例如 ASan 才能更清楚地报告：

```text
asan_basic.cpp:15
```

告诉我们具体是哪一行出了问题。

同时 GDB 也依赖调试信息查看：

- 源代码
- 变量
- 行号

---

## 5. `-fsanitize=address`

开启：

```text
AddressSanitizer
```

这是 ASan 最关键的参数。

---

## 6. `-fno-omit-frame-pointer`

要求编译器保留 frame pointer。

初学阶段可以简单理解为：

```text
让函数调用栈信息更完整，
方便 Sanitizer 和调试器定位问题。
```

---

# 五、第一个实验：Heap Buffer Overflow

实验代码：

```cpp
int* data = new int[3];

data[0] = 10;
data[1] = 20;
data[2] = 30;

data[3] = 40;
```

---

## 1. 合法下标范围

申请：

```cpp
new int[3]
```

得到三个 `int`：

```text
data[0]
data[1]
data[2]
```

合法下标：

```text
0 ~ 2
```

因此：

```cpp
data[3]
```

属于越界。

---

## 2. 为什么 ASan 显示 12-byte region

常见环境中：

```cpp
sizeof(int) == 4
```

所以：

```text
3 × 4 = 12 bytes
```

因此 ASan 报告：

```text
12-byte region
```

和代码完全对应。

---

## 3. 实际 ASan 报错

今天看到：

```text
ERROR: AddressSanitizer: heap-buffer-overflow
```

表示：

```text
堆内存发生缓冲区越界
```

---

## 4. `WRITE of size 4`

ASan 输出：

```text
WRITE of size 4
```

说明本次非法操作属于：

```text
写内存
```

而且写了：

```text
4 bytes
```

正好对应：

```cpp
data[3] = 40;
```

向非法位置写入一个 `int`。

---

## 5. `0 bytes after 12-byte region`

报告中还有：

```text
0 bytes after 12-byte region
```

这句话容易理解错。

它不是说：

```text
没有越界
```

而是表示：

```text
访问地址恰好从合法内存区域结束后的
第一个字节开始。
```

内存可以理解为：

```text
data[0]    4 bytes
data[1]    4 bytes
data[2]    4 bytes
-------------------
总共        12 bytes

data[3]
↑
从合法区域结束后的第一个位置开始
```

---

# 六、如何快速阅读 ASan 报告

ASan 报告通常很长。

不要从第一行一直机械读到底。

推荐阅读顺序：

```text
1. ERROR 类型
        ↓
2. READ / WRITE
        ↓
3. 自己的 .cpp 文件和行号
        ↓
4. allocated / freed 的位置
```

---

## 1. 先看 ERROR

例如：

```text
heap-buffer-overflow
```

说明：

```text
数组 / 缓冲区越界
```

---

## 2. 再看 READ / WRITE

例如：

```text
WRITE of size 4
```

说明：

```text
非法写
```

例如：

```text
READ of size 4
```

说明：

```text
非法读
```

---

## 3. 找自己的源码

例如：

```text
/home/wangyuchen/cpp_september/day19/asan_basic.cpp:15
```

优先关注：

```text
自己的 .cpp 文件
```

而不是：

```text
libc
asan_new_delete.cpp
_start
```

这些底层运行库信息。

---

## 4. 看 allocated / freed

例如：

```text
allocated by thread T0 here:
```

表示：

```text
这块资源在哪里申请
```

而：

```text
freed by thread T0 here:
```

表示：

```text
这块资源在哪里释放
```

因此真正调试内存问题时，要形成：

```text
资源在哪里创建？
        ↓
资源在哪里释放？
        ↓
为什么这里还在使用？
```

这种生命周期思维。

---

# 七、第二个实验：Heap Use After Free

实验：

```cpp
int* p = new int(100);

delete p;

std::cout << *p;
```

ASan 报告：

```text
ERROR: AddressSanitizer: heap-use-after-free
```

---

# 八、delete 到底做了什么

这一点今天非常重要。

执行：

```cpp
delete p;
```

释放的是：

```text
p 当前指向的堆资源
```

而不是：

```text
指针变量 p 本身
```

---

## delete 前

```text
p
│
│ 地址
▼
┌─────────────┐
│     100     │
└─────────────┘
```

此时：

```cpp
*p
```

合法。

---

## delete 后

```cpp
delete p;
```

内存变成：

```text
p
│
│ 仍然可能保存原地址
▼
X 该资源已经被释放
```

但是：

```text
p 这个变量依然存在
```

---

# 九、什么是悬空指针

如果一个指针仍保存某个地址，但是：

```text
该地址对应的对象已经失效
```

那么这个指针称为：

```text
dangling pointer
```

中文：

```text
悬空指针
```

例如：

```cpp
int* p = new int(100);

delete p;
```

此时 `p` 就可能成为悬空指针。

---

# 十、为什么 delete 后还能打印原地址

今天普通程序运行结果中：

```text
delete 前：
p address = 0x...

delete 后：
p address = 0x...
```

地址完全一样。

原因是：

```cpp
delete p;
```

不会自动执行：

```cpp
p = nullptr;
```

所以：

```text
指针变量中保存的数值
```

仍然可能是原来的地址。

但：

```text
地址还在
≠
资源还有效
```

这是非常重要的区别。

---

# 十一、为什么 delete 后读出来一个奇怪数字

今天普通运行中：

```text
*p = 100
```

删除以后再次读取：

```text
*p = 1347134523
```

这个数字没有业务意义。

原因是：

```text
该内存已经被释放，
程序已经不再拥有它。
```

继续读取属于：

```text
Undefined Behavior
```

所以可能：

```text
仍然读到 100
```

也可能：

```text
读到随机值
```

也可能：

```text
程序崩溃
```

因此：

```text
某一次运行出现什么结果
不能当成 C++ 标准保证的行为。
```

---

# 十二、Use-After-Free 报告怎么看

今天 ASan 给出了：

```text
READ of size 4
```

因为代码：

```cpp
std::cout << *p;
```

是在：

```text
读取 int
```

而常见环境中：

```text
int = 4 bytes
```

所以是：

```text
READ of size 4
```

---

## 完整生命周期

ASan 同时显示：

```text
previously allocated by thread T0 here:
```

表示：

```cpp
new
```

的位置。

随后：

```text
freed by thread T0 here:
```

表示：

```cpp
delete
```

的位置。

最后：

```text
heap-use-after-free
```

告诉我们：

```text
释放以后又访问
```

完整关系：

```text
new
↓
资源创建

delete
↓
资源释放

*p
↓
继续读取已经失效的资源

heap-use-after-free
```

---

# 十三、`delete p; p = nullptr;`

裸指针代码中，经常写：

```cpp
delete p;
p = nullptr;
```

这样做的好处是：

```text
避免 p 自己继续保存已经失效的地址
```

---

## `delete nullptr` 是安全的

C++ 中：

```cpp
delete nullptr;
```

是安全的。

因此：

```cpp
delete p;
p = nullptr;

delete p;
```

第二次实际上相当于：

```cpp
delete nullptr;
```

不会造成同一块资源再次释放。

---

# 十四、但是 nullptr 不能解决所有问题

例如：

```cpp
int* p = new int(100);

int* q = p;

delete p;
p = nullptr;
```

此时：

```text
p = nullptr
```

但是：

```text
q 仍然保存原来的地址
```

所以：

```cpp
*q
```

仍然属于：

```text
use-after-free
```

这就是为什么：

```text
靠程序员手动维护大量裸指针
```

非常容易出错。

---

# 十五、RAII 和智能指针为什么重要

前几天已经学习：

- RAII
- unique_ptr
- shared_ptr
- weak_ptr

今天的 ASan 实验进一步说明这些工具为什么重要。

核心原因：

```text
尽量把资源生命周期交给对象管理，
而不是让程序员手动维护 new / delete。
```

例如：

```cpp
std::unique_ptr<int> p =
    std::make_unique<int>(100);
```

生命周期结束后会自动释放。

减少：

- 忘记 delete
- double-free
- use-after-free
- 所有权不清晰

等风险。

---

# 十六、第三个实验：Double Free

实验：

```cpp
int* p = new int(100);

delete p;

delete p;
```

第一次：

```cpp
delete p;
```

释放资源。

第二次：

```cpp
delete p;
```

再次释放同一块已经释放的资源。

属于：

```text
double-free
```

---

# 十七、普通程序也可能发现 double-free

今天普通程序输出：

```text
free(): double free detected in tcache 2
已中止
```

说明 glibc 的内存分配器检测到了：

```text
重复释放
```

但是需要注意：

```text
不能依赖普通运行环境一定帮我们发现。
```

这种行为本身仍然属于错误行为 / 未定义行为。

ASan 能提供更加完整的定位信息。

---

# 十八、ASan 的 double-free 报告

今天 ASan 报告：

```text
ERROR: AddressSanitizer: attempting double-free
```

并告诉我们：

```text
第二次 delete 在哪里
第一次 delete 在哪里
最初 new 在哪里
```

完整资源生命周期：

```text
new
↓
第一次 delete
↓
资源已经释放
↓
第二次 delete
↓
double-free
```

---

# 十九、Use-After-Free 和 Double-Free 的区别

## Use-After-Free

资源释放以后：

```text
继续读 / 写
```

例如：

```cpp
delete p;

std::cout << *p;
```

属于：

```text
heap-use-after-free
```

---

## Double-Free

资源释放以后：

```text
再次释放
```

例如：

```cpp
delete p;

delete p;
```

属于：

```text
double-free
```

---

# 二十、Shadow Memory / Shadow Bytes

ASan 输出中出现：

```text
Shadow bytes around the buggy address:
```

这是 ASan 内部检测机制的一部分。

初学阶段不用记所有标记。

今天重点认识：

```text
fa
```

和：

```text
fd
```

---

## `fa`

ASan 图例中：

```text
Heap left redzone: fa
```

redzone 可以简单理解成：

```text
ASan 在合法内存周围设置的禁止访问区域
```

如果程序访问到了 redzone：

```text
很可能发生越界
```

---

## `fd`

ASan 图例：

```text
Freed heap region: fd
```

表示：

```text
已经释放的堆内存
```

今天 use-after-free 的报告中就出现了：

```text
fd
```

---

# 二十一、UndefinedBehaviorSanitizer（UBSan）

UBSan 全称：

```text
UndefinedBehaviorSanitizer
```

主要用于检测：

```text
部分 C/C++ Undefined Behavior
```

也就是：

```text
未定义行为
```

---

# 二十二、UBSan 编译方式

常用命令：

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=undefined \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

其中：

```text
-fsanitize=undefined
```

用于开启 UBSan。

---

# 二十三、什么是 Undefined Behavior

Undefined Behavior：

```text
未定义行为
```

简称：

```text
UB
```

简单理解：

```text
C++ 标准没有规定程序在这种情况下
必须产生什么结果。
```

因此可能：

- 看起来正常
- 输出奇怪结果
- 崩溃
- 不同编译器表现不同
- 不同优化等级表现不同

---

## 一个重要原则

```text
某次运行恰好得到某个结果
≠
C++ 保证一定得到这个结果
```

这个原则今天出现了很多次。

例如：

- 数组越界
- use-after-free
- signed integer overflow

---

# 二十四、Signed Integer Overflow

今天实验：

```cpp
int x = std::numeric_limits<int>::max();

x = x + 1;
```

常见 32 位 int：

```text
INT_MAX = 2147483647
```

所以：

```text
2147483647 + 1
```

已经无法继续使用 `int` 表示。

UBSan 报告：

```text
runtime error:
signed integer overflow:
2147483647 + 1 cannot be represented in type 'int'
```

---

# 二十五、为什么普通运行得到 -2147483648

今天普通运行：

```text
2147483647 + 1
```

得到：

```text
-2147483648
```

但不能因此记成：

```text
INT_MAX + 1 = INT_MIN
```

因为 signed integer overflow 属于：

```text
Undefined Behavior
```

所以不能编写依赖这种行为的程序。

---

# 二十六、Signed Overflow 与 Unsigned Overflow

这是今天 UBSan 部分最重要的区别之一。

---

## Signed Integer

例如：

```cpp
int x = INT_MAX;

x = x + 1;
```

属于：

```text
Undefined Behavior
```

---

## Unsigned Integer

例如：

```cpp
unsigned int x = UINT_MAX;

x = x + 1;
```

这是：

```text
定义良好的行为
```

---

# 二十七、Unsigned Wraparound

今天实际输出：

```text
unsigned max = 4294967295
```

再加 1：

```text
unsigned result = 0
```

这种行为称为：

```text
wraparound
```

即：

```text
回绕
```

无符号整数按照模运算处理。

以常见 32 位 unsigned int 为例：

```text
2^32 = 4294967296
```

因此：

```text
4294967295 + 1
= 4294967296

4294967296 mod 4294967296
= 0
```

---

# 二十八、必须记住 signed / unsigned 区别

```text
signed overflow
→ Undefined Behavior

unsigned overflow
→ 定义良好的 wraparound
```

不要把二者混为一谈。

---

# 二十九、为什么 Signed Overflow 被设计成 UB

初学阶段可以这样理解：

C++ 允许编译器假设：

```text
一个正确的程序不会发生 signed integer overflow
```

这样编译器可以做更多优化。

因此：

```text
不要编写依赖 signed overflow 的代码。
```

具体编译器优化细节后续再深入。

---

# 三十、ASan + UBSan 联合使用

实际项目里可以同时打开：

```bash
-fsanitize=address,undefined
```

完整命令：

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

---

# 三十一、今天的综合实验

综合代码先制造：

```text
signed integer overflow
```

再制造：

```text
heap-buffer-overflow
```

实际运行中：

```text
UBSan
↓
报告 signed integer overflow

程序继续执行

ASan
↓
报告 heap-buffer-overflow

程序终止
```

---

## 注意

不能死记：

```text
UBSan 一定继续
ASan 一定退出
```

因为 Sanitizer 的行为可以通过参数调整。

初学阶段只需要知道：

```text
默认情况下，
UBSan 的很多报告可能允许程序继续，
ASan 发现严重内存错误后通常终止程序。
```

---

# 三十二、GDB、ASan、UBSan 的区别

这三个工具不能混淆。

---

## GDB

GDB 是：

```text
Debugger
调试器
```

主要能力：

- break
- run
- next
- step
- print
- backtrace
- 查看变量
- 查看函数调用过程

GDB 更适合回答：

```text
程序执行到这里时变量是多少？
```

```text
程序是怎样一步一步运行到这里的？
```

```text
哪个函数调用了哪个函数？
```

---

## ASan

ASan 主要回答：

```text
程序有没有非法内存访问？
```

例如：

- 越界
- use-after-free
- double-free

---

## UBSan

UBSan 主要回答：

```text
程序有没有发生某些未定义行为？
```

例如：

```text
signed integer overflow
```

---

# 三十三、实际调试思路

实际开发中通常不是：

```text
GDB 和 ASan 二选一
```

而是组合使用。

可以形成：

```text
编译器 Warning
↓
ASan / UBSan
↓
GDB
↓
定位具体逻辑
```

例如：

```text
Sanitizer：
告诉我这里发生 use-after-free

GDB：
进一步观察为什么这个对象提前被释放
```

---

# 三十四、LeetCode 151：反转字符串中的单词

题目示例：

```text
输入：
"  the sky   is blue  "

输出：
"blue is sky the"
```

要求：

```text
1. 去掉开头空格
2. 去掉结尾空格
3. 单词之间多个空格压缩成一个
4. 反转单词顺序
5. 单词内部字符顺序保持不变
```

---

# 三十五、第一种解法：vector 保存单词

基本流程：

```text
扫描字符串
↓
提取每个单词
↓
存入 vector<string>
↓
从后往前遍历 vector
↓
重新拼接
```

---

## 核心代码

```cpp
vector<string> words;
string word;

for (int i = 0; i < s.length(); ++i) {
    if (s[i] != ' ') {
        word += s[i];
    }
    else if (!word.empty()) {
        words.push_back(word);
        word.clear();
    }
}
```

---

# 三十六、为什么需要 `!word.empty()`

假设输入：

```text
"the     sky"
```

中间有很多空格。

如果：

```text
每遇到一个空格
就 push_back(word)
```

就会把很多：

```text
空字符串
```

加入 vector。

所以：

```cpp
else if (!word.empty())
```

表示：

```text
只有当前真的收集到了一个完整单词，
才加入 vector。
```

---

# 三十七、为什么循环结束还要再 push 一次

代码：

```cpp
if (!word.empty()) {
    words.push_back(word);
}
```

很重要。

例如：

```text
"hello world"
```

最后一个：

```text
world
```

后面没有空格。

如果只有：

```text
遇到空格才保存单词
```

那么：

```text
world
```

永远不会被放入 vector。

这是非常常见的字符串扫描边界问题。

---

# 三十八、今天实际出现的错误：`=` 与 `==`

今天最重要的 C++ 易错点之一。

错误代码：

```cpp
if (i = 0)
```

这里：

```text
=
```

表示：

```text
赋值
```

相当于：

```cpp
i = 0;
```

并不是：

```text
判断 i 是否等于 0
```

---

## 正确写法

```cpp
if (i == 0)
```

其中：

```text
==
```

才是：

```text
比较是否相等
```

---

# 三十九、为什么 `if (i = 0)` 会导致循环提前结束

假设：

```text
i = 3
```

执行：

```cpp
if (i = 0)
```

首先执行：

```cpp
i = 0;
```

然后：

```text
整个表达式的值为 0
```

因此：

```cpp
if (0)
```

条件为 false。

随后 for 循环：

```cpp
--i;
```

导致：

```text
i = -1
```

循环直接结束。

因此原来的代码可能只处理：

```text
最后一个单词
```

---

# 四十、字符串拼接的正确方式

可以写：

```cpp
for (int i =
         static_cast<int>(words.size()) - 1;
     i >= 0;
     --i) {

    ans += words[i];

    if (i != 0) {
        ans += ' ';
    }
}
```

核心思路：

```text
先添加当前元素
↓
如果后面还有元素
↓
再添加分隔符
```

这样可以避免：

```text
字符串最后多一个空格
```

---

# 四十一、第一种解法复杂度

设字符串长度为：

```text
n
```

提取所有单词：

```text
O(n)
```

重新拼接：

```text
O(n)
```

总体：

```text
O(n)
```

额外需要：

```cpp
vector<string> words;
```

因此额外空间：

```text
O(n)
```

---

# 四十二、第二种解法：双指针 + 两次反转

第二种解法更加经典。

整体过程：

```text
"  the sky   is blue  "

↓ 去除多余空格

"the sky is blue"

↓ 整体 reverse

"eulb si yks eht"

↓ 每个单词 reverse

"blue is sky the"
```

---

# 四十三、为什么整体反转后再局部反转

原始单词：

```text
the
sky
is
blue
```

整体反转：

```text
eulb
si
yks
eht
```

可以看到：

```text
单词整体顺序
已经反过来了
```

但是：

```text
每个单词内部
也被反转了
```

因此只需要再将每个单词单独反转：

```text
eulb → blue
si   → is
yks  → sky
eht  → the
```

最终：

```text
blue is sky the
```

---

# 四十四、双指针去除多余空格

使用：

```text
fast
```

负责：

```text
读取原字符串
```

使用：

```text
slow
```

负责：

```text
写整理后的字符串
```

这种模式非常经典：

```text
fast = 读指针
slow = 写指针
```

---

# 四十五、跳过连续空格

代码：

```cpp
while (fast < static_cast<int>(s.size()) &&
       s[fast] == ' ') {

    ++fast;
}
```

作用：

```text
一次跳过所有连续空格
```

因此可以处理：

```text
开头空格
单词之间多个空格
结尾空格
```

---

# 四十六、为什么需要判断 `fast >= s.size()`

跳过空格以后：

```cpp
if (fast >= static_cast<int>(s.size())) {
    break;
}
```

是为了处理：

```text
字符串末尾全部都是空格
```

例如：

```text
"hello     "
```

跳完空格后：

```text
fast == s.size()
```

说明：

```text
已经没有新的单词
```

直接结束。

---

# 四十七、为什么 `slow != 0` 才添加空格

代码：

```cpp
if (slow != 0) {
    s[slow++] = ' ';
}
```

含义：

```text
如果前面已经写过至少一个单词，
那么新单词前面加一个空格。
```

---

## 第一个单词

例如：

```text
hello
```

此时：

```text
slow == 0
```

因此不会在字符串最前面添加空格。

---

## 第二个单词以后

例如：

```text
world
```

此时：

```text
slow != 0
```

先写：

```text
' '
```

然后再写：

```text
world
```

所以自然得到：

```text
hello world
```

---

# 四十八、复制当前单词

代码：

```cpp
while (fast < static_cast<int>(s.size()) &&
       s[fast] != ' ') {

    s[slow++] = s[fast++];
}
```

含义：

```text
只要当前字符不是空格，
就从 fast 读，
写到 slow。
```

这是典型的：

```text
原地双指针覆盖
```

---

# 四十九、为什么需要 `s.resize(slow)`

双指针只是：

```text
把正确结果写到了字符串前面
```

但是原字符串后面仍然可能残留旧字符。

所以：

```cpp
s.resize(slow);
```

把字符串长度截断为：

```text
真正有效的长度
```

---

# 五十、整体反转

整理完成：

```text
the sky is blue
```

执行：

```cpp
reverse(s.begin(), s.end());
```

得到：

```text
eulb si yks eht
```

---

# 五十一、逐个反转每个单词

维护：

```cpp
int start = 0;
```

`start` 表示：

```text
当前单词起始位置
```

扫描：

```cpp
for (int i = 0;
     i < static_cast<int>(s.size());
     ++i) {

    if (s[i] == ' ') {
        reverse(s.begin() + start,
                s.begin() + i);

        start = i + 1;
    }
}
```

当：

```text
遇到空格
```

说明当前单词结束。

因此反转：

```text
[start, i)
```

这一段。

---

# 五十二、为什么最后一个单词要单独处理

循环中的逻辑依赖：

```text
遇到空格
```

才处理一个单词。

但是最后一个单词：

```text
后面没有空格
```

所以循环结束后必须：

```cpp
reverse(s.begin() + start, s.end());
```

单独处理最后一个单词。

这个问题和第一种解法中的：

```cpp
if (!word.empty()) {
    words.push_back(word);
}
```

本质是同一种边界问题：

```text
最后一个元素后面
没有分隔符
```

---

# 五十三、LeetCode 151 最终代码

```cpp
class Solution {
public:
    string reverseWords(string s) {
        int slow = 0;

        // 第一步：去除多余空格
        for (int fast = 0;
             fast < static_cast<int>(s.size()); ) {

            // 跳过连续空格
            while (fast <
                       static_cast<int>(s.size()) &&
                   s[fast] == ' ') {
                ++fast;
            }

            // 已经扫描到字符串结尾
            if (fast >=
                static_cast<int>(s.size())) {
                break;
            }

            // 单词之间补一个空格
            if (slow != 0) {
                s[slow++] = ' ';
            }

            // 复制当前单词
            while (fast <
                       static_cast<int>(s.size()) &&
                   s[fast] != ' ') {

                s[slow++] = s[fast++];
            }
        }

        // 删除后面残留字符
        s.resize(slow);

        // 第二步：整体反转
        reverse(s.begin(), s.end());

        // 第三步：逐个反转单词
        int start = 0;

        for (int i = 0;
             i < static_cast<int>(s.size());
             ++i) {

            if (s[i] == ' ') {

                reverse(
                    s.begin() + start,
                    s.begin() + i
                );

                start = i + 1;
            }
        }

        // 最后一个单词
        reverse(
            s.begin() + start,
            s.end()
        );

        return s;
    }
};
```

---

# 五十四、第二种解法复杂度

设字符串长度为：

```text
n
```

去空格：

```text
O(n)
```

整体反转：

```text
O(n)
```

逐单词反转：

```text
O(n)
```

所以总时间复杂度：

```text
O(n)
```

算法本身没有额外创建：

```text
vector<string>
```

因此如果：

```text
不计算传入 string 自身及其内部存储
```

额外辅助空间可以看作：

```text
O(1)
```

---

# 五十五、两种解法比较

## 方法一：vector

优点：

```text
容易理解
容易实现
边界清晰
```

缺点：

```text
需要额外 O(n) 空间
```

---

## 方法二：双指针 + reverse

优点：

```text
额外辅助空间更小
能练习双指针
能练习字符串原地修改
能练习 reverse
```

难点：

```text
边界情况更多
```

特别是：

- 开头空格
- 结尾空格
- 连续空格
- 最后一个单词
- slow / fast 的含义

---

# 五十六、今天所有易错点总结

## 易错点 1：程序没崩不代表没有内存错误

数组越界普通运行时可能表现正常。

所以：

```text
能运行
≠
代码正确
```

---

## 易错点 2：delete 不会删除指针变量

```cpp
delete p;
```

释放的是：

```text
p 指向的资源
```

不是：

```text
p 变量本身
```

---

## 易错点 3：delete 后 p 不会自动变 nullptr

```cpp
delete p;
```

之后：

```text
p 仍可能保存原地址
```

因此容易形成：

```text
dangling pointer
```

---

## 易错点 4：悬空指针不能解引用

```cpp
delete p;

*p;
```

属于：

```text
use-after-free
```

---

## 易错点 5：同一资源不能释放两次

```cpp
delete p;

delete p;
```

属于：

```text
double-free
```

---

## 易错点 6：`p = nullptr` 只修改 p

如果：

```cpp
q = p;
```

之后：

```cpp
delete p;
p = nullptr;
```

`q` 仍然可能悬空。

---

## 易错点 7：READ 和 WRITE 要区分

```cpp
*p
```

主要是：

```text
READ
```

而：

```cpp
*p = 100;
```

主要是：

```text
WRITE
```

---

## 易错点 8：先找自己代码，不要被底层库吓到

ASan 报告中会出现大量：

```text
libc
asan_new_delete.cpp
_start
```

初学阶段优先找：

```text
自己的 cpp 文件和行号
```

---

## 易错点 9：signed overflow 和 unsigned overflow 不一样

```text
signed overflow
→ Undefined Behavior

unsigned overflow
→ wraparound
```

---

## 易错点 10：不要依赖 UB 的偶然结果

例如：

```text
INT_MAX + 1
```

某次运行得到：

```text
INT_MIN
```

不代表：

```text
C++ 保证这样做
```

---

## 易错点 11：`=` 与 `==`

```cpp
i = 0;
```

表示：

```text
赋值
```

```cpp
i == 0;
```

表示：

```text
比较
```

写条件判断时必须高度警惕。

---

## 易错点 12：字符串分隔符边界

很多字符串扫描算法都采用：

```text
遇到分隔符
→ 处理一个元素
```

但是：

```text
最后一个元素后面
通常没有分隔符
```

所以经常需要：

```text
循环结束后
再处理最后一个元素
```

---

## 易错点 13：不要多加最后一个空格

拼接字符串时推荐：

```cpp
ans += words[i];

if (i != 0) {
    ans += ' ';
}
```

而不是：

```text
每个单词后面无脑加空格
```

---

## 易错点 14：`size()` 是无符号类型

例如：

```cpp
s.size()
words.size()
```

返回：

```text
size_t
```

通常是：

```text
无符号整数类型
```

因此：

```cpp
int i = words.size() - 1;
```

存在 signed / unsigned 类型混用问题。

可以显式：

```cpp
static_cast<int>(words.size())
```

---

## 易错点 15：不要写无符号倒序循环

如果写：

```cpp
for (size_t i = words.size() - 1;
     i >= 0;
     --i)
```

这是危险写法。

因为：

```text
size_t 是无符号类型
```

不会小于 0。

当 `i == 0` 再执行：

```cpp
--i
```

会发生无符号回绕。

---

## 易错点 16：`reverse` 的右边界不包含

例如：

```cpp
reverse(
    s.begin() + start,
    s.begin() + i
);
```

反转的是：

```text
[start, i)
```

即：

```text
包含 start
不包含 i
```

这和 STL 常见的：

```text
左闭右开
```

区间规则一致。

---

# 五十七、今天应该能够口述的问题

## 1. ASan 是什么？

ASan 是 AddressSanitizer，是一个运行时内存错误检测工具。

主要用于发现：

- 数组越界
- use-after-free
- double-free
- 一些非法内存访问

---

## 2. UBSan 是什么？

UBSan 是 UndefinedBehaviorSanitizer。

主要用于发现部分：

```text
Undefined Behavior
```

例如：

```text
signed integer overflow
```

---

## 3. GDB 和 ASan 有什么区别？

GDB 是调试器：

```text
断点
单步
查看变量
查看调用栈
```

ASan 是自动检测工具：

```text
检测非法内存访问
```

二者可以配合使用。

---

## 4. `delete p` 后 p 会消失吗？

不会。

`delete p`：

```text
释放 p 指向的资源
```

但是：

```text
p 变量本身仍然存在
```

---

## 5. 什么是悬空指针？

指针仍然保存一个地址，但是：

```text
该地址对应的对象或资源已经失效
```

这样的指针叫：

```text
dangling pointer
```

---

## 6. Use-After-Free 是什么？

资源已经释放以后：

```text
再次读取或写入
```

例如：

```cpp
delete p;

std::cout << *p;
```

---

## 7. Double-Free 是什么？

同一块动态资源：

```text
被释放两次
```

例如：

```cpp
delete p;
delete p;
```

---

## 8. 为什么 `delete p; p = nullptr;` 更安全？

因为可以避免：

```text
p 本身继续保存已经失效的地址
```

但是不能解决：

```text
其他别名指针仍指向旧资源
```

的问题。

---

## 9. 什么是 Undefined Behavior？

C++ 标准没有规定：

```text
程序必须产生什么结果
```

的一类行为。

---

## 10. Signed overflow 和 unsigned overflow 有什么区别？

```text
signed integer overflow
→ Undefined Behavior

unsigned integer overflow
→ 按模运算回绕
```

---

## 11. LeetCode 151 原地解法是什么？

```text
双指针去除多余空格
↓
整体 reverse
↓
逐个 reverse 每个单词
```

---

# 五十八、今日命令复习

## ASan

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

---

## UBSan

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=undefined \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

---

## ASan + UBSan

```bash
g++ -std=c++17 \
    -Wall -Wextra \
    -g \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    test.cpp \
    -o test
```

---

# 五十九、今日代码文件

今天完成：

```text
asan_basic.cpp
```

用于：

```text
heap-buffer-overflow
```

---

```text
asan_use_after_free.cpp
```

用于：

```text
heap-use-after-free
```

---

```text
asan_double_free.cpp
```

用于：

```text
double-free
```

---

```text
ubsan_basic.cpp
```

用于：

```text
signed integer overflow
```

---

```text
ubsan_signed_unsigned.cpp
```

用于：

```text
比较 signed overflow
与 unsigned wraparound
```

---

```text
sanitizer_practice.cpp
```

用于：

```text
ASan + UBSan 综合实验
```

---

```text
reverse_words.cpp
```

用于：

```text
LeetCode 151
反转字符串中的单词
```

---

# 六十、今日知识串联

今天的内容不是独立知识。

它和前几天已经学习的：

```text
new / delete
↓
深拷贝 / 浅拷贝
↓
RAII
↓
unique_ptr
↓
shared_ptr
↓
weak_ptr
↓
移动语义
↓
Rule of Five
```

是连在一起的。

前几天主要学习：

```text
如何正确管理资源
```

今天则进一步学习：

```text
资源管理出错以后，
如何利用工具发现问题。
```

所以目前已经形成：

```text
理解资源生命周期
        ↓
使用 RAII 减少错误
        ↓
使用 Sanitizer 检测错误
        ↓
使用 GDB 深入调试
```

这是一条非常重要的 C++ 工程学习主线。

---

# 六十一、Day19 最终验收

- [x] 理解 ASan 的作用
- [x] 会使用 `-fsanitize=address`
- [x] 理解 `-g`
- [x] 理解 `-fno-omit-frame-pointer`
- [x] 复现 heap-buffer-overflow
- [x] 会阅读 READ / WRITE
- [x] 会查源码行号
- [x] 会查 allocated 位置
- [x] 会查 freed 位置
- [x] 复现 heap-use-after-free
- [x] 理解 dangling pointer
- [x] 理解 `delete p` 不会让 p 消失
- [x] 理解 `p = nullptr`
- [x] 理解别名指针问题
- [x] 复现 double-free
- [x] 理解 use-after-free 与 double-free 区别
- [x] 初步认识 Shadow Bytes
- [x] 认识 `fa`
- [x] 认识 `fd`
- [x] 理解 UBSan
- [x] 会使用 `-fsanitize=undefined`
- [x] 理解 Undefined Behavior
- [x] 复现 signed integer overflow
- [x] 理解 signed overflow 不能依赖回绕
- [x] 理解 unsigned wraparound
- [x] 理解 signed / unsigned overflow 区别
- [x] 会联合使用 ASan + UBSan
- [x] 能解释 GDB、ASan、UBSan 的区别
- [x] 完成 LeetCode 151
- [x] 掌握 vector 解法
- [x] 掌握双指针去空格
- [x] 掌握整体 reverse
- [x] 掌握逐单词 reverse
- [x] 理解最后一个单词的边界处理
- [x] 修正 `i = 0` 与 `i == 0` 错误
- [x] 理解 `size_t` 与 `int` 的区别
- [x] 能分析两种算法的时间与空间复杂度

---

# 六十二、Day19 核心总结

今天最需要真正记住的不是命令，而是下面几句话。

```text
1. 程序能运行不代表程序没有内存错误。

2. delete 释放的是指针指向的资源，
   而不是指针变量本身。

3. 资源释放以后继续访问，
   是 use-after-free。

4. 同一块资源释放两次，
   是 double-free。

5. ASan 主要用于发现内存访问错误。

6. UBSan 主要用于发现部分未定义行为。

7. signed integer overflow 是 Undefined Behavior。

8. unsigned integer overflow 按模运算回绕。

9. Sanitizer 用来快速发现问题，
   GDB 用来深入观察程序执行状态。

10. 字符串反转单词的经典套路是：

    去多余空格
        ↓
    整体反转
        ↓
    单词局部反转

11. 条件判断中：
    = 是赋值，
    == 才是比较。

12. 字符串扫描时必须特别注意
    最后一个元素没有分隔符的边界情况。
```

Day19 完成。
