# C++ September

## Day 01

### C++ Basics

学习内容：

- 变量与函数
- 值传递
- 指针传递
- 引用传递
- 指针与解引用

### Parameter Passing

值传递：

函数获得实参的副本，修改形参不会影响原对象。

指针传递：

函数获得原对象的地址，通过解引用可以修改原对象。

引用传递：

引用是原对象的别名，可以直接修改原对象。

### Pointer

- `x`：变量的值
- `&x`：变量的地址
- `p`：指针保存的地址
- `*p`：指针指向的对象
- `&p`：指针变量自身的地址

### LeetCode

704 Binary Search

使用左闭右闭区间：

`[left, right]`

因此：

```cpp
while (left <= right)
