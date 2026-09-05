# Day 06 学习总结：RAII 与链表元素删除

## 一、今日学习内容

今天主要学习了以下内容：

1. RAII 的基本思想；
2. 构造函数与析构函数在资源管理中的作用；
3. 使用 `FileHandle` 自动管理文件资源；
4. 提前 `return` 和离开作用域时的析构过程；
5. RAII 对象的资源所有权；
6. 单链表节点的基本结构；
7. 使用指针遍历单链表；
8. 原地删除链表中的目标节点；
9. LeetCode 203：移除链表元素。

---

## 二、RAII 的基本概念

RAII 的全称是：

> Resource Acquisition Is Initialization

可以理解为：

> 在对象构造时获取资源，在对象析构时释放资源。

RAII 不是 Linux 系统自动提供的文件管理功能，而是建立在
C++ 对象生命周期机制上的一种资源管理方式。

程序员负责：

- 在构造函数中获取资源；
- 在析构函数中释放资源；
- 确定哪个对象拥有资源。

C++ 负责：

- 创建对象时自动调用构造函数；
- 对象离开作用域时自动调用析构函数；
- 函数提前 `return` 时销毁局部对象；
- 正常异常展开时销毁已经构造的局部对象。

---

## 三、使用 FileHandle 管理文件

`FileHandle` 类内部保存一个文件指针：

```cpp
std::FILE* file_;
```

构造函数通过 `fopen()` 打开文件：

```cpp
explicit FileHandle(const char* filename)
    : file_(std::fopen(filename, "w")) {
}
```

析构函数通过 `fclose()` 关闭文件：

```cpp
~FileHandle() {
    if (file_ != nullptr) {
        std::fclose(file_);
        file_ = nullptr;
    }
}
```

因此，文件资源的生命周期和 `FileHandle` 对象绑定在了一起：

```text
创建 FileHandle 对象
→ 打开文件
→ 使用文件
→ 对象离开作用域
→ 自动C++ 自动调用析构函数
→ 关闭文件
```

即使函数提前执行：

```cpp
return;
```

局部对象仍会先被销毁，因此文件仍然能够正确关闭。

---

## 四、作用域实验

在内部作用域中创建文件对象：

```cpp
{
    FileHandle file("scope_output.txt");
    file.write("测试内部作用域\n");
}
```

程序执行到右花括号 `}` 时，`file` 离开作用域，析构函数立即执行。

实验中观察到的顺序是：

```text
即将离开内部作用域
文件已经关闭
已经离开内部作用域
```

这说明析构函数不一定要等整个程序结束才执行，而是在对象生命周期结束时执行。

---

## 五、RAII 对象与复制问题

如果直接复制 `FileHandle`：

```cpp
FileHandle first("test.txt");
FileHandle second = first;
```

默认复制只会复制其中的文件指针，两个对象将指向同一个文件资源。

两个对象析构时可能对同一个资源执行两次 `fclose()`，产生未定义行为。

因此可以禁止复制：

```cpp
FileHandle(const FileHandle&) = delete;
FileHandle& operator=(const FileHandle&) = delete;
```

这体现了资源所有权原则：

> 一个不能安全复制的资源，应当只由一个对象负责释放。

昨天学习的 `Buffer` 和 `NumberBox` 也使用了 RAII：

- 构造函数通过 `new` 获取内存；
- 析构函数通过 `delete` 或 `delete[]` 释放内存；
- 拷贝构造函数通过深拷贝避免多个对象管理同一块内存。

---

## 六、单链表结构

单链表节点包含数据和下一个节点的地址：

```cpp
struct ListNode {
    int val;
    ListNode* next;
};
```

例如：

```text
10 → 20 → 30 → nullptr
```

其中：

- `val` 保存节点数值；
- `next` 指向下一个节点；
- `nullptr` 表示链表结束。

链表的基本遍历模板是：

```cpp
ListNode* current = head;

while (current != nullptr) {
    std::cout << current->val << '\n';
    current = current->next;
}
```

因为 `current` 是指针，所以使用箭头运算符访问成员：

```cpp
current->val
current->next
```

`current->val` 等价于：

```cpp
(*current).val
```

---

## 七、LeetCode 203：移除链表元素

题目要求删除链表中所有数值等于 `val` 的节点，并返回新的头节点。

本次使用原地删除方法：

1. 先处理开头连续需要删除的节点；
2. 使用 `current` 遍历剩余链表；
3. 检查 `current->next` 是否需要删除；
4. 修改指针，让链表跳过目标节点；
5. 使用 `delete` 释放被移除节点的内存。

删除节点的核心代码是：

```cpp
ListNode* nodeToDelete = current->next;

current->next = nodeToDelete->next;

delete nodeToDelete;
```

例如：

```text
current
   ↓
   1 → 6 → 2
```

修改指针后：

```text
current
   ↓
   1 ─────→ 2
```

随后释放节点 `6`。

---

## 八、连续节点删除

如果链表为：

```text
1 → 6 → 6 → 2
```

删除第一个 `6` 后，`current` 不能马上向后移动，因为新的
`current->next` 仍然可能是 `6`。

因此代码结构为：

```cpp
if (current->next->val == val) {
    // 删除节点，current 不移动
} else {
    current = current->next;
}
```

只有当前节点后面的节点不需要删除时，才移动 `current`。

---

## 九、算法复杂度

每个节点最多访问一次，因此：

```text
时间复杂度：O(n)
```

算法只使用少量指针变量，没有创建新的结果链表，因此：

```text
额外空间复杂度：O(1)
```

这属于原地修改链表。

---

## 十、今天容易出现的问题

### 1. 删除节点后继续访问该节点

错误思路：

```cpp
delete head;
head = head->next;
```

`delete head` 后，不能再访问 `head->next`。

正确顺序：

```cpp
ListNode* nodeToDelete = head;
head = head->next;
delete nodeToDelete;
```

### 2. 忘记处理头节点

头节点没有前驱节点，所以需要单独修改：

```cpp
head = head->next;
```

### 3. 删除节点后错误移动 current

删除后应该继续检查新的 `current->next`，否则可能漏掉连续目标节点。

### 4. 只断开节点但不释放内存

修改 `next` 可以把节点移出链表，但实际项目中还要根据资源所有权决定是否执行：

```cpp
delete nodeToDelete;
```

---

## 十一、今日总结

今天进一步理解了构造函数和析构函数不只是类的语法，而是
C++ 管理资源的重要基础。

RAII 的核心是：

> 把资源的生命周期绑定到对象的生命周期。

同时学习了单链表的节点结构、指针遍历和原地删除方法，并完成了
LeetCode 203。链表删除的关键不是修改节点数值，而是正确改变节点之间的
`next` 指向，同时安全释放不再使用的节点。
