# Day10 学习总结

## 今日学习内容

今天对前几天学习的 `std::unique_ptr`、`std::shared_ptr` 和
`std::weak_ptr` 进行了综合复习，重点不再只是记忆三种智能指针的语法，
而是根据对象之间的所有权与生命周期选择合适的类型。

今天进一步学习了：

```text
unique_ptr<T>：独占拥有对象
shared_ptr<T>：多个对象共同拥有资源
weak_ptr<T>：观察 shared_ptr 管理的对象
T*：不拥有对象，保存对象地址
T&：不拥有对象，只在当前调用中使用
```

最重要的认识是：

> 使用一个对象，并不代表拥有这个对象。

编程部分完成了智能指针综合演示和类关系练习；算法部分完成了
LeetCode 19「删除链表的倒数第 N 个节点」，分别练习了长度法和快慢双指针法。

---

## 1. 从所有权开始选择指针

看到两个类之间存在关系时，不应该立刻猜测使用哪一种智能指针，而应该先问：

```text
谁创建这个对象？
谁负责保证它继续存活？
谁消失以后，它才可以销毁？
当前对象只是使用它，还是需要拥有它？
```

“能访问对象”和“拥有对象”是两件不同的事情。

例如，一个函数只在调用期间使用 `Logger`：

```cpp
void printMessage(Logger& logger) {
    logger.log("hello");
}
```

函数使用了 `Logger`，但是并不负责它的生命周期，所以使用引用即可，
没有必要使用 `shared_ptr<Logger>`。

---

## 2. unique_ptr：独占所有权

如果一个对象只有一个明确的所有者，优先使用 `std::unique_ptr`。

例如，`Application` 独占自己的 `Logger`：

```cpp
class Application {
private:
    std::unique_ptr<Logger> logger_;
};
```

它表达的关系是：

```text
Logger 只属于 Application
Application 负责 Logger 的生命周期
Application 销毁时，Logger 自动销毁
```

推荐使用 `std::make_unique` 创建对象：

```cpp
auto resource = std::make_unique<Resource>(1);
```

`unique_ptr` 不能复制：

```cpp
auto firstOwner = std::make_unique<Resource>(1);

// 错误：不能复制独占所有权
// std::unique_ptr<Resource> secondOwner = firstOwner;
```

但是可以使用 `std::move` 转移所有权：

```cpp
std::unique_ptr<Resource> secondOwner =
    std::move(firstOwner);
```

移动以后：

```text
firstOwner 变为空
secondOwner 获得资源的所有权
底层 Resource 没有被复制
```

目前需要记住：

> `unique_ptr` 不能复制，但是可以移动。

---

## 3. shared_ptr：共享所有权

如果多个对象都必须保证同一份资源存活，可以使用 `std::shared_ptr`。

例如，`Server` 和 `Worker` 共同使用一条连接：

```text
Server ──shared_ptr──┐
                    ├──> Connection
Worker ──shared_ptr──┘
```

可以写成：

```cpp
auto connection = std::make_shared<Connection>(1001);

std::shared_ptr<Connection> serverConnection = connection;
std::shared_ptr<Connection> workerConnection = connection;
```

三个 `shared_ptr` 指向的是同一个 `Connection`，并不是复制了三个对象。

引用计数变化如下：

```text
创建 connection                use_count = 1
复制给 serverConnection        use_count = 2
复制给 workerConnection        use_count = 3
一个所有者销毁                 use_count = 2
最后一个所有者销毁             use_count = 0
Connection 自动析构
```

因此，`shared_ptr` 表达的是：

> 我也是对象的所有者之一，只要我还持有它，对象就必须继续存活。

只有确实存在共享生命周期时，才应该使用 `shared_ptr`，不能只是为了复制方便
就把所有成员都写成 `shared_ptr`。

---

## 4. weak_ptr：不拥有对象的观察者

如果一个对象只想观察由 `shared_ptr` 管理的资源，但不应该延长资源生命周期，
可以使用 `std::weak_ptr`。

例如：

```text
Server ──shared_ptr──┐
                    ├──> Connection
Worker ──shared_ptr──┘

Monitor - - weak_ptr - -> Connection
```

代码可以写成：

```cpp
std::shared_ptr<Connection> owner =
    std::make_shared<Connection>(1001);

std::weak_ptr<Connection> observer = owner;
```

创建 `weak_ptr` 不会增加强引用计数：

```text
创建 owner 后               use_count = 1
创建 observer 后            use_count 仍然为 1
```

所以即使 `observer` 仍然存在，只要最后一个 `shared_ptr` 被销毁，
`Connection` 仍然会正常释放。

---

## 5. weak_ptr 必须通过 lock() 访问对象

不能直接通过 `weak_ptr` 访问对象：

```cpp
// 错误
// observer->send("hello");
```

因为 `weak_ptr` 不拥有对象，它观察的对象可能已经销毁。

正确写法是先调用 `lock()`：

```cpp
if (auto connection = observer.lock()) {
    connection->send("health check");
} else {
    std::cout << "Connection has expired\n";
}
```

`lock()` 的结果分为两种情况：

```text
对象仍然存在
→ 返回有效的 shared_ptr
→ 可以安全访问对象

对象已经销毁
→ 返回空的 shared_ptr
→ if 条件不成立
```

需要注意：

```text
weak_ptr 本身不会增加强引用计数
lock() 成功返回的临时 shared_ptr 会暂时增加强引用计数
```

也可以用 `expired()` 判断对象是否已经销毁：

```cpp
if (observer.expired()) {
    std::cout << "Resource has expired\n";
}
```

但真正访问对象时，更推荐直接使用 `lock()`。

---

## 6. T&：只在当前调用中借用对象

如果函数只在调用期间使用一个一定存在的对象，并且不保存它，通常使用引用：

```cpp
void process(Resource& resource) {
    resource.use();
}
```

如果不需要修改对象，可以使用常量引用：

```cpp
void printResource(const Resource& resource) {
    resource.print();
}
```

引用表达的是：

```text
对象不是我的
我只在这次函数调用中使用它
调用者保证对象有效
```

---

## 7. T*：保存一个非拥有地址

裸指针并不一定是不安全的。`T*` 可以用于表达：

```text
当前对象不拥有目标对象
需要长期保存目标对象的地址
外部结构能够保证目标对象的生命周期
```

例如：

```text
Game
 └── unique_ptr<Player>

HUD 只负责显示 Player 的状态
并且程序保证 Player 一定比 HUD 活得久
```

此时可以写成：

```cpp
class HUD {
private:
    Player* player_;
};
```

原因是：

```text
Game 拥有 Player
HUD 不拥有 Player
HUD 只保存 Player 的地址
外部保证 HUD 使用期间 Player 一定存在
```

因此没有必要为了让 `HUD` 访问 `Player`，就把整个系统都改成
`shared_ptr<Player>`。

---

## 8. T* 与 weak_ptr<T> 的区别

`T*` 和 `weak_ptr<T>` 都可以表达“不拥有”，但使用条件不同。

### 使用 T*

```text
不拥有对象
需要保存对象地址
能够确定对象不会提前销毁
```

### 使用 weak_ptr<T>

```text
不拥有对象
对象由 shared_ptr 管理
对象可能提前销毁
每次使用前需要检查对象是否仍然存在
```

可以简单记成：

```text
生命周期确定的非拥有关系
→ T*

生命周期不确定的 shared_ptr 观察关系
→ weak_ptr<T>
```

---

## 9. weak_ptr 不能直接观察 unique_ptr

下面的写法是错误的：

```cpp
std::unique_ptr<Player> player =
    std::make_unique<Player>();

// 错误：weak_ptr 不能观察 unique_ptr
// std::weak_ptr<Player> observer = player;
```

`weak_ptr` 依赖 `shared_ptr` 的控制块，只能观察由 `shared_ptr` 管理的对象：

```cpp
std::shared_ptr<Player> player =
    std::make_shared<Player>();

std::weak_ptr<Player> observer = player;
```

因此，不能只看某一个成员的需要，还要分析整个系统采用了哪一种所有权结构。

---

## 10. 智能指针的选择顺序

以后遇到类成员类型选择时，可以按照下面的顺序判断。

### 第一步：是否真的需要动态分配

如果对象可以直接作为成员，最简单的写法可能是：

```cpp
class Application {
private:
    Logger logger_;
};
```

普通对象也可以直接放入容器：

```cpp
std::vector<Track> tracks_;
```

只有在需要动态生命周期、多态或稳定地址等情况下，才需要考虑智能指针。

### 第二步：当前对象是否拥有目标对象

```text
只在函数调用期间临时使用
→ T& 或 const T&

长期保存地址，生命周期由外部保证
→ T*

长期观察，目标可能提前销毁，并由 shared_ptr 管理
→ weak_ptr<T>
```

### 第三步：如果拥有，有几个所有者

```text
只有一个明确所有者
→ unique_ptr<T>

多个对象必须共同保证资源存活
→ shared_ptr<T>
```

最终可以浓缩成：

```text
临时借用                    → T&
非拥有，生命周期确定         → T*
独占拥有                    → unique_ptr<T>
共同拥有                    → shared_ptr<T>
非拥有，生命周期不确定       → weak_ptr<T>
```

---

## 11. 综合类关系练习

今天在 `smart_pointer_practice.cpp` 中练习了下面的对象关系：

```text
Application
     │
     └── unique_ptr<Logger>

Server ──shared_ptr──┐
                    ├──> Connection
Worker ──shared_ptr──┘

Monitor - - weak_ptr - -> Connection
```

### Application 与 Logger

```cpp
class Application {
private:
    std::unique_ptr<Logger> logger_;
};
```

`Logger` 完全属于 `Application`，所以使用 `unique_ptr`。

### Server、Worker 与 Connection

```cpp
class Server {
private:
    std::shared_ptr<Connection> connection_;
};

class Worker {
private:
    std::shared_ptr<Connection> connection_;
};
```

`Server` 和 `Worker` 都需要保证连接存活，所以使用 `shared_ptr`。

### Monitor 与 Connection

```cpp
class Monitor {
private:
    std::weak_ptr<Connection> connection_;
};
```

`Monitor` 只观察连接，不负责维持连接，所以使用 `weak_ptr`。

---

## 12. LeetCode 19：删除链表的倒数第 N 个节点

题目要求删除链表的倒数第 `n` 个节点，并返回新的头节点。

例如：

```text
原链表：1 -> 2 -> 3 -> 4 -> 5 -> nullptr
n = 2

删除节点：4

结果：1 -> 2 -> 3 -> 5 -> nullptr
```

删除一个节点时，需要找到它的前一个节点，然后修改前一个节点的 `next`。

---

## 13. 为什么使用虚拟头节点

首先创建一个栈上的虚拟头节点：

```cpp
ListNode dummyHead(0, head);
```

加入后链表变成：

```text
dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> nullptr
```

这样即使删除的是原来的头节点，也可以统一处理：

```cpp
previous->next = previous->next->next;
```

最后返回：

```cpp
return dummyHead.next;
```

虚拟头节点可以避免单独处理删除头节点的边界情况。

---

## 14. 解法一：先求链表长度

第一遍遍历得到链表长度：

```cpp
int getLength(const ListNode* head) {
    int length = 0;

    while (head != nullptr) {
        ++length;
        head = head->next;
    }

    return length;
}
```

从虚拟头节点出发，向前移动 `length - n` 步，就能到达待删除节点的前一个节点。

核心代码：

```cpp
ListNode* removeNthFromEndByLength(ListNode* head, int n) {
    ListNode dummyHead(0, head);
    ListNode* previous = &dummyHead;
    int length = getLength(head);

    for (int i = 0; i < length - n; ++i) {
        previous = previous->next;
    }

    ListNode* nodeToDelete = previous->next;
    previous->next = nodeToDelete->next;
    delete nodeToDelete;

    return dummyHead.next;
}
```

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

---

## 15. 解法二：快慢双指针

让两个指针都从虚拟头节点出发：

```cpp
ListNode* slow = &dummyHead;
ListNode* fast = &dummyHead;
```

先让 `fast` 向前移动 `n + 1` 步，然后让两个指针同时移动。
当 `fast` 到达 `nullptr` 时，`slow` 正好指向待删除节点的前一个节点。

```cpp
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummyHead(0, head);
    ListNode* slow = &dummyHead;
    ListNode* fast = &dummyHead;

    for (int i = 0; i <= n; ++i) {
        fast = fast->next;
    }

    while (fast != nullptr) {
        fast = fast->next;
        slow = slow->next;
    }

    ListNode* nodeToDelete = slow->next;
    slow->next = nodeToDelete->next;
    delete nodeToDelete;

    return dummyHead.next;
}
```

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

### 为什么 fast 要先走 n + 1 步

```text
fast 与 slow 相距 n 步
→ slow 最终停在待删除节点

fast 与 slow 相距 n + 1 步
→ slow 最终停在待删除节点的前一个节点
```

我们的目标是修改待删除节点前一个节点的 `next`，所以需要保持 `n + 1` 步距离。

---

## 16. 为什么要 delete 被删除的节点

如果节点通过 `new` 创建，只修改链表连接并不会释放它占用的内存。

因此应先保存节点地址，再断开并释放：

```cpp
ListNode* nodeToDelete = slow->next;
slow->next = nodeToDelete->next;
delete nodeToDelete;
```

LeetCode 测试框架有时会管理节点内存，但在本地完整程序中应该主动释放
不再使用的动态节点。

---

## 今日易错点

### 1. 看到“使用对象”就选择 shared_ptr

是否使用 `shared_ptr` 取决于是否共享生命周期，而不是是否访问对象。

### 2. 把所有非拥有关系都写成 weak_ptr

生命周期确定时可以使用 `T*`；只在函数中临时使用时可以使用 `T&`；
只有观察 `shared_ptr` 管理且可能提前销毁的对象时才使用 `weak_ptr`。

### 3. 使用 weak_ptr 观察 unique_ptr

`weak_ptr` 必须与 `shared_ptr` 的控制块配合，不能直接观察 `unique_ptr`。

### 4. 忘记 lock() 返回的是临时 shared_ptr

`weak_ptr` 本身不增加强引用计数，但 `lock()` 成功后得到的临时
`shared_ptr` 会暂时增加强引用计数。

### 5. 滥用 shared_ptr

如果只有一个所有者，应优先使用 `unique_ptr` 或直接把对象作为成员保存。

### 6. 忘记 unique_ptr 移动后原指针为空

```cpp
auto secondOwner = std::move(firstOwner);
```

移动以后，资源属于 `secondOwner`，使用 `firstOwner` 前需要先检查它是否为空。

### 7. 对象与指针的成员访问符写混

```cpp
dummyHead.next = head;  // 对象使用 .
slow->next = head;      // 指针使用 ->
```

### 8. 使用 new 创建虚拟头节点

推荐使用栈对象：

```cpp
ListNode dummyHead(0, head);
```

这样函数结束时会自动析构，不需要手动释放虚拟头节点。

### 9. fast 只提前走 n 步

如果希望 `slow` 停在待删除节点的前一个节点，`fast` 应该先走 `n + 1` 步。

### 10. 断开节点后忘记释放内存

本地程序中，通过 `new` 创建的节点在移出链表后还需要执行 `delete`。

---

## 今日代码

```text
day10/
├── day10.md
├── smart_pointer_demo.cpp
├── smart_pointer_practice.cpp
└── remove_nth_from_end.cpp
```

### smart_pointer_demo.cpp

用于观察 `unique_ptr` 的所有权转移、`shared_ptr` 的引用计数，以及
`weak_ptr::lock()` 和 `expired()` 的使用。

### smart_pointer_practice.cpp

通过 `Application`、`Logger`、`Server`、`Worker`、`Monitor` 和
`Connection` 的关系，练习独占所有权、共享所有权与非拥有观察。

### remove_nth_from_end.cpp

实现 LeetCode 19 的长度法和快慢双指针法，并练习虚拟头节点、指针移动、
删除节点和内存释放。

---

## 今日总结

今天把不同指针类型放进了同一套所有权判断方法中：

```text
T&                 临时借用
T*                 非拥有，生命周期有保证
unique_ptr<T>      独占所有权
shared_ptr<T>      共享所有权
weak_ptr<T>        非拥有地观察 shared_ptr 管理的对象
```

智能指针的选择不能只看“是否需要使用对象”，而要判断“是否拥有对象”和
“谁负责对象的生命周期”。

链表部分掌握了虚拟头节点、长度法、快慢双指针和删除节点后的内存释放。

下一阶段将开始学习左值、右值、右值引用、`std::move` 和移动语义，
进一步理解 `unique_ptr` 为什么不能复制，却能够转移所有权。
