# Day07 学习总结

## 今日学习内容

今天主要学习了 C++ 中的 `std::unique_ptr`、资源所有权，以及单链表的设计与内存管理。

### 1. unique_ptr 与资源所有权

`std::unique_ptr` 用来独占管理一个动态资源。

```cpp
std::unique_ptr<int> p =
    std::make_unique<int>(10);
```

相比裸指针：

```cpp
int* p = new int(10);
delete p;
```

`unique_ptr` 不需要手动 `delete`，当对象离开作用域时，会自动释放所管理的资源。

`unique_ptr` 不允许复制：

```cpp
// 错误
std::unique_ptr<int> p2 = p1;
```

因为同一份资源如果同时被两个 `unique_ptr` 拥有，可能造成重复释放。

但可以通过 `std::move` 转移所有权：

```cpp
std::unique_ptr<int> p2 = std::move(p1);
```

转移之后：

```text
p1 -> nullptr
p2 -> 原来的资源
```

`p1` 对象本身仍然存在，只是不再拥有资源。

这让我进一步理解了 RAII：

> 对象负责自己拥有的资源，对象生命周期结束时自动完成资源释放。

---

## 2. LeetCode 707：设计链表

今天实现了一个带虚拟头节点的单链表：

```text
dummy -> node -> node -> nullptr
```

链表类中保存：

```cpp
ListNode* dummyHead_;
int size_;
```

使用虚拟头节点可以统一处理头节点和普通节点的插入、删除操作。

### get

通过下标判断和指针遍历找到指定节点：

```cpp
ListNode* cur = dummyHead_->next;

for (int i = 0; i < index; ++i) {
    cur = cur->next;
}
```

需要注意合法下标范围：

```cpp
0 <= index < size_
```

因此判断应该写：

```cpp
if (index < 0 || index >= size_) {
    return -1;
}
```

---

## 3. 链表插入

头部插入的关键是先让新节点连接旧头节点：

```cpp
newNode->next = dummyHead_->next;
dummyHead_->next = newNode;
```

不能先修改 `dummyHead_->next`，否则可能丢失原来的链表。

普通位置插入的核心操作：

```cpp
newNode->next = cur->next;
cur->next = newNode;
```

理解为：

```text
新节点先连接后面的节点
前一个节点再连接新节点
```

`addAtIndex()` 中：

```cpp
index == 0
```

可以自动完成头插；

```cpp
index == size_
```

可以自动完成尾插。

---

## 4. 链表删除

删除节点时不能只修改指针：

```cpp
cur->next = cur->next->next;
```

因为节点是通过 `new` 创建的，这样会造成内存泄漏。

应该先保存待删除节点：

```cpp
ListNode* temp = cur->next;

cur->next = temp->next;

delete temp;
```

因此链表删除不仅是修改结构，还需要正确释放动态内存。

---

## 5. while(cur) 和 while(cur->next)

今天进一步理解了两种链表遍历方式的区别。

```cpp
while (cur != nullptr)
```

会一直遍历到：

```text
cur == nullptr
```

而：

```cpp
while (cur->next != nullptr)
```

循环结束时：

```text
cur == 最后一个节点
```

因此寻找尾节点时应该使用：

```cpp
while (cur->next != nullptr)
```

---

## 6. 析构函数与 RAII

由于链表节点都是通过：

```cpp
new ListNode(...)
```

动态创建的，所以链表对象销毁时必须释放所有节点。

析构函数中需要先保存下一个节点：

```cpp
ListNode* nextNode = cur->next;
delete cur;
cur = nextNode;
```

不能写：

```cpp
delete cur;
cur = cur->next;
```

因为 `delete` 之后继续访问 `cur->next` 属于访问已经释放的内存。

这进一步体现了 RAII：

```text
MyLinkedList 生命周期结束
        ↓
自动调用析构函数
        ↓
释放整条链表
```

---

## 7. clear 与析构函数的区别

`clear()` 只删除真实节点：

```text
dummy -> 10 -> 20 -> 30

变成

dummy -> nullptr
```

因此应该从：

```cpp
dummyHead_->next
```

开始删除。

而析构函数需要把虚拟头节点也释放，所以从：

```cpp
dummyHead_
```

开始删除。

两者区别：

```text
clear()：
保留链表对象和 dummy node

析构函数：
整个链表对象即将结束，所有节点都释放
```

---

## 今日易错点

1. `get()` 判断越界时应该使用 `index >= size_`。
2. 头插时要先保存原来的链表连接关系。
3. 找尾节点应该使用 `while (cur->next != nullptr)`。
4. 删除节点不仅要断开链表，还需要 `delete`。
5. `delete` 一个指针后不能再访问该对象的成员。
6. `clear()` 不能把 `dummyHead_` 删除。
7. 使用裸指针管理资源时，需要考虑析构、复制和所有权问题。

---

## 今日总结

今天把前几天学习的指针、动态内存、链表和 RAII 串联了起来。

相比单纯会写链表操作，更重要的是开始理解：

```text
谁创建资源
谁拥有资源
谁负责释放资源
什么时候释放资源
```

`unique_ptr` 是现代 C++ 中自动管理独占资源的重要工具，而自己实现链表则让我更加直观地理解了 RAII 和资源所有权。

