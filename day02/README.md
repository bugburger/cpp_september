# Day 2：指针、数组、const 与双指针

日期：2026-09-02

## 今日完成内容

### 1. 指针与数组

通过 `pointer_array.cpp` 练习了数组与指针的关系。

掌握的核心内容：

* 数组不是指针，但数组名在多数表达式中会退化为指向首元素的指针。
* `numbers`、`&numbers[0]` 和指针 `p` 保存或表示相同的首元素地址。
* `numbers[i]`、`p[i]` 和 `*(p + i)` 可以访问相同的数组元素。
* 指针加法按元素类型移动，不是简单增加一个字节。
* 通过 `*(p + 2)` 可以访问并修改 `numbers[2]`。
* `sizeof(numbers)` 得到整个数组的大小。
* `sizeof(p)` 得到指针变量本身的大小。

本次实验中：

```text
sizeof(numbers) = 20
sizeof(p) = 8
```

数组包含5个 `int`，每个 `int` 占4字节，因此共占20字节；当前64位 Ubuntu 系统中的指针占8字节。

### 2. const 指针与常量引用

通过 `const_demo.cpp` 学习了以下三种声明：

```cpp
const int* p1;
int* const p2;
const int& ref;
```

区别如下：

| 声明               |   能否改变指向 | 能否通过它修改数据 |
| ---------------- | -------: | --------: |
| `const int* p1`  |       可以 |       不可以 |
| `int* const p2`  |      不可以 |        可以 |
| `const int& ref` | 引用不能重新绑定 |       不可以 |

主动取消错误代码的注释后，观察到了编译器报错：

```text
assignment of read-only location
assignment of read-only variable
assignment of read-only reference
```

这说明 `const` 可以在编译阶段阻止非法修改。

### 3. Vim 交换文件问题

编辑代码时遇到了 Vim 的 `E325` 警告。

了解到 `.swp` 是 Vim 自动生成的交换文件，通常表示：

* 同一个文件可能仍被另一个 Vim 进程编辑；
* 或者上次 Vim 异常退出，遗留了交换文件。

处理时应先使用 `jobs` 和 `ps` 检查旧进程，确认没有其他 Vim 正在编辑后，再删除过期的交换文件，不能直接忽略警告。

### 4. 指针版与引用版 swap

通过 `swap_demo.cpp` 分别实现了：

```cpp
swap_by_pointer(int* a, int* b);
swap_by_reference(int& a, int& b);
```

指针版本调用：

```cpp
swap_by_pointer(&x, &y);
```

函数接收变量地址，通过 `*a` 和 `*b` 修改原变量。

引用版本调用：

```cpp
swap_by_reference(x, y);
```

函数中的引用参数是原变量的别名，因此可以直接修改原变量。

核心区别：

> 指针通过“地址与解引用”访问原变量，引用通过“别名”访问原变量。

### 5. LeetCode 27：移除元素

通过 `leetcode27.cpp` 学习了快慢双指针。

* `fast` 负责遍历数组中的每个元素。
* `slow` 指向下一个有效元素应该写入的位置。
* 当 `nums[fast] != val` 时，将有效元素写到 `nums[slow]`，然后增加 `slow`。
* 最后返回 `slow`，它表示保留下来的有效元素数量。

已验证测试：

```text
nums = [3, 2, 2, 3]
val = 3
```

运行结果：

```text
k = 2
Remaining elements: 2 2
```

这里的“双指针”是算法概念，可以由两个数组下标实现，不一定必须使用两个真正的 C++ 指针。

### 6. 复查二分查找

复查 Day 1 的二分查找代码时，发现原代码错误地使用了：

```cpp
mid == target
```

`mid` 是数组下标，真正需要比较的是该位置的元素值：

```cpp
nums[mid] == target
```

同理，左右区间调整也必须根据 `nums[mid]` 与 `target` 的大小关系进行。

## 今日形成的关键认识

```text
数组名在多数表达式中可以退化为首元素指针
p + i 表示向后移动 i 个对应类型的元素
*p 表示访问指针指向的数据
const int* 限制数据
int* const 限制指针本身
引用是原变量的别名
快慢双指针可以原地整理数组
mid 是下标，nums[mid] 才是元素值
```

## 待巩固内容

* 使用第二组数据继续测试 LeetCode 27。
* 独立默写指针版和引用版 `swap`。
* 继续练习区分 `const int*`、`int* const` 和 `const int&`。
* 修改并重新测试 Day 1 的二分查找代码。

