# Day09 学习总结

## 今日学习内容

今天主要学习了 C++ 中的 `std::weak_ptr`、`shared_ptr` 循环引用问题，以及如何使用 `weak_ptr` 打破循环引用。

通过 `Person` 相互引用的案例，观察了两个对象互相使用 `shared_ptr` 时引用计数无法归零的问题；随后将对象之间的关系改为 `weak_ptr`，使对象能够正常析构。

还通过一个单独的观察者练习，进一步理解了：

```text
weak_ptr 不增加强引用计数
lock() 可以临时获得 shared_ptr
expired() 可以判断资源是否已经销毁
```

算法部分完成了 LeetCode 24「两两交换链表中的节点」，使用虚拟头节点和指针操作，在不交换节点值的情况下原地交换链表节点。

---

### 1. shared_ptr 循环引用

昨天学习了 `shared_ptr` 的引用计数：

```text
创建或复制 shared_ptr
        ↓
强引用计数增加

shared_ptr reset 或析构
        ↓
强引用计数减少

强引用计数变成 0
        ↓
资源自动释放
```

但是，如果两个对象互相使用 `shared_ptr` 保存对方，就可能发生循环引用。

例如 `Person` 类中保存：

```cpp
std::shared_ptr<Person> partner_;
```

创建 Alice 和 Bob：

```cpp
auto alice =
    std::make_shared<Person>("Alice");

auto bob =
    std::make_shared<Person>("Bob");
```

然后让两人互相保存对方：

```cpp
alice->setPartner(bob);
bob->setPartner(alice);
```

此时的所有权关系是：

```text
alice变量 ──────────> Alice对象
                        │
                        └──shared_ptr──> Bob对象
                                            ↑
bob变量 ────────────────────────────────────┘

同时：

Bob对象 ──shared_ptr──> Alice对象
```

Alice 和 Bob 互相拥有对方，形成一个闭环。

---

### 2. 循环引用为什么导致资源无法释放

建立相互引用后，Alice 的所有者包括：

```text
alice 变量
Bob 对象内部的 partner_
```

所以 Alice 的引用计数为：

```text
2
```

Bob 的所有者包括：

```text
bob 变量
Alice 对象内部的 partner_
```

所以 Bob 的引用计数也是：

```text
2
```

离开局部作用域时，局部变量 `alice` 和 `bob` 被销毁：

```text
Alice count：2 -> 1
Bob count：2 -> 1
```

但两个对象内部仍然互相拥有：

```text
Alice ──shared_ptr──> Bob
  ↑                    │
  └────shared_ptr──────┘
```

因此两个对象的引用计数都无法变成 `0`。

最终结果是：

```text
Alice 析构函数没有执行
Bob 析构函数没有执行
资源没有释放
发生内存泄漏
```

程序虽然已经离开局部作用域，但没有输出：

```text
Alice destroyed
Bob destroyed
```

这说明 `shared_ptr` 虽然可以自动管理资源，但并不能自动解决循环引用。

---

### 3. weak_ptr 的基本作用

`std::weak_ptr` 是一种弱引用智能指针。

它可以观察由 `shared_ptr` 管理的资源，但不会取得该资源的共享所有权。

例如：

```cpp
auto owner =
    std::make_shared<Resource>(1);

std::weak_ptr<Resource> observer =
    owner;
```

此时：

```text
owner
    ↓
shared_ptr
    ↓
真正拥有 Resource

observer
    ↓
weak_ptr
    ↓
只观察 Resource
```

创建 `owner` 后：

```text
owner.use_count() == 1
```

再创建 `observer` 后：

```text
owner.use_count() == 1
```

引用计数没有增加。

因此今天最重要的结论是：

> `weak_ptr` 不增加对象的强引用计数，也不会阻止对象被销毁。

---

### 4. 使用 weak_ptr 打破循环引用

将 `Person` 类中的：

```cpp
std::shared_ptr<Person> partner_;
```

改成：

```cpp
std::weak_ptr<Person> partner_;
```

此时 Alice 和 Bob 的关系变成：

```text
alice ──shared_ptr，拥有──> Alice对象
                              │
                              └──weak_ptr，观察──> Bob对象

bob ───shared_ptr，拥有──> Bob对象
                            │
                            └──weak_ptr，观察──> Alice对象
```

Alice 和 Bob 仍然可以互相知道对方，但不再互相拥有。

因此相互设置 `partner_` 后：

```text
Alice count = 1
Bob count = 1
```

离开作用域时：

```text
alice shared_ptr 析构
        ↓
Alice count：1 -> 0
        ↓
Alice 对象销毁

bob shared_ptr 析构
        ↓
Bob count：1 -> 0
        ↓
Bob 对象销毁
```

两个对象都能够正常执行析构函数。

---

### 5. weak_ptr 为什么不能直接访问对象

`shared_ptr` 可以直接使用：

```cpp
owner->use();
```

但 `weak_ptr` 不能直接写：

```cpp
observer->use();
```

也不能直接写：

```cpp
*observer;
```

因为 `weak_ptr` 不拥有对象，它观察的对象可能已经被销毁。

如果允许直接访问，就可能访问一块已经释放的内存。

因此，使用 `weak_ptr` 访问资源之前，需要先调用：

```cpp
observer.lock()
```

---

### 6. lock() 的作用

`lock()` 会尝试把 `weak_ptr` 转换成一个临时的 `shared_ptr`：

```cpp
std::shared_ptr<Resource> temporary =
    observer.lock();
```

如果资源仍然存在：

```text
lock()
    ↓
返回有效的 shared_ptr
```

此时可以安全访问：

```cpp
if (temporary != nullptr) {
    temporary->use();
}
```

如果资源已经销毁：

```text
lock()
    ↓
返回空的 shared_ptr
```

此时：

```cpp
temporary == nullptr
```

因此常见写法是：

```cpp
if (auto temporary = observer.lock()) {
    temporary->use();
}
```

只有当 `lock()` 成功时，才通过临时 `shared_ptr` 访问资源。

---

### 7. lock() 与引用计数的变化

需要区分 `weak_ptr` 本身和 `lock()` 返回的 `shared_ptr`。

创建观察者：

```cpp
observer = owner;
```

不会增加强引用计数：

```text
owner count = 1
```

但是执行：

```cpp
auto temporary = observer.lock();
```

会创建一个临时 `shared_ptr`：

```text
owner ─────────┐
               ├──> Resource
temporary ─────┘
```

因此在 `temporary` 存在期间：

```text
owner count = 2
```

当 `temporary` 离开作用域时：

```text
count：2 -> 1
```

所以应该准确理解为：

```text
weak_ptr 本身
    ↓
不增加强引用计数

weak_ptr.lock()
    ↓
返回临时 shared_ptr
    ↓
临时增加强引用计数
```

---

### 8. expired() 的作用

可以使用：

```cpp
observer.expired()
```

判断 `weak_ptr` 观察的对象是否已经销毁。

当对象仍然存在时：

```cpp
observer.expired() == false
```

当最后一个 `shared_ptr` 放弃所有权后：

```cpp
owner.reset();
```

强引用计数变成：

```text
0
```

资源被销毁，此时：

```cpp
observer.expired() == true
```

`expired()` 返回 `true` 并不代表 `observer` 这个变量被销毁，而是表示它原来观察的资源已经不存在。

真正访问资源时，仍然应该使用 `lock()`，因为 `lock()` 可以在检查资源存在的同时，临时保证访问期间资源不会被销毁。

---

### 9. weak_ptr 观察者练习

今天通过下面的流程观察了引用计数：

```text
创建 owner
    ↓
count = 1

observer = owner
    ↓
weak_ptr 不拥有资源
    ↓
count 仍然为 1

temporary = observer.lock()
    ↓
得到临时 shared_ptr
    ↓
count = 2

temporary 离开作用域
    ↓
count = 1

owner.reset()
    ↓
count = 0
    ↓
Resource destroyed

observer.expired()
    ↓
true

再次调用 observer.lock()
    ↓
返回空 shared_ptr
```

运行结果为：

```text
Resource 1 created
1. owner count: 1
2. after weak_ptr: 1
3. expired: false
Using Resource 1
4. inside block: 2
5. after block: 1
Resource 1 destroyed
6. expired: true
7. Resource no longer exists
```

这个实验完整验证了 `weak_ptr` 的观察者性质。

---

### 10. shared_ptr 与 weak_ptr 的分工

目前可以把两者理解为：

```text
shared_ptr
    ↓
真正拥有资源
    ↓
参与强引用计数
    ↓
决定资源生命周期

weak_ptr
    ↓
只观察资源
    ↓
不参与共享所有权
    ↓
不能阻止资源销毁
```

`weak_ptr` 不能单独创建并管理一个动态对象。

通常需要先存在：

```cpp
std::shared_ptr<T>
```

然后再让 `weak_ptr` 观察它：

```cpp
std::weak_ptr<T> observer = owner;
```

所以 `weak_ptr` 通常与 `shared_ptr` 配合使用。

---

### 11. LeetCode 24：两两交换链表节点

今天完成了 LeetCode 24「两两交换链表中的节点」。

原链表：

```text
1 -> 2 -> 3 -> 4 -> nullptr
```

交换后：

```text
2 -> 1 -> 4 -> 3 -> nullptr
```

如果链表节点数量为奇数：

```text
1 -> 2 -> 3 -> nullptr
```

交换后：

```text
2 -> 1 -> 3 -> nullptr
```

最后一个不能组成一对的节点保持不变。

题目要求交换节点本身，不能只交换：

```cpp
node->val
```

因此需要真正修改节点之间的 `next` 指针。

---

### 12. 使用虚拟头节点

在链表前面添加一个虚拟头节点：

```text
dummy -> 1 -> 2 -> 3 -> 4
```

代码为：

```cpp
ListNode dummyHead(0);
dummyHead.next = head;

ListNode* cur = &dummyHead;
```

`cur` 始终指向当前需要交换的两个节点之前的节点。

第一次交换时：

```text
cur
 ↓
dummy -> 1 -> 2 -> 3 -> 4
```

需要交换的是：

```text
1 和 2
```

使用虚拟头节点后，第一组节点和后面的节点可以使用相同的处理方式，不需要单独修改 `head`。

---

### 13. 保存三个关键位置

每次交换前，先保存三个节点：

```cpp
ListNode* first = cur->next;
ListNode* second = first->next;
ListNode* nextNode = second->next;
```

此时：

```text
cur       first  second  nextNode
 ↓          ↓      ↓       ↓
dummy  ->   1  ->  2  ->   3 -> 4
```

各变量作用为：

```text
first
    ↓
当前一组中的第一个节点

second
    ↓
当前一组中的第二个节点

nextNode
    ↓
下一组链表的入口
```

提前保存 `nextNode` 是为了避免交换当前两个节点后丢失后面的链表。

---

### 14. 两两交换的指针操作

核心操作为：

```cpp
second->next = first;
cur->next = second;
first->next = nextNode;
```

第一句：

```cpp
second->next = first;
```

把：

```text
1 -> 2
```

变成：

```text
2 -> 1
```

第二句：

```cpp
cur->next = second;
```

让前面的链表连接交换后的头节点：

```text
dummy -> 2 -> 1
```

第三句：

```cpp
first->next = nextNode;
```

把后面的链表重新连接回来：

```text
dummy -> 2 -> 1 -> 3 -> 4
```

交换完成后：

```cpp
cur = first;
```

因为原来的 `first` 已经变成本组交换后的最后一个节点。

下一轮就可以从它后面的节点继续交换。

---

### 15. 循环条件与短路判断

循环条件为：

```cpp
while (cur->next != nullptr &&
       cur->next->next != nullptr)
```

含义是：

```text
cur->next != nullptr
        ↓
后面至少还有第一个节点

cur->next->next != nullptr
        ↓
后面至少还有第二个节点
```

只有剩余节点至少有两个时，才能进行交换。

这里还使用了 `&&` 的短路特性：

```text
先判断 cur->next
        ↓
如果为空，后面的条件不再执行
        ↓
不会访问 nullptr->next
```

因此判断顺序不能随意颠倒。

---

### 16. 为什么返回 dummyHead.next

交换前：

```text
dummy -> 1 -> 2 -> 3 -> 4
```

交换后：

```text
dummy -> 2 -> 1 -> 4 -> 3
```

真正的新头节点是：

```cpp
dummyHead.next
```

所以最终返回：

```cpp
return dummyHead.next;
```

虽然 `dummyHead` 是局部对象，函数结束后会销毁，但返回的是它保存的真实链表节点地址，而不是 `dummyHead` 自己的地址，因此不会产生悬空指针。

---

### 17. 两两交换链表的复杂度

程序只遍历一遍链表：

```text
时间复杂度：O(n)
```

交换过程中只使用了固定数量的指针：

```text
cur
first
second
nextNode
```

没有创建新的链表节点，所以：

```text
额外空间复杂度：O(1)
```

这是一种原地操作。

---

## 今日易错点

1. 两个对象互相使用 `shared_ptr` 保存对方时，可能形成循环引用。

2. 循环引用会导致引用计数无法变成 `0`，对象的析构函数无法执行。

3. `shared_ptr` 可以自动释放资源，但不能自动解决循环引用。

4. `weak_ptr` 只观察资源，不拥有资源，也不会增加强引用计数。

5. `weak_ptr` 不能直接使用 `->` 或 `*` 访问对象。

6. 使用 `weak_ptr` 前需要调用 `lock()`，得到临时的 `shared_ptr`。

7. `weak_ptr` 本身不增加强引用计数，但 `lock()` 返回的临时 `shared_ptr` 会暂时增加引用计数。

8. `expired()` 返回 `true` 表示被观察的资源已经销毁，不表示 `weak_ptr` 变量本身不存在。

9. `lock()` 失败后返回空 `shared_ptr`，访问前必须进行判断。

10. 类型名必须正确书写：

    ```cpp
    Resource
    ```

    不能误写成：

    ```cpp
    Rsource
    ```

11. `use_count` 是成员函数，调用时不能漏掉括号：

    ```cpp
    owner.use_count()
    ```

12. C++ 语句末尾必须使用英文分号：

    ```cpp
    ;
    ```

    不能使用中文分号：

    ```text
    ；
    ```

13. 输出字符串中的 `created` 容易误写成 `craeted`，虽然不影响逻辑，但应保持代码规范。

14. LeetCode 24 要交换节点连接关系，不能只交换节点中的 `val`。

15. 两两交换之前必须保存 `nextNode`，否则可能丢失后面的链表。

16. `cur` 必须移动到本轮交换后的末尾，也就是原来的 `first`。

17. 循环条件必须先判断 `cur->next`，再判断 `cur->next->next`，避免访问空指针。

18. 奇数长度链表的最后一个节点无法组成一对，应保持原位置不变。

19. 函数最后返回的是：

    ```cpp
    dummyHead.next
    ```

    不能返回 `&dummyHead`，因为 `dummyHead` 是局部对象。

20. 本题不需要重新 `new` 节点，额外空间复杂度为 `O(1)`。

---

## 今日总结

今天在 `shared_ptr` 引用计数的基础上，进一步学习了 `weak_ptr` 和循环引用。

首先通过两个 `Person` 对象互相使用 `shared_ptr` 保存对方，复现了循环引用：

```text
Alice 拥有 Bob
Bob 又拥有 Alice
        ↓
引用计数无法归零
        ↓
析构函数无法执行
        ↓
发生内存泄漏
```

然后将对象之间的关系改为 `weak_ptr`：

```text
外部 shared_ptr
    ↓
真正拥有 Person 对象

对象内部 weak_ptr
    ↓
只观察另一个 Person
    ↓
不阻止对方销毁
```

目前已经能够理解：

```text
shared_ptr
    ↓
表示共享所有权
    ↓
决定资源生命周期

weak_ptr
    ↓
表示观察关系
    ↓
不拥有资源
```

同时掌握了 `weak_ptr` 的两个重要操作：

```text
lock()
    ↓
尝试获得临时 shared_ptr
    ↓
安全访问资源

expired()
    ↓
判断被观察的资源是否已经销毁
```

算法部分通过 LeetCode 24 继续练习链表指针操作。

今天不仅需要保存当前交换的两个节点，还需要保存下一组链表的入口：

```text
first
    ↓
当前第一个节点

second
    ↓
当前第二个节点

nextNode
    ↓
后续链表入口
```

两两交换的核心过程是：

```text
保存当前两个节点和后续入口
        ↓
second 指向 first
        ↓
前面的节点连接 second
        ↓
first 连接后续链表
        ↓
cur 移动到本组末尾
```

到目前为止，智能指针学习进度为：

```text
Day07：unique_ptr
       独占所有权

Day08：shared_ptr
       共享所有权和引用计数

Day09：weak_ptr
       弱引用、资源观察和循环引用
```

链表学习进度为：

```text
LeetCode 203：移除链表元素
LeetCode 707：设计链表
LeetCode 206：反转链表
LeetCode 24：两两交换链表节点
```

对于智能指针的所有权和链表中的指针操作，已经形成了更加完整的基础认识。
