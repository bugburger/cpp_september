# Day08 学习总结

## 今日学习内容

今天主要学习了 C++ 中的 `std::shared_ptr`、共享所有权和引用计数机制，并通过代码观察了 `shared_ptr` 在创建、复制、离开作用域和调用 `reset()` 时的变化。

算法部分完成了 LeetCode 206「反转链表」，重点练习了单链表中的指针保存、指向修改和指针移动，理解了为什么反转链表时必须先保存下一个节点。

今天还进一步区分了：

```text
unique_ptr 与 shared_ptr
ListNode 与 ListNode*
对象和指针
. 和 ->
原地反转和重新创建链表
```

这些内容把前面学习的指针、动态内存、RAII、智能指针和单链表进一步联系了起来。

---

### 1. shared_ptr 与共享所有权

昨天学习的 `std::unique_ptr` 表示独占所有权，一份资源只能由一个 `unique_ptr` 管理：

```cpp
std::unique_ptr<int> p1 =
    std::make_unique<int>(10);
```

因此，`unique_ptr` 不能普通复制：

```cpp
// 错误
std::unique_ptr<int> p2 = p1;
```

而今天学习的 `std::shared_ptr` 表示共享所有权，允许多个智能指针共同管理同一份资源：

```cpp
std::shared_ptr<int> p1 =
    std::make_shared<int>(10);

std::shared_ptr<int> p2 = p1;
```

此时：

```text
p1 ─────┐
        ├──> [10]
p2 ─────┘
```

`p1` 和 `p2` 并不是分别管理两个值为 `10` 的对象，而是共同指向同一个对象。

如果通过 `p2` 修改对象：

```cpp
*p2 = 100;
```

再通过 `p1` 访问：

```cpp
std::cout << *p1 << '\n';
```

输出也会是：

```text
100
```

这说明复制 `shared_ptr` 时，复制的是对同一份资源的共享所有权，而不是重新创建底层资源。

---

### 2. 使用 make_shared 创建资源

创建 `shared_ptr` 时，推荐使用：

```cpp
std::make_shared<T>()
```

例如：

```cpp
std::shared_ptr<int> p =
    std::make_shared<int>(10);
```

也可以使用 `auto` 简化类型：

```cpp
auto p = std::make_shared<int>(10);
```

如果管理的是自定义对象：

```cpp
class Resource {
private:
    int id_;

public:
    explicit Resource(int id)
        : id_(id) {
        std::cout << "Resource "
                  << id_
                  << " created\n";
    }

    ~Resource() {
        std::cout << "Resource "
                  << id_
                  << " destroyed\n";
    }
};
```

可以这样创建：

```cpp
auto p = std::make_shared<Resource>(1);
```

资源由 `shared_ptr` 自动管理，不需要手动执行 `delete`。

当最后一个拥有资源的 `shared_ptr` 消失时，资源会自动析构和释放。

这仍然体现了 RAII：

```text
shared_ptr 对象管理资源
        ↓
shared_ptr 生命周期结束
        ↓
自动减少引用计数
        ↓
最后一个所有者消失
        ↓
资源自动释放
```

---

### 3. shared_ptr 的引用计数

`shared_ptr` 会通过引用计数记录当前有多少个智能指针共同拥有同一份资源。

创建第一个 `shared_ptr`：

```cpp
auto p1 = std::make_shared<int>(10);
```

此时：

```text
use_count = 1
```

复制得到 `p2`：

```cpp
auto p2 = p1;
```

引用计数变成：

```text
use_count = 2
```

继续复制得到 `p3`：

```cpp
auto p3 = p2;
```

引用计数变成：

```text
use_count = 3
```

可以使用：

```cpp
p1.use_count()
```

查看引用计数：

```cpp
std::cout << p1.use_count() << '\n';
```

引用计数增加的典型情况是：

```cpp
auto p2 = p1;
```

因为增加了一个新的共同所有者。

引用计数减少的情况包括：

```cpp
p2.reset();
```

以及 `p2` 离开作用域并自动析构。

---

### 4. 引用计数的生命周期变化

今天通过下面的过程观察了引用计数变化：

```cpp
std::shared_ptr<Resource> p1 =
    std::make_shared<Resource>(1);

std::shared_ptr<Resource> p2 = p1;

{
    std::shared_ptr<Resource> p3 = p2;

    std::cout << p1.use_count()
              << '\n';
}

std::cout << p1.use_count()
          << '\n';

p2.reset();
p1.reset();
```

整个变化过程是：

```text
创建 p1
count = 1

复制得到 p2
count = 2

复制得到 p3
count = 3

p3 离开局部作用域
count = 2

p2.reset()
count = 1

p1.reset()
count = 0

资源真正析构并释放
```

因此，`shared_ptr` 最重要的资源释放规则是：

> 只有当引用计数变成 0 时，被共同管理的资源才会真正释放。

---

### 5. reset() 的作用

今天进一步理解了：

```cpp
p.reset();
```

它并不一定表示“立刻删除资源”，而是表示：

> 当前这个 `shared_ptr` 放弃对原资源的所有权，并变为空指针。

例如：

```cpp
auto p1 = std::make_shared<int>(10);
auto p2 = p1;
auto p3 = p1;
```

此时：

```text
p1 ─┐
p2 ─┼──> [10]
p3 ─┘

count = 3
```

执行：

```cpp
p2.reset();
```

以后：

```text
p1 ─┐
    ├──> [10]
p3 ─┘

p2 -> nullptr
count = 2
```

因为 `p1` 和 `p3` 仍然拥有资源，所以底层资源不会释放。

可以判断一个 `shared_ptr` 是否为空：

```cpp
if (p2 == nullptr) {
    std::cout << "p2 is empty\n";
}
```

---

### 6. shared_ptr 作为函数参数

如果函数参数使用值传递：

```cpp
void test(std::shared_ptr<int> p) {
    std::cout << p.use_count()
              << '\n';
}
```

调用：

```cpp
auto p1 = std::make_shared<int>(10);

test(p1);
```

进入函数时，会复制出一个新的 `shared_ptr` 参数：

```text
调用函数前
count = 1

进入函数后
count = 2

函数结束，参数 p 析构
count = 1
```

这是因为按值传递相当于增加了一个临时所有者。

如果函数只是使用资源，不需要获得新的共享所有权，可以使用常量引用：

```cpp
void test(
    const std::shared_ptr<int>& p) {

    std::cout << *p << '\n';
}
```

此时不会因为函数参数本身复制 `shared_ptr`，因此引用计数不会额外增加。

---

### 7. unique_ptr 与 shared_ptr 的区别

`unique_ptr`：

```cpp
auto p = std::make_unique<int>(10);
```

特点：

- 一个资源只有一个所有者；
- 不能普通复制；
- 可以通过 `std::move` 转移所有权；
- 所有权关系简单明确。

`shared_ptr`：

```cpp
auto p = std::make_shared<int>(10);
```

特点：

- 一个资源可以有多个所有者；
- 可以复制；
- 使用引用计数记录所有者数量；
- 最后一个所有者消失后自动释放资源。

可以简单理解为：

```text
unique_ptr
    ↓
“这份资源只由我负责”

shared_ptr
    ↓
“这份资源由我们共同负责”
```

`shared_ptr` 并不一定比 `unique_ptr` 更好。

如果资源只有一个明确的所有者，应优先考虑 `unique_ptr`；确实需要多个对象共同拥有资源时，再使用 `shared_ptr`。

---

### 8. LeetCode 206：反转链表

今天完成了 LeetCode 206「反转链表」。

原链表：

```text
1 -> 2 -> 3 -> 4 -> 5 -> nullptr
```

要求变成：

```text
5 -> 4 -> 3 -> 2 -> 1 -> nullptr
```

今天使用迭代方式进行原地反转，不创建新的链表节点，而是直接修改原有节点的 `next` 指向。

核心代码：

```cpp
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* preNode = nullptr;
        ListNode* cur = head;

        while (cur != nullptr) {
            ListNode* nextNode = cur->next;

            cur->next = preNode;

            preNode = cur;
            cur = nextNode;
        }

        return preNode;
    }
};
```

---

### 9. 反转链表中的三个指针

反转链表主要使用三个指针：

```text
preNode
    ↓
指向已经反转完成的部分

cur
    ↓
指向当前正在处理的节点

nextNode
    ↓
临时保存剩余链表的入口
```

初始状态：

```text
preNode      cur
   ↓          ↓
nullptr      1 -> 2 -> 3 -> nullptr
```

先保存下一个节点：

```cpp
ListNode* nextNode = cur->next;
```

然后反转当前节点的指向：

```cpp
cur->next = preNode;
```

接着移动两个工作指针：

```cpp
preNode = cur;
cur = nextNode;
```

整个过程可以记为：

```text
保存后路
    ↓
反转当前指针
    ↓
preNode 前进
    ↓
cur 前进
```

---

### 10. 为什么必须先保存 nextNode

假设当前链表为：

```text
1 -> 2 -> 3 -> nullptr
↑
cur
```

如果直接执行：

```cpp
cur->next = preNode;
```

节点 `1` 原来指向节点 `2` 的链接就会被覆盖。

如果之前没有保存节点 `2` 的地址，就可能丢失后半段链表。

因此正确顺序必须是：

```cpp
ListNode* nextNode = cur->next;
cur->next = preNode;
preNode = cur;
cur = nextNode;
```

这和昨天析构链表时的操作非常相似：

```cpp
ListNode* nextNode = cur->next;
delete cur;
cur = nextNode;
```

两者都体现了同一个原则：

> 在破坏当前节点或当前链接之前，先保存后面仍然需要访问的位置。

---

### 11. 为什么最后返回 preNode

循环条件是：

```cpp
while (cur != nullptr)
```

循环结束后：

```text
cur == nullptr
```

而 `preNode` 已经指向原链表最后一个节点：

```text
nullptr <- 1 <- 2 <- 3 <- 4 <- 5
                              ↑
                           preNode
```

原来的尾节点 `5` 已经变成反转后的新头节点，所以应该：

```cpp
return preNode;
```

不能返回：

```cpp
return cur;
```

因为此时 `cur` 已经是 `nullptr`。

---

### 12. 原地反转与新建链表

还可以通过不断创建新节点得到反向链表：

```cpp
ListNode* ans = nullptr;

for (ListNode* x = head;
     x != nullptr;
     x = x->next) {
    ans = new ListNode(x->val, ans);
}
```

这种方法每遍历一个节点都要重新申请一个节点：

```text
时间复杂度：O(n)
额外空间复杂度：O(n)
```

原地反转直接修改已有节点的指向，不需要重新创建节点：

```text
时间复杂度：O(n)
额外空间复杂度：O(1)
```

因此 LeetCode 206 更推荐使用原地反转。

---

### 13. ListNode、ListNode*、`.` 和 `->`

`ListNode` 表示一个真正的节点对象：

```cpp
ListNode node;
```

对象访问成员时使用 `.`：

```cpp
node.val;
node.next;
```

`ListNode*` 表示指向节点的指针：

```cpp
ListNode* cur = head;
```

指针访问成员时使用 `->`：

```cpp
cur->val;
cur->next;
```

其中：

```cpp
cur->next
```

等价于：

```cpp
(*cur).next
```

因此可以记为：

```text
对象 ListNode
    ↓
使用 .

引用 ListNode&
    ↓
使用 .

指针 ListNode*
    ↓
使用 ->
```

LeetCode 206 返回的是新头节点的地址，因此返回类型必须是：

```cpp
ListNode*
```

---

## 今日易错点

1. `shared_ptr` 复制后管理的是同一份资源，并不是重新创建一份资源。

2. 每新增一个共同所有者，引用计数会增加。

3. `shared_ptr` 离开作用域或者调用 `reset()` 后，引用计数会减少。

4. `reset()` 只是让当前 `shared_ptr` 放弃所有权，不代表资源一定会立即释放。

5. 只有最后一个所有者消失，使引用计数变成 `0` 时，资源才真正释放。

6. `shared_ptr` 按值传入函数会发生复制，因此引用计数可能暂时增加。

7. 如果函数只需要使用资源，可以传递 `const std::shared_ptr<T>&`，避免额外复制。

8. `shared_ptr` 并不总是优于 `unique_ptr`，明确为独占所有权时应优先使用 `unique_ptr`。

9. 反转链表时必须先保存：

   ```cpp
   ListNode* nextNode = cur->next;
   ```

   否则修改 `cur->next` 后可能丢失剩余链表。

10. 反转链表的四个步骤不能随意交换：

    ```text
    保存 next
        ↓
    反转 next
        ↓
    移动 preNode
        ↓
    移动 cur
    ```

11. 循环结束后 `cur == nullptr`，真正的新头节点是 `preNode`。

12. 原地反转没有重新 `new` 节点，额外空间复杂度为 `O(1)`。

13. C++ 空指针应该写 `nullptr`，不能写 Java 风格的 `null`。

14. `ListNode* cur` 是指针，访问成员应该使用 `cur->next`，不能写 `cur.next`。

15. `ListNode node` 是对象，访问成员使用 `node.next`。

16. `ListNode` 和 `ListNode*` 是不同类型，前者是节点对象，后者是节点地址。

17. LeetCode 206 返回新头节点的地址，因此返回类型不能漏掉 `*`。

18. 使用 `new` 创建新链表时，还要负责释放新节点，否则可能造成内存泄漏。

---

## 今日总结

今天在昨天 `unique_ptr` 独占所有权的基础上，继续学习了 `shared_ptr` 的共享所有权。

`shared_ptr` 的生命周期可以总结为：

```text
创建第一个 shared_ptr
        ↓
引用计数为 1

复制 shared_ptr
        ↓
引用计数增加

shared_ptr reset 或析构
        ↓
引用计数减少

引用计数变成 0
        ↓
资源自动析构并释放
```

今天开始从“谁拥有资源”和“资源什么时候释放”的角度理解智能指针，而不只是把智能指针当成裸指针的替代写法。

同时，通过 LeetCode 206 进一步练习了链表中的指针操作。

反转链表的核心思想是：

```text
先保存后路
再修改当前链接
最后移动两个工作指针
```

目前已经能够理解三个指针的作用：

```text
preNode
    ↓
已经反转完成的链表

cur
    ↓
当前正在处理的节点

nextNode
    ↓
提前保存的剩余链表入口
```

到目前为止，智能指针部分已经完成：

```text
Day07：unique_ptr
       独占所有权

Day08：shared_ptr
       共享所有权与引用计数
```

链表部分已经完成：

```text
LeetCode 203：移除链表元素
LeetCode 707：设计链表
LeetCode 206：反转链表
```

对于链表的节点结构、创建、插入、删除、遍历、内存释放以及指针反转，已经形成了比较完整的第一轮认识。
