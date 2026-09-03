# Day 4：类、对象生命周期与滑动窗口

计划日期：2026-09-04  
提前完成日期：2026-09-03

## 今日学习目标

- 理解类和对象的基本关系。
- 掌握构造函数和析构函数的基本写法。
- 理解内层作用域与外层作用域。
- 观察局部对象和动态对象的生命周期。
- 掌握对象和对象指针访问成员的不同方式。
- 完成 LeetCode 209「长度最小的子数组」。

## 1. 类和对象

类可以理解为程序员定义的一种新类型：

```cpp
class Student
{
};
```

使用这个类型创建变量：

```cpp
Student alice;
```

其中：

- `Student` 是类，也是一种类型。
- `alice` 是根据 `Student` 类创建的对象。
- 每个对象都可以拥有自己的成员变量和成员函数。

例如：

```cpp
class Student
{
public:
    void introduce()
    {
        std::cout << name_ << '\n';
    }

private:
    std::string name_;
    int age_;
};
```

`public` 中的成员可以从类外部访问，`private` 中的成员只能由类内部直接访问。

## 2. 构造函数

构造函数会在对象创建时自动调用，主要负责初始化对象。

```cpp
Student(const std::string& name, int age)
    : name_(name), age_(age)
{
    std::cout << "Constructor: " << name_ << '\n';
}
```

构造函数具有以下特点：

- 名字必须和类名相同。
- 没有返回值类型，不能写 `void`。
- 创建对象时自动调用。
- 可以通过初始化列表初始化成员变量。

初始化列表：

```cpp
: name_(name), age_(age)
```

表示使用参数 `name` 初始化成员变量 `name_`，使用参数 `age` 初始化成员变量 `age_`。

## 3. 析构函数

析构函数会在对象生命周期结束时自动调用，主要负责清理对象管理的资源。

```cpp
~Student()
{
    std::cout << "Destructor: " << name_ << '\n';
}
```

析构函数的特点：

- 名字是 `~` 加类名。
- 没有返回值。
- 不接收普通参数。
- 对象销毁时自动调用。

以后学习 RAII 时，可以在析构函数中释放动态内存、文件、锁等资源。

## 4. 内层作用域与外层作用域

一对花括号通常会形成一个作用域：

```cpp
int main()
{
    Student alice("Alice", 24);

    {
        Student bob("Bob", 23);
        Student carol("Carol", 22);
    }

    return 0;
}
```

`alice` 位于 `main()` 的外层作用域，因此会一直存在到 `main()` 结束。

`bob` 和 `carol` 位于内层作用域，程序运行到内层右花括号时，它们就会自动析构。

作用域还会影响名字的可见范围：

```text
内层作用域可以访问外层作用域中的变量
外层作用域不能访问已经结束的内层作用域中的变量
```

内层作用域可以限制变量的使用范围，也可以让临时对象提前结束生命周期。

## 5. 构造与析构顺序

同一作用域中的局部对象按照定义顺序构造，按照构造顺序的相反顺序析构。

例如：

```cpp
Student bob("Bob", 23);
Student carol("Carol", 22);
```

执行顺序为：

```text
构造：Bob → Carol
析构：Carol → Bob
```

这样设计的原因是：后创建的对象可能依赖先创建的对象。

如果后创建的对象先析构，那么它析构时所依赖的先创建对象仍然存在。

这可以理解为：

```text
后进先出
```

## 6. 局部对象与动态对象

普通局部对象：

```cpp
Student alice("Alice", 24);
```

会在所在作用域结束时自动调用析构函数。

动态对象：

```cpp
Student* david = new Student("David", 25);
```

这里存在两个不同的对象：

- `david` 是局部指针变量。
- `new Student(...)` 创建的是堆中的动态对象。
- 指针离开作用域，不会自动销毁它指向的动态对象。

因此需要执行：

```cpp
delete david;
david = nullptr;
```

`delete david` 会自动调用析构函数，然后释放动态对象占用的内存。

准确的表达是：

```text
程序员手动执行 delete
delete 自动触发析构函数并释放内存
```

## 7. 点运算符与箭头运算符

普通对象访问成员时使用点运算符：

```cpp
Student alice("Alice", 24);
alice.introduce();
```

对象指针访问成员时使用箭头运算符：

```cpp
Student* david = new Student("David", 25);
david->introduce();
```

箭头运算符相当于先解引用，再访问对象成员：

```cpp
david->introduce();
```

等价于：

```cpp
(*david).introduce();
```

记忆方式：

```text
对象或引用访问成员：.
对象指针访问成员：->
```

## 8. student_lifecycle.cpp

本次生命周期实验完成了：

- 创建外层作用域中的 `Alice`。
- 创建内层作用域中的 `Bob` 和 `Carol`。
- 观察 `Carol` 和 `Bob` 的反向析构顺序。
- 使用 `new` 创建动态对象 `David`。
- 使用 `delete` 触发 `David` 的析构函数。
- 观察 `Alice` 在 `main()` 结束时最后析构。

关键析构顺序：

```text
Destructor: Carol
Destructor: Bob
Destructor: David
Destructor: Alice
```

其中：

- `Carol` 和 `Bob` 在内层作用域结束时析构。
- `David` 在执行 `delete david` 时析构。
- `Alice` 在 `main()` 外层作用域结束时析构。

## 9. LeetCode 209：长度最小的子数组

题目要求在一个正整数数组中，找到元素之和大于或等于 `target` 的最短连续子数组。

例如：

```text
target = 7
nums = [2, 3, 1, 2, 4, 3]
```

最短的符合条件子数组是：

```text
[4, 3]
```

因此返回：

```text
2
```

注意：

- 子数组必须是连续的。
- 条件是 `sum >= target`，不只是 `sum == target`。

## 10. 滑动窗口

使用两个指针维护一个连续窗口：

```text
[slow, fast]
```

执行过程：

1. 使用 `fast` 扩大窗口。
2. 将 `nums[fast]` 加入窗口总和。
3. 如果总和达到 `target`，记录当前窗口长度。
4. 移除 `nums[slow]`，让 `slow` 向右移动。
5. 持续缩小窗口，直到总和小于 `target`。
6. 再继续移动 `fast`。

核心代码：

```cpp
while (fast < n)
{
    sum += nums[fast];

    while (sum >= target)
    {
        int length = fast - slow + 1;

        if (length < result)
        {
            result = length;
        }

        sum -= nums[slow];
        ++slow;
    }

    ++fast;
}
```

这里需要使用：

```cpp
while (sum >= target)
```

不能只使用一次性的 `if`，因为当前窗口满足条件后，可能还可以连续删除多个左端元素，从而得到更短的窗口。

## 11. 本次代码中的问题

最开始的代码存在同一个右端元素被重复加入的问题：

```cpp
ans += nums[fast];
```

当 `ans >= target` 时，如果没有移动 `fast`，下一轮又执行这句话，就会再次加入同一个元素。

正确方式是：

```text
每个 nums[fast] 只加入一次
满足条件后使用内层 while 连续移动 slow
处理完成后再移动 fast
```

结果变量初始化为：

```cpp
int result = n + 1;
```

因为有效子数组的最大长度是 `n`，所以 `n + 1` 表示“目前还没有找到答案”。

最后判断：

```cpp
if (result == n + 1)
{
    return 0;
}
```

## 12. 算法复杂度

滑动窗口中：

- `fast` 最多遍历数组一次。
- `slow` 最多遍历数组一次。
- 每个元素最多被加入和移除一次。

因此：

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

虽然代码中存在嵌套的 `while`，但内层循环的总执行次数不会超过数组长度。

## 今日核心总结

```text
类是程序员定义的新类型，对象是类的实例
构造函数在对象创建时自动调用
析构函数在对象生命周期结束时自动调用
内层作用域中的局部对象会更早析构
同一作用域中的对象按照构造顺序的相反顺序析构
new 创建的动态对象需要通过 delete 释放
对象访问成员使用 .
对象指针访问成员使用 ->
滑动窗口使用 fast 扩大窗口，使用 slow 缩小窗口
窗口满足条件时，需要使用 while 连续收缩
每个 nums[fast] 只能加入窗口一次
```

## 当前掌握状态

目前能够理解类、对象、构造函数、析构函数和基本生命周期，也能够判断局部对象的析构顺序。

已经能够在提示下写出 LeetCode 209 的滑动窗口结构，但对 `slow`、`fast` 和窗口总和之间的变化还需要继续练习。

后续需要重点复习：

- 为什么析构顺序与构造顺序相反。
- 局部指针和动态对象之间的区别。
- 为什么滑动窗口中 `fast` 不能重复加入。
- 为什么满足条件后需要使用 `while` 收缩窗口。

## 今日完成文件

```text
day04/student_lifecycle.cpp
day04/leetcode209.cpp
day04/README.md
```

## 下一阶段

下一学习日继续按照九月份学习计划进行，并复习今天的对象生命周期和滑动窗口知识。
