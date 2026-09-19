# Day21 学习总结

## 今日学习内容

今天主要学习了 Linux 文件描述符和文件 IO 系统调用，并使用 `open()`、`read()`、`write()`、`close()` 完成了文件复制程序。

算法部分完成了 LeetCode 232「用栈实现队列」。

主要内容：

1. 文件描述符 `fd`
2. `open()` 打开文件
3. `read()` 读取数据
4. `write()` 写入数据
5. `close()` 关闭文件
6. EOF 与循环读取
7. partial write 部分写入
8. 使用 `cmp` 验证文件复制
9. 栈和队列的区别
10. 使用两个栈实现队列
11. 默认构造函数 `= default`

---

## 一、文件描述符

Linux 打开文件后，会返回一个整数：

```cpp
int fd = open("test.txt", O_RDONLY);
```

这个整数称为：

```text
File Descriptor
文件描述符
```

文件描述符的类型是：

```cpp
int
```

不是：

```cpp
FILE*
```

需要区分：

```text
open()  → int fd
fopen() → FILE*
```

Linux 程序启动时，通常已经存在三个文件描述符：

```text
0 → stdin  标准输入
1 → stdout 标准输出
2 → stderr 标准错误
```

所以第一次打开其他文件时，经常得到：

```text
fd = 3
```

但 `3` 不是固定值，程序不能依赖具体编号。

---

## 二、`open()`

使用 `open()` 需要：

```cpp
#include <fcntl.h>
```

只读打开：

```cpp
int fd = open("test.txt", O_RDONLY);
```

常见标志：

```text
O_RDONLY → 只读
O_WRONLY → 只写
O_RDWR   → 可读可写
O_CREAT  → 文件不存在时创建
O_TRUNC  → 文件存在时清空旧内容
```

创建目标文件：

```cpp
int destinationFd = open(
    "destination.txt",
    O_WRONLY | O_CREAT | O_TRUNC,
    0644
);
```

这里的：

```cpp
|
```

是按位或，用来组合多个标志，不是逻辑或 `||`。

`0644` 通常表示：

```text
文件所有者：可读、可写
同组用户：可读
其他用户：可读
```

`open()` 成功时返回大于或等于 0 的文件描述符，失败时返回：

```text
-1
```

正确判断：

```cpp
if (fd == -1) {
    std::cerr << "open failed" << std::endl;
}
```

不能简单写成：

```cpp
if (fd) {
}
```

---

## 三、`close()`

使用 `close()` 需要：

```cpp
#include <unistd.h>
```

基本写法：

```cpp
close(fd);
```

`close()` 表示当前进程不再使用这个文件描述符。

文件描述符关闭后，对应的整数编号可能被后面的 `open()` 再次使用。

`open()` 和 `close()` 也体现了资源管理过程：

```text
open()  → 获取资源
close() → 释放资源
```

这和之前学习的 RAII 思想有关。以后可以用类管理文件描述符，在析构函数中自动调用 `close()`。

---

## 四、`read()`

基本写法：

```cpp
char buffer[128];

ssize_t bytesRead = read(
    fd,
    buffer,
    sizeof(buffer)
);
```

参数含义：

```text
fd             → 从哪个文件读取
buffer         → 数据保存到哪里
sizeof(buffer) → 本次最多读取多少字节
```

数据流向：

```text
文件描述符 → read() → buffer
```

`read()` 返回值：

```text
> 0  → 本次实际读取的字节数
= 0  → EOF，文件读取结束
= -1 → 读取失败
```

例如：

```cpp
if (bytesRead == -1) {
    std::cerr << "read failed" << std::endl;
}

if (bytesRead == 0) {
    break;
}
```

`read()` 读取的是原始字节，不会自动添加：

```cpp
'\0'
```

因此不能直接假设 `buffer` 是合法的 C 风格字符串。

---

## 五、为什么需要循环读取

假设缓冲区大小是：

```cpp
char buffer[128];
```

而文件大小是：

```text
1791 bytes
```

一次 `read()` 最多读取 128 字节，所以必须循环读取：

```cpp
while (true) {
    ssize_t bytesRead = read(
        sourceFd,
        buffer,
        sizeof(buffer)
    );

    if (bytesRead == -1) {
        // 读取失败
    }

    if (bytesRead == 0) {
        break;
    }
}
```

当 `read()` 返回 0 时，表示到达 EOF。

`break` 只结束当前循环，不会直接结束整个程序。跳出循环后仍然需要关闭文件描述符。

---

## 六、`write()`

基本写法：

```cpp
write(fd, buffer, bytesRead);
```

数据流向：

```text
buffer → write() → 文件描述符
```

必须使用：

```cpp
bytesRead
```

作为写入长度，不能直接使用：

```cpp
sizeof(buffer)
```

因为缓冲区可能有 128 字节，但本轮可能只读取了 22 字节。

正确：

```cpp
write(fd, buffer, bytesRead);
```

错误：

```cpp
write(fd, buffer, sizeof(buffer));
```

---

## 七、partial write

一次 `write()` 不保证把请求的数据全部写完，这种情况叫：

```text
Partial Write
部分写入
```

因此需要记录已经写出的字节数：

```cpp
ssize_t totalWritten = 0;

while (totalWritten < bytesRead) {
    ssize_t bytesWritten = write(
        destinationFd,
        buffer + totalWritten,
        bytesRead - totalWritten
    );

    if (bytesWritten == -1) {
        std::cerr << "write failed" << std::endl;
        return 1;
    }

    totalWritten += bytesWritten;
}
```

其中：

```cpp
buffer + totalWritten
```

表示从尚未写出的数据位置继续写。

```cpp
bytesRead - totalWritten
```

表示这一轮还剩多少字节没有写出。

---

## 八、完整文件复制程序

```cpp
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

int main() {
    int sourceFd = open("source.txt", O_RDONLY);

    if (sourceFd == -1) {
        std::cerr << "open source failed" << std::endl;
        return 1;
    }

    int destinationFd = open(
        "destination.txt",
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (destinationFd == -1) {
        std::cerr << "open destination failed" << std::endl;
        close(sourceFd);
        return 1;
    }

    char buffer[128];

    while (true) {
        ssize_t bytesRead = read(
            sourceFd,
            buffer,
            sizeof(buffer)
        );

        if (bytesRead == -1) {
            std::cerr << "read failed" << std::endl;
            close(sourceFd);
            close(destinationFd);
            return 1;
        }

        if (bytesRead == 0) {
            break;
        }

        ssize_t totalWritten = 0;

        while (totalWritten < bytesRead) {
            ssize_t bytesWritten = write(
                destinationFd,
                buffer + totalWritten,
                bytesRead - totalWritten
            );

            if (bytesWritten == -1) {
                std::cerr << "write failed" << std::endl;
                close(sourceFd);
                close(destinationFd);
                return 1;
            }

            totalWritten += bytesWritten;
        }
    }

    close(sourceFd);
    close(destinationFd);

    std::cout << "file copy success" << std::endl;

    return 0;
}
```

编译运行：

```bash
g++ -std=c++17 -Wall -Wextra \
    file_copy.cpp -o file_copy

./file_copy
```

---

## 九、文件复制验证

查看文件大小：

```bash
wc -c source.txt destination.txt
```

最终结果：

```text
1791 source.txt
1791 destination.txt
```

比较两个文件：

```bash
cmp source.txt destination.txt
```

如果没有输出，通常表示文件完全相同。

查看退出状态：

```bash
echo $?
```

返回：

```text
0
```

表示两个文件逐字节相同。

今天曾经出现：

```text
1791 source.txt
22 destination.txt
```

原因不是程序逻辑错误，而是修改 `source.txt` 后没有重新运行：

```bash
./file_copy
```

文件不会自动同步。当前程序只是运行一次、复制一次。

正确顺序：

```text
修改 source.txt
→ 运行 ./file_copy
→ 使用 wc 检查大小
→ 使用 cmp 比较内容
```

---

## 十、LeetCode 232：用栈实现队列

栈的特点：

```text
LIFO
Last In First Out
后进先出
```

队列的特点：

```text
FIFO
First In First Out
先进先出
```

使用两个栈：

```cpp
stack<int> stackIn;
stack<int> stackOut;
```

其中：

```text
stackIn  → 负责入队
stackOut → 负责出队
```

当 `stackOut` 为空时，把 `stackIn` 中的所有元素搬到 `stackOut`。

搬运后，最早进入的元素会位于 `stackOut` 顶部。

---

## 十一、完整 `MyQueue` 代码

```cpp
#include <stack>

using namespace std;

class MyQueue {
private:
    stack<int> stackIn;
    stack<int> stackOut;

public:
    MyQueue() = default;

    void push(int x) {
        stackIn.push(x);
    }

    int pop() {
        if (stackOut.empty()) {
            while (!stackIn.empty()) {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }

        int result = stackOut.top();
        stackOut.pop();

        return result;
    }

    int peek() {
        if (stackOut.empty()) {
            while (!stackIn.empty()) {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }

        return stackOut.top();
    }

    bool empty() {
        return stackIn.empty() &&
               stackOut.empty();
    }
};
```

---

## 十二、为什么只有输出栈为空时才搬运

假设：

```text
stackOut：2、3
stackIn：4
```

队列的正确顺序应该是：

```text
2 → 3 → 4
```

下一次 `pop()` 必须返回 `2`。

如果这时把 `stackIn` 的 `4` 搬到 `stackOut`，就会破坏原来的队列顺序。

所以必须记住：

```text
stackOut 不为空 → 直接从 stackOut 取

stackOut 为空 → 才把 stackIn 全部搬过去
```

---

## 十三、`pop()`、`top()` 和 `peek()`

`std::stack::pop()` 只删除元素，不返回元素。

错误：

```cpp
int result = stackOut.pop();
```

正确：

```cpp
int result = stackOut.top();
stackOut.pop();
```

在 `MyQueue` 中：

```text
peek() → 查看队首，但不删除
pop()  → 返回队首，并删除
```

---

## 十四、`empty()` 的返回值问题

最开始的写法：

```cpp
bool empty() {
    if (stackIn.empty() &&
        stackOut.empty()) {
        return true;
    }
}
```

存在问题：条件不成立时没有返回值。

正确写法：

```cpp
bool empty() {
    return stackIn.empty() &&
           stackOut.empty();
}
```

只有两个栈都为空时，整个队列才为空。

---

## 十五、`MyQueue() = default`

```cpp
MyQueue() = default;
```

表示：

> 让编译器生成默认构造函数。

这里的两个成员：

```cpp
stack<int> stackIn;
stack<int> stackOut;
```

都可以默认构造，所以不需要特殊初始化。

还可以写成：

```cpp
MyQueue() {
}
```

对于当前类，两种写法效果基本相同。

也可以完全不写构造函数，让编译器自动生成。

如果成员需要特殊初始化，则应该使用初始化列表：

```cpp
class Test {
private:
    int count;

public:
    Test()
        : count(0) {
    }
};
```

---

## 十六、今日主要易错点

1. 文件描述符是 `int`，不是 `FILE*`。
2. `open()` 失败返回 `-1`。
3. 不能假设第一次打开一定得到 `fd = 3`。
4. `read()` 返回 0 表示 EOF，不是错误。
5. `read()` 不会自动添加 `'\0'`。
6. 大文件必须循环调用 `read()`。
7. 写入长度应该使用 `bytesRead`。
8. `write()` 可能发生 partial write。
9. `break` 只结束当前循环。
10. 获取文件描述符后要使用 `close()` 释放。
11. 修改源文件后必须重新运行复制程序。
12. `stack::pop()` 不返回元素。
13. 只有 `stackOut` 为空时才搬运 `stackIn`。
14. `peek()` 不删除元素，`pop()` 会删除元素。
15. 非 `void` 函数必须保证正常路径有返回值。
16. 两个栈同时为空时，队列才为空。
17. `= default` 表示使用编译器生成的默认实现。

---

## 十七、今日英文术语

```text
File Descriptor      文件描述符
System Call          系统调用
Standard Input       标准输入
Standard Output      标准输出
Standard Error       标准错误
End Of File          文件结束
Buffer               缓冲区
Partial Write        部分写入
Stack                栈
Queue                队列
Last In First Out    后进先出
First In First Out   先进先出
Encapsulation        封装
Default Constructor  默认构造函数
```

---

## 十八、今日总结

今天完成了从 C++ 基础向 Linux 系统编程的进一步过渡。

Linux IO 部分掌握了：

```text
open
→ 获得文件描述符
→ read
→ buffer
→ write
→ EOF
→ close
```

并完成了经过 `1791` 字节大文件验证的文件复制程序。

算法部分掌握了：

```text
Stack → LIFO
Queue → FIFO
两个栈 → 实现队列
```

还进一步理解了类的封装、非 `void` 函数返回值以及默认构造函数 `= default`。

