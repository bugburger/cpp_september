# Day11 学习总结

## 今日学习内容

今天主要学习了 C++ 中的左值、右值、左值引用、右值引用，以及 `const T&` 和 `T&&` 的基本使用。

同时完成了 LeetCode 142「环形链表 II」，继续练习快慢指针在链表中的应用。

### 1. 左值与右值

左值可以简单理解为：

> 有明确身份，可以在之后继续找到和访问的对象。

例如：

```cpp
int a = 10;
```

这里：

```cpp
a
```

是左值。

即使写成：

```cpp
int b = a;
```

`a` 出现在等号右边，它仍然是左值。

因此不能简单理解成：

```text
等号左边 = 左值
等号右边 = 右值
```

右值通常表示临时值或者计算产生的临时结果，例如：

```cpp
10
a + b
```

其中：

```cpp
a + b
```

产生的是一个临时计算结果，因此属于右值。

---

## 2. 左值引用

普通引用：

```cpp
int& ref = a;
```

属于左值引用。

例如：

```cpp
int a = 10;
int& ref = a;
```

`ref` 并没有创建新的 `int` 对象，而是成为 `a` 的另一个名字。

因此：

```cpp
ref = 20;
```

之后：

```cpp
a == 20
```

普通左值引用通常只能绑定左值：

```cpp
int& ref = a;     // 正确
```

而：

```cpp
int& ref = 10;    // 错误
```

因为 `10` 是右值。

---

## 3. 右值引用

C++11 引入了右值引用：

```cpp
T&&
```

例如：

```cpp
int&& r = 10;
```

这里：

```cpp
10
```

是右值，因此可以绑定到：

```cpp
int&&
```

可以先记住：

```text
T&   -> 通常绑定左值
T&&  -> 通常绑定右值
```

右值引用是后续学习移动语义和 `std::move` 的基础。

---

## 4. 右值引用变量本身仍然是左值

今天比较容易混淆的一点是：

```cpp
int&& r = 10;
```

虽然 `r` 的类型是：

```cpp
int&&
```

但是单独使用：

```cpp
r
```

时，它是一个左值。

因为 `r` 已经有了名字，可以再次找到它。

因此需要区分：

```text
变量的类型
```

和：

```text
表达式的值类别
```

例如：

```cpp
int&& r = 10;
```

可以理解为：

```text
r 的类型：int&&
表达式 r：左值
```

这是理解后面 `std::move(r)` 的关键。

---

## 5. const T& 与 T&& 函数重载

今天还练习了通过不同引用类型区分左值和右值：

```cpp
void process(const string& str)
{
    cout << "const lvalue reference" << endl;
}

void process(string&& str)
{
    cout << "rvalue reference" << endl;
}
```

例如：

```cpp
string s = "hello";

process(s);
process(string("world"));
```

其中：

```cpp
process(s);
```

会匹配：

```cpp
const string&
```

而：

```cpp
process(string("world"));
```

可以匹配：

```cpp
string&&
```

`const T&` 常用于只读访问对象而避免复制；

`T&&` 常用于接收临时对象或者后续允许转移资源的对象。

---

## 6. LeetCode 142：环形链表 II

今天使用快慢指针解决了环形链表入口问题。

第一阶段使用：

```cpp
ListNode* slow = head;
ListNode* fast = head;
```

每次移动：

```cpp
slow = slow->next;
fast = fast->next->next;
```

如果链表存在环：

```cpp
slow == fast
```

最终一定会在环内发生。

需要注意循环条件：

```cpp
while (fast != nullptr && fast->next != nullptr)
```

这样才能保证：

```cpp
fast->next->next
```

不会造成空指针访问。

---

## 7. 寻找环入口

快慢指针第一次相遇之后：

```cpp
slow = head;
```

让：

```text
slow -> 链表头
fast -> 第一次相遇点
```

然后两个指针都一次走一步：

```cpp
slow = slow->next;
fast = fast->next;
```

再次相遇的位置就是环入口。

最终代码核心：

```cpp
if (slow == fast) {
    slow = head;

    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }

    return slow;
}
```

这道题的时间复杂度为：

```text
O(n)
```

空间复杂度为：

```text
O(1)
```

---

## 今日易错点

1. 左值和右值不能根据它们位于等号哪一边来判断。
2. `int&& r = 10` 中，`10` 是右值，但有名字的 `r` 本身是左值。
3. 要区分“变量类型”和“表达式值类别”。
4. `T&` 通常绑定左值，`T&&` 通常绑定右值。
5. 链表快指针移动两步前必须检查 `fast` 和 `fast->next`。
6. 找环入口时，第一个指针回到 `head` 后，两个指针都只能一次走一步。
7. `&&` 具有短路求值，可以避免访问空指针成员。

---

## 今日总结

今天开始正式接触 C++ 的值类别和右值引用。

最重要的是理解：

```text
有名字的变量通常作为左值使用
临时值通常是右值
T& 绑定左值
T&& 可以绑定右值
```

同时进一步明确了：

```cpp
int&& r = 10;
```

并不意味着后面所有的 `r` 都是右值。

LeetCode 142 则继续强化了链表指针操作，并学会了利用快慢指针同时完成“判断环”和“寻找环入口”。

这些内容为下一步学习 `std::move` 和移动语义打下了基础。
