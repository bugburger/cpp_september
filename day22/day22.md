# Day22 学习总结：Linux 文件定位与属性 + 用队列实现栈

> 主线：Linux 文件定位与属性  
> Linux API：`lseek()`、`stat()`、`access()`  
> LeetCode：225. 用队列实现栈  
> 今日目标：理解文件偏移量，能够获取文件大小、类型、权限，并掌握一个队列模拟栈的方法。

---

# 一、今日学习内容

今天主要学习两部分。

## 1. Linux 文件编程

学习内容：

- 文件描述符 `fd`
- 文件偏移量
- `read()` 与 EOF
- `lseek()`
- `SEEK_SET`
- `SEEK_CUR`
- `SEEK_END`
- `stat()`
- `struct stat`
- `st_size`
- `st_mode`
- Linux 文件类型
- Linux `rwx` 权限
- `owner / group / others`
- `644 / 755` 数字权限
- `access()`
- `F_OK / R_OK / W_OK / X_OK`

## 2. LeetCode 225

学习：

- 栈 Stack
- 队列 Queue
- LIFO
- FIFO
- 使用一个 `queue` 模拟 `stack`
- `push()`
- `pop()`
- `top()`
- `empty()`

---

# 二、文件描述符 fd

Linux 中：

```cpp
int fd = open("test.txt", O_RDONLY);
```

这里：

```cpp
fd
```

叫：

> 文件描述符 File Descriptor。

它是一个整数。

进程通过文件描述符引用一个已经打开的文件。

例如：

```cpp
int fd = open("test.txt", O_RDONLY);

if (fd == -1) {
    std::cerr << "open failed" << std::endl;
    return 1;
}
```

通常：

```text
fd >= 0
```

表示打开成功。

```text
fd == -1
```

表示打开失败。

---

# 三、文件偏移量

文件描述符对应的打开文件存在一个：

> 当前文件偏移量。

可以把它理解成文件里的一个“光标”。

例如文件：

```text
ABCDEFGHIJ
```

刚打开：

```text
|ABCDEFGHIJ
^
当前位置
```

执行：

```cpp
read(fd, buffer, 5);
```

读取：

```text
ABCDE
```

之后：

```text
ABCDE|FGHIJ
     ^
```

文件偏移量已经自动向后移动。

再执行：

```cpp
read(fd, buffer, 5);
```

读取：

```text
FGHIJ
```

而不是重新读取 `ABCDE`。

## 核心

```text
read()
    ↓
从当前偏移量开始读取
    ↓
读取成功
    ↓
偏移量自动向后移动
```

---

# 四、read() 返回值

```cpp
ssize_t n = read(fd, buffer, sizeof(buffer));
```

返回值：

## n > 0

表示：

> 成功读取了 n 个字节。

## n == 0

表示：

> 到达文件末尾 EOF。

EOF：

```text
End Of File
```

## n < 0

表示：

> 读取发生错误。

典型代码：

```cpp
ssize_t n = read(fd, buffer, sizeof(buffer));

if (n < 0) {
    std::cerr << "read failed" << std::endl;
}

if (n == 0) {
    break;
}
```

---

# 五、lseek()

`lseek()` 用于：

> 修改当前文件偏移量。

头文件：

```cpp
#include <unistd.h>
```

函数形式：

```cpp
off_t lseek(int fd, off_t offset, int whence);
```

参数：

```text
fd
文件描述符

offset
移动多少字节

whence
以什么位置作为基准
```

---

# 六、SEEK_SET

```cpp
lseek(fd, 0, SEEK_SET);
```

表示：

```text
以文件开头为基准
+
移动 0 个字节
```

也就是：

> 回到文件开头。

例如：

```cpp
lseek(fd, 4, SEEK_SET);
```

对于：

```text
0123456789
```

新的位置：

```text
0123|456789
    ^
```

下一次读取一个字符：

```text
4
```

---

# 七、SEEK_CUR

`CUR`：

```text
current
```

表示：

> 以当前位置作为基准。

假设已经读取：

```text
ABC
```

当前位置：

```text
ABC|DEFGHIJ
   ^
```

执行：

```cpp
lseek(fd, 2, SEEK_CUR);
```

表示再向后移动两个字节：

```text
ABCDE|FGHIJ
     ^
```

下一次读取：

```text
F
```

## 易错点

错误：

```text
SEEK_CUR = 从中间开始
```

正确：

```text
SEEK_CUR = 从当前位置开始
```

当前位置可能在开头、中间或者末尾。

---

# 八、SEEK_END

```cpp
lseek(fd, -3, SEEK_END);
```

表示：

> 从文件末尾向前移动 3 个字节。

例如：

```text
ABCDEFGHIJ
```

执行以后：

```text
ABCDEFG|HIJ
       ^
```

下一次读取：

```text
H
```

---

# 九、lseek() 返回值

成功：

> 返回修改后的新文件偏移量。

失败：

```text
-1
```

例如：

```cpp
off_t pos = lseek(fd, 4, SEEK_SET);

if (pos == -1) {
    std::cerr << "lseek failed" << std::endl;
}
```

成功以后：

```text
pos = 4
```

---

# 十、使用 lseek() 获取文件大小

例如文件：

```text
ABCDEFGHIJ
```

长度：

```text
10 bytes
```

执行：

```cpp
off_t size = lseek(fd, 0, SEEK_END);
```

由于文件末尾偏移量为：

```text
10
```

所以：

```text
size = 10
```

可以：

```cpp
std::cout << "file size: "
          << size
          << " bytes"
          << std::endl;
```

---

# 十一、lseek 到文件末尾后的问题

执行：

```cpp
lseek(fd, 0, SEEK_END);
```

以后：

```text
ABCDEFGHIJ|
          ^
```

已经处于文件末尾。

如果马上：

```cpp
read(fd, buffer, 5);
```

通常：

```text
read() 返回 0
```

因为已经没有内容可以读取。

如果想重新从头读：

```cpp
lseek(fd, 0, SEEK_SET);
```

---

# 十二、文件下标与偏移量

例如：

```text
hello
```

有：

```text
5 bytes
```

可以理解成：

```text
偏移量：

0   1   2   3   4   5
| h | e | l | l | o |
                    ^
```

最后一个字符：

```text
o
```

下标是：

```text
4
```

但文件末尾偏移量：

```text
5
```

因此：

```cpp
lseek(fd, 0, SEEK_END);
```

返回：

```text
5
```

## 易错点

不要混淆：

```text
字符下标
```

和：

```text
文件偏移量
```

---

# 十三、lseek_demo.cpp

```cpp
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
    int fd = open("test.txt", O_RDONLY);

    if (fd == -1) {
        std::cerr << "open failed" << std::endl;
        return 1;
    }

    char buffer[6] = {};

    ssize_t n = read(fd, buffer, 5);

    if (n == -1) {
        std::cerr << "read failed" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "first read: "
              << buffer
              << std::endl;

    if (lseek(fd, 0, SEEK_SET) == -1) {
        std::cerr << "lseek failed" << std::endl;
        close(fd);
        return 1;
    }

    for (int i = 0; i < 6; ++i) {
        buffer[i] = '\0';
    }

    n = read(fd, buffer, 5);

    if (n == -1) {
        std::cerr << "read failed" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "second read: "
              << buffer
              << std::endl;

    close(fd);

    return 0;
}
```

测试：

```bash
echo -n "ABCDEFGHIJ" > test.txt
```

编译：

```bash
g++ -std=c++17 -Wall -Wextra lseek_demo.cpp -o lseek_demo
./lseek_demo
```

输出：

```text
first read: ABCDE
second read: ABCDE
```

---

# 十四、为什么使用 echo -n

如果：

```bash
echo "ABCDEFGHIJ" > test.txt
```

`echo` 通常会额外加入换行：

```text
\n
```

所以文件可能是：

```text
11 bytes
```

使用：

```bash
echo -n "ABCDEFGHIJ" > test.txt
```

不会自动增加换行。

因此正好：

```text
10 bytes
```

---

# 十五、今日遇到的编译 Warning

曾出现：

```text
warning: variable ‘n’ set but not used
[-Wunused-but-set-variable]
```

原因：

```cpp
ssize_t n = read(...);
```

虽然保存了 `read()` 返回值，但是后面没有使用。

更规范：

```cpp
ssize_t n = read(fd, buffer, 5);

if (n == -1) {
    std::cerr << "read failed" << std::endl;
}
```

## 重要习惯

Linux 系统调用：

```text
open
read
write
lseek
stat
```

都应该关注返回值。

---

# 十六、stat()

`stat()` 用于：

> 获取文件属性和元数据。

头文件：

```cpp
#include <sys/stat.h>
```

函数：

```cpp
int stat(const char* pathname,
         struct stat* statbuf);
```

可以获得：

```text
文件大小
文件类型
文件权限
文件所有者
所属组
其他元数据
```

---

# 十七、struct stat fileInfo

```cpp
struct stat fileInfo;
```

意思是：

> 创建一个 `struct stat` 类型的变量 `fileInfo`。

可以理解成：

```text
fileInfo
├── st_size
├── st_mode
├── st_uid
├── st_gid
└── ...
```

今天主要使用：

```cpp
fileInfo.st_size
```

和：

```cpp
fileInfo.st_mode
```

---

# 十八、st_size

```cpp
fileInfo.st_size
```

表示：

> 文件大小。

例如：

```cpp
std::cout << fileInfo.st_size
          << std::endl;
```

对于 10 字节文件：

```text
10
```

---

# 十九、st_mode

```cpp
fileInfo.st_mode
```

里面主要包含：

```text
文件类型
+
文件权限
```

后面可以用相关宏判断。

---

# 二十、为什么要传 &fileInfo

```cpp
stat("test.txt", &fileInfo);
```

这里：

```cpp
fileInfo
```

表示：

> 变量本身。

而：

```cpp
&fileInfo
```

表示：

> 变量在内存中的地址。

`stat()` 需要拿到地址：

```text
&fileInfo
```

才能把查询到的信息写入：

```text
fileInfo
```

---

# 二十一、struct stat 与 stat()

注意：

```cpp
struct stat fileInfo;
stat("test.txt", &fileInfo);
```

这里两个 `stat` 不是一回事。

```cpp
struct stat
```

是：

> 结构体类型。

而：

```cpp
stat(...)
```

是：

> 函数。

---

# 二十二、stat() 返回值

例如：

```cpp
int result = stat("test.txt", &fileInfo);
```

成功：

```text
0
```

失败：

```text
-1
```

所以：

```cpp
if (result == -1) {
    std::cerr << "stat failed" << std::endl;
    return 1;
}
```

---

# 二十三、判断普通文件

```cpp
S_ISREG(fileInfo.st_mode)
```

用于判断：

> 是否为普通文件。

例如：

```cpp
if (S_ISREG(fileInfo.st_mode)) {
    std::cout << "type: regular file"
              << std::endl;
}
```

---

# 二十四、判断目录

```cpp
S_ISDIR(fileInfo.st_mode)
```

用于判断：

> 是否为目录。

例如：

```cpp
if (S_ISDIR(fileInfo.st_mode)) {
    std::cout << "type: directory"
              << std::endl;
}
```

---

# 二十五、Linux 中的 .

```text
.
```

表示：

> 当前目录。

所以：

```cpp
stat(".", &fileInfo);
```

通常：

```cpp
S_ISDIR(fileInfo.st_mode)
```

为真。

---

# 二十六、Linux rwx 权限

执行：

```bash
ls -l test.txt
```

可能得到：

```text
-rw-rw-r--
```

拆分：

```text
- | rw- | rw- | r--
↑    ↑     ↑     ↑
类型 owner group others
```

---

# 二十七、第一位表示文件类型

例如：

```text
-
```

表示：

> 普通文件。

```text
d
```

表示：

> 目录。

例如：

```text
-rw-r--r--
```

是普通文件。

```text
drwxr-xr-x
```

是目录。

---

# 二十八、rwx 含义

```text
r = read
w = write
x = execute
```

中文：

```text
r = 读
w = 写
x = 执行
```

没有某项权限：

```text
-
```

---

# 二十九、owner / group / others

Linux 权限分三组：

```text
owner
group
others
```

例如：

```text
-rwxr-xr--
```

拆：

```text
- | rwx | r-x | r--
```

表示：

## owner

```text
rwx
```

可读、可写、可执行。

## group

```text
r-x
```

可读、不可写、可执行。

## others

```text
r--
```

可读、不可写、不可执行。

---

# 三十、权限数字

Linux：

```text
r = 4
w = 2
x = 1
```

因此：

```text
rwx = 4 + 2 + 1 = 7
rw- = 4 + 2     = 6
r-x = 4 + 1     = 5
r-- = 4         = 4
```

重点：

```text
rwx = 7
rw- = 6
r-x = 5
r-- = 4
```

---

# 三十一、755

```text
7 = rwx
5 = r-x
5 = r-x
```

因此：

```text
755
```

就是：

```text
rwxr-xr-x
```

---

# 三十二、644

```text
6 = rw-
4 = r--
4 = r--
```

所以：

```text
644
```

就是：

```text
rw-r--r--
```

---

# 三十三、chmod

例如：

```bash
chmod 755 program
```

表示：

```text
rwxr-xr-x
```

而：

```bash
chmod 644 test.txt
```

表示：

```text
rw-r--r--
```

---

# 三十四、普通文件与目录的 rwx

## 普通文件

```text
r
读取文件内容

w
修改文件内容

x
执行文件
```

## 目录

```text
r
读取目录中的名称列表

w
修改目录项

x
搜索/进入目录并访问其中对象
```

因此：

```bash
cd day22
```

通常需要目录具有：

```text
x
```

权限。

---

# 三十五、st_mode 权限宏

owner：

```cpp
S_IRUSR
S_IWUSR
S_IXUSR
```

分别：

```text
owner read
owner write
owner execute
```

group：

```cpp
S_IRGRP
S_IWGRP
S_IXGRP
```

others：

```cpp
S_IROTH
S_IWOTH
S_IXOTH
```

---

# 三十六、权限宏记忆方式

```text
R = Read
W = Write
X = Execute
```

以及：

```text
USR = User / Owner
GRP = Group
OTH = Others
```

所以：

```cpp
S_IRGRP
```

表示：

> group 是否拥有 read 权限。

```cpp
S_IWOTH
```

表示：

> others 是否拥有 write 权限。

---

# 三十七、今天出现的两个 &

第一个：

```cpp
&fileInfo
```

这里：

```text
&
```

表示：

> 取地址。

第二个：

```cpp
fileInfo.st_mode & S_IRUSR
```

这里：

```text
&
```

表示：

> 按位与。

两个 `&` 长得相同，但是用途不同。

这是今天的重要易错点。

---

# 三十八、三目运算符

例如：

```cpp
(condition) ? "r" : "-"
```

意思：

```text
condition 成立
→ "r"

condition 不成立
→ "-"
```

例如：

```cpp
std::cout
    << ((fileInfo.st_mode & S_IRUSR)
        ? "r"
        : "-");
```

等价：

```cpp
if (fileInfo.st_mode & S_IRUSR) {
    std::cout << "r";
} else {
    std::cout << "-";
}
```

---

# 三十九、输出完整权限

```cpp
std::cout << "permissions: ";

std::cout << ((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
std::cout << ((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
std::cout << ((fileInfo.st_mode & S_IXUSR) ? "x" : "-");

std::cout << ((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
std::cout << ((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
std::cout << ((fileInfo.st_mode & S_IXGRP) ? "x" : "-");

std::cout << ((fileInfo.st_mode & S_IROTH) ? "r" : "-");
std::cout << ((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
std::cout << ((fileInfo.st_mode & S_IXOTH) ? "x" : "-");

std::cout << std::endl;
```

例如：

```text
permissions: rw-rw-r--
```

---

# 四十、file_stat.cpp

```cpp
#include <sys/stat.h>
#include <iostream>

int main() {
    struct stat fileInfo;

    int result = stat("test.txt", &fileInfo);

    if (result == -1) {
        std::cerr << "stat failed" << std::endl;
        return 1;
    }

    std::cout << "file size: "
              << fileInfo.st_size
              << " bytes"
              << std::endl;

    if (S_ISREG(fileInfo.st_mode)) {
        std::cout << "type: regular file" << std::endl;
    }
    else if (S_ISDIR(fileInfo.st_mode)) {
        std::cout << "type: directory" << std::endl;
    }
    else {
        std::cout << "type: other" << std::endl;
    }

    std::cout << "permissions: ";

    std::cout << ((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXUSR) ? "x" : "-");

    std::cout << ((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXGRP) ? "x" : "-");

    std::cout << ((fileInfo.st_mode & S_IROTH) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXOTH) ? "x" : "-");

    std::cout << std::endl;

    return 0;
}
```

实际：

```text
file size: 10 bytes
type: regular file
permissions: rw-rw-r--
```

---

# 四十一、access()

头文件：

```cpp
#include <unistd.h>
```

函数：

```cpp
int access(const char* pathname, int mode);
```

用于检查：

> 当前访问条件下，对路径是否具有指定访问能力。

---

# 四十二、F_OK / R_OK / W_OK / X_OK

```cpp
F_OK
```

表示：

> 文件是否存在。

```cpp
R_OK
```

表示：

> 是否可读。

```cpp
W_OK
```

表示：

> 是否可写。

```cpp
X_OK
```

表示：

> 是否可执行。

记忆：

```text
F = File
R = Read
W = Write
X = Execute
```

---

# 四十三、access() 返回值

成功：

```text
0
```

失败：

```text
-1
```

例如：

```cpp
if (access("test.txt", F_OK) == 0) {
    std::cout << "file exists" << std::endl;
}
```

---

# 四十四、access() 易错点

例如：

```cpp
access("test.txt", X_OK)
```

返回：

```text
-1
```

最直接说明：

> X_OK 检查没有通过。

不能直接认为：

```text
文件一定不存在
```

文件存在性应该检查：

```cpp
F_OK
```

---

# 四十五、access_demo.cpp

```cpp
#include <unistd.h>
#include <iostream>

int main() {
    const char* filename = "test.txt";

    if (access(filename, F_OK) == 0) {
        std::cout << "file exists" << std::endl;
    } else {
        std::cout << "file does not exist" << std::endl;
        return 1;
    }

    if (access(filename, R_OK) == 0) {
        std::cout << "readable: yes" << std::endl;
    } else {
        std::cout << "readable: no" << std::endl;
    }

    if (access(filename, W_OK) == 0) {
        std::cout << "writable: yes" << std::endl;
    } else {
        std::cout << "writable: no" << std::endl;
    }

    if (access(filename, X_OK) == 0) {
        std::cout << "executable: yes" << std::endl;
    } else {
        std::cout << "executable: no" << std::endl;
    }

    return 0;
}
```

今天实际结果：

```text
file exists
readable: yes
writable: yes
executable: no
```

---

# 四十六、stat() 与 access() 区别

## stat()

更像是在问：

> 这个文件本身有什么属性？

例如：

```text
文件大小
文件类型
权限位
```

## access()

更像是在问：

> 当前访问检查下，我能不能对它做某件事？

例如：

```text
是否存在
是否可读
是否可写
是否可执行
```

---

# 四十七、综合 file_info.cpp

今天综合程序使用：

```text
access
stat
open
lseek
close
```

实际运行：

```text
===== File Info =====
file: test.txt
size: 10 bytes
type: regular file
permissions: rw-rw-r--
readable: yes
writable: yes
executable: no
current offset: 0
end offset: 10
reset offset: 0
```

理解：

```text
current offset: 0
```

刚打开时位于开头。

```text
end offset: 10
```

`SEEK_END` 到达文件末尾。

```text
reset offset: 0
```

`SEEK_SET` 成功回到文件开头。

---

# 四十八、lseek() 工程注意点

不是所有文件描述符都能：

```cpp
lseek()
```

普通文件通常可以。

但例如：

```text
pipe
socket
部分设备
```

可能无法像普通文件一样随机定位。

因此应该检查：

```cpp
lseek(...)
```

是否返回：

```text
-1
```

---

# 四十九、access() 工程注意点

学习阶段：

```cpp
access()
```

很适合练习：

```text
文件存在
读权限
写权限
执行权限
```

但是实际程序如果最终需要：

```text
打开文件并操作
```

不要认为：

```text
access 成功
```

就代表：

```text
之后 open 一定成功
```

因为两次调用之间文件状态可能发生变化。

因此真正打开文件时：

> 仍然必须检查 `open()` 自己的返回值。

---

# 五十、LeetCode 225：用队列实现栈

题目目标：

> 使用队列实现栈。

---

# 五十一、Stack 栈

核心：

```text
LIFO
Last In First Out
后进先出
```

执行：

```text
push(1)
push(2)
push(3)
```

栈：

```text
栈顶
 ↓
 3
 2
 1
```

执行：

```cpp
pop();
```

应该删除：

```text
3
```

---

# 五十二、Queue 队列

核心：

```text
FIFO
First In First Out
先进先出
```

例如：

```text
push(1)
push(2)
push(3)
```

队列：

```text
front
 ↓
[1, 2, 3]
       ↑
      back
```

执行：

```cpp
pop();
```

删除：

```text
1
```

---

# 五十三、队列模拟栈的矛盾

栈：

```text
最后加入
最先出去
```

队列：

```text
最早加入
最先出去
```

因此需要主动调整队列内部顺序。

---

# 五十四、核心思想：旋转旧元素

假设：

```text
q = [1, 2]
```

执行：

```cpp
push(3);
```

正常：

```text
[1, 2, 3]
```

但是为了模拟栈，需要：

```text
[3, 1, 2]
```

操作：

```text
[1,2,3]

取 1 放到队尾

[2,3,1]

取 2 放到队尾

[3,1,2]
```

最终：

```text
front = 3
```

所以人为维护：

> 队头永远等于栈顶。

---

# 五十五、push()

```cpp
void push(int x) {
    int n = q.size();

    q.push(x);

    for (int i = 0; i < n; ++i) {
        int y = q.front();
        q.pop();
        q.push(y);
    }
}
```

注意：

```cpp
int n = q.size();
```

必须在：

```cpp
q.push(x);
```

之前。

因为 `n` 要记录：

> 旧元素数量。

---

# 五十六、push() 易错点

错误：

```cpp
q.push(x);

for (int i = 0; i < q.size(); ++i) {
    ...
}
```

例如：

```text
[1,2]
```

加入 `3`：

```text
[1,2,3]
```

如果旋转三次：

```text
[1,2,3]
→ [2,3,1]
→ [3,1,2]
→ [1,2,3]
```

第三次把：

```text
3
```

又移走了。

正确：

> 只旋转旧元素数量。

---

# 五十七、q.size() 语法

错误：

```cpp
q.size)()
```

正确：

```cpp
q.size()
```

因为：

```cpp
size()
```

是成员函数。

---

# 五十八、queue::pop() 没有返回值

错误：

```cpp
return q.pop();
```

原因：

```cpp
q.pop()
```

只负责：

> 删除队头元素。

返回类型：

```cpp
void
```

正确：

```cpp
int x = q.front();
q.pop();
return x;
```

---

# 五十九、pop()

```cpp
int pop() {
    int x = q.front();
    q.pop();
    return x;
}
```

因为已经保证：

```text
队头 = 栈顶
```

---

# 六十、top()

```cpp
int top() {
    return q.front();
}
```

只读取栈顶：

> 不删除元素。

---

# 六十一、empty()

```cpp
bool empty() {
    return q.empty();
}
```

因为内部只有一个：

```cpp
queue<int> q;
```

队列为空：

> 模拟的栈也为空。

---

# 六十二、完整 MyStack

```cpp
#include <queue>

class MyStack {
private:
    std::queue<int> q;

public:
    MyStack() = default;

    void push(int x) {
        int n = q.size();

        q.push(x);

        for (int i = 0; i < n; ++i) {
            int y = q.front();
            q.pop();
            q.push(y);
        }
    }

    int pop() {
        int x = q.front();
        q.pop();
        return x;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }
};
```

---

# 六十三、测试 MyStack

```cpp
#include <iostream>
#include <queue>

class MyStack {
private:
    std::queue<int> q;

public:
    MyStack() = default;

    void push(int x) {
        int n = q.size();

        q.push(x);

        for (int i = 0; i < n; ++i) {
            int y = q.front();
            q.pop();
            q.push(y);
        }
    }

    int pop() {
        int x = q.front();
        q.pop();
        return x;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }
};

int main() {
    MyStack s;

    s.push(1);
    s.push(2);
    s.push(3);

    std::cout << "top: "
              << s.top()
              << std::endl;

    std::cout << "pop: "
              << s.pop()
              << std::endl;

    std::cout << "pop: "
              << s.pop()
              << std::endl;

    std::cout << "pop: "
              << s.pop()
              << std::endl;

    std::cout << "empty: "
              << std::boolalpha
              << s.empty()
              << std::endl;

    return 0;
}
```

预期：

```text
top: 3
pop: 3
pop: 2
pop: 1
empty: true
```

---

# 六十四、时间复杂度

当前实现：

## push()

需要旋转原有元素：

```text
O(n)
```

## pop()

```text
O(1)
```

## top()

```text
O(1)
```

## empty()

```text
O(1)
```

## 空间复杂度

```text
O(n)
```

---

# 六十五、与 Day21 对比

Day21：

```text
两个 stack
→ 模拟 queue
```

Day22：

```text
一个 queue
→ 模拟 stack
```

Day21 核心：

> 利用两个栈改变元素顺序。

Day22 核心：

> 新元素加入以后旋转旧元素，让新元素来到队头。

---

# 六十六、今日易错点总结

## 易错 1：read() 每次从头读

错误。

`read()`：

> 从当前文件偏移量开始。

---

## 易错 2：read() 返回 0

```cpp
read() == 0
```

通常表示：

```text
EOF
```

不是普通错误。

---

## 易错 3：SEEK_CUR

不是：

```text
从文件中间
```

而是：

```text
从当前位置
```

---

## 易错 4：SEEK_END 后继续 read

执行：

```cpp
lseek(fd, 0, SEEK_END);
```

之后已经在文件末尾。

如果还要重新读：

```cpp
lseek(fd, 0, SEEK_SET);
```

---

## 易错 5：忽略 lseek 返回值

失败：

```text
-1
```

应该检查。

---

## 易错 6：struct stat fileInfo

```cpp
struct stat fileInfo;
```

是在：

> 创建一个结构体变量。

不是调用 `stat()`。

---

## 易错 7：struct stat 与 stat()

```cpp
struct stat
```

是类型。

```cpp
stat(...)
```

是函数。

---

## 易错 8：两个 &

```cpp
&fileInfo
```

是：

> 取地址。

```cpp
st_mode & S_IRUSR
```

是：

> 按位与。

---

## 易错 9：权限第一位

```text
-rw-rw-r--
```

第一位：

```text
-
```

表示文件类型。

后面的：

```text
rw-rw-r--
```

才是权限。

---

## 易错 10：权限数字

记：

```text
r = 4
w = 2
x = 1
```

---

## 易错 11：F_OK / R_OK / W_OK / X_OK

```text
F_OK = 存在
R_OK = 可读
W_OK = 可写
X_OK = 可执行
```

---

## 易错 12：queue::pop()

不能：

```cpp
return q.pop();
```

应该：

```cpp
int x = q.front();
q.pop();
return x;
```

---

## 易错 13：q.size()

正确：

```cpp
q.size()
```

---

## 易错 14：push() 旋转整个队列

不应该旋转：

> 新队列所有元素。

只应该旋转：

> 旧元素数量。

---

# 六十七、今日核心代码速记

回到文件开头：

```cpp
lseek(fd, 0, SEEK_SET);
```

查看当前位置：

```cpp
lseek(fd, 0, SEEK_CUR);
```

移动到文件末尾：

```cpp
lseek(fd, 0, SEEK_END);
```

获取文件信息：

```cpp
struct stat fileInfo;
stat("test.txt", &fileInfo);
```

文件大小：

```cpp
fileInfo.st_size
```

普通文件：

```cpp
S_ISREG(fileInfo.st_mode)
```

目录：

```cpp
S_ISDIR(fileInfo.st_mode)
```

文件存在：

```cpp
access(filename, F_OK)
```

可读：

```cpp
access(filename, R_OK)
```

可写：

```cpp
access(filename, W_OK)
```

可执行：

```cpp
access(filename, X_OK)
```

Queue 队头：

```cpp
q.front()
```

Queue 删除队头：

```cpp
q.pop()
```

Queue 插入队尾：

```cpp
q.push(x)
```

---

# 六十八、今日口述复习题

## 1. 文件描述符是什么？

进程用来引用已经打开文件的整数标识。

## 2. 文件偏移量是什么？

当前文件读写位置。

`read()` 会推进它。

`lseek()` 可以修改它。

## 3. SEEK_SET 是什么？

以文件开头为基准。

## 4. SEEK_CUR 是什么？

以当前位置为基准。

## 5. SEEK_END 是什么？

以文件末尾为基准。

## 6. read() 返回 0 表示什么？

通常表示 EOF，即文件已经读取结束。

## 7. stat() 是做什么的？

获取文件元数据，例如：

- 大小
- 类型
- 权限

## 8. struct stat fileInfo 是什么？

创建一个保存文件属性的结构体变量。

## 9. 为什么 stat 要传 &fileInfo？

因为 `stat()` 需要通过变量地址把结果写入 `fileInfo`。

## 10. rwx 是什么？

```text
r = read
w = write
x = execute
```

## 11. 755 是什么？

```text
7 = rwx
5 = r-x
5 = r-x
```

所以：

```text
rwxr-xr-x
```

## 12. stat 和 access 有什么区别？

`stat()`：

> 获取文件属性。

`access()`：

> 检查当前访问条件下是否能读、写、执行等。

## 13. 栈和队列的区别？

栈：

```text
LIFO
后进先出
```

队列：

```text
FIFO
先进先出
```

## 14. 一个队列如何模拟栈？

每次加入新元素以后：

> 把所有旧元素依次从队头移动到队尾。

最终：

```text
队头 = 栈顶
```

---

# 六十九、Day22 文件结构

建议：

```text
day22/
├── day22.md
├── test.txt
├── lseek_demo.cpp
├── file_stat.cpp
├── access_demo.cpp
├── file_info.cpp
└── my_stack.cpp
```

编译产生的：

```text
lseek_demo
file_stat
access_demo
file_info
my_stack
```

属于可执行文件。

Git 提交时通常优先提交：

```text
.md
.cpp
测试文本
```

---

# 七十、Day22 最终验收

- [x] 理解文件描述符
- [x] 理解文件偏移量
- [x] 理解 `read()` 会推进偏移量
- [x] 理解 `read() == 0` 表示 EOF
- [x] 掌握 `lseek()`
- [x] 掌握 `SEEK_SET`
- [x] 掌握 `SEEK_CUR`
- [x] 掌握 `SEEK_END`
- [x] 能使用 `lseek()` 回到文件开头
- [x] 理解文件末尾偏移量
- [x] 掌握 `stat()`
- [x] 理解 `struct stat`
- [x] 理解 `&fileInfo`
- [x] 使用 `st_size`
- [x] 使用 `st_mode`
- [x] 判断普通文件和目录
- [x] 看懂 Linux `rwx`
- [x] 理解 owner/group/others
- [x] 掌握 `r=4 w=2 x=1`
- [x] 看懂 `644`
- [x] 看懂 `755`
- [x] 使用权限宏
- [x] 掌握 `access()`
- [x] 掌握 `F_OK`
- [x] 掌握 `R_OK`
- [x] 掌握 `W_OK`
- [x] 掌握 `X_OK`
- [x] 理解 `stat()` 与 `access()` 的区别
- [x] 完成 `file_info.cpp`
- [x] 理解 Stack 的 LIFO
- [x] 理解 Queue 的 FIFO
- [x] 用一个 Queue 实现 Stack
- [x] 实现 `push()`
- [x] 实现 `pop()`
- [x] 实现 `top()`
- [x] 实现 `empty()`
- [x] 理解 `queue::pop()` 不返回元素
- [x] 理解 `push()` 为什么只旋转旧元素

---

# 七十一、Day22 总结

今天 Linux 主线从 Day21 的：

```text
open
read
write
close
```

继续扩展到：

```text
lseek
→ 文件定位

stat
→ 文件属性

access
→ 文件访问能力检查
```

今天 Linux 部分最重要的思想：

> 文件描述符不仅代表一个打开的文件，还存在当前文件偏移量。`read()` 会推进偏移量，而 `lseek()` 可以主动修改偏移量。

算法部分完成：

```text
queue
→ stack
```

最重要的思想：

> 每次加入新元素以后，将旧元素轮转到队尾，让最新元素移动到队头，从而始终保持“队头 = 栈顶”。

---

# Day22 完成
