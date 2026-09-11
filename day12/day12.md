# Day12 学习总结

## 今日学习内容

今天主要学习了 C++ 中的 `std::move`、移动构造函数、移动赋值运算符，以及资源所有权转移。

同时结合前几天学习的 `unique_ptr`，进一步理解了为什么独占资源不能复制，但是可以移动。

LeetCode 完成了 242「有效的字母异位词」，开始接触哈希计数问题。

### 1. std::move

`std::move` 常见写法：

```cpp
Buffer b = std::move(a);
```

今天最重要的认识是：

> `std::move` 本身并不会真正移动资源。

它主要是把一个对象转换成可以匹配右值引用的形式，从而使移动构造或者移动赋值有机会被调用。

可以理解为：

```text
a 原来是左值
        ↓
std::move(a)
        ↓
可以匹配 T&&
        ↓
调用移动操作
```

真正完成资源转移的是：

```cpp
Buffer(Buffer&& other)
```

或者：

```cpp
Buffer& operator=(Buffer&& other)
```

---

## 2. 移动构造函数

移动构造函数的形式：

```cpp
Buffer(Buffer&& other) noexcept
```

例如：

```cpp
Buffer a(10);

Buffer b = std::move(a);
```

这里 `b` 是新创建的对象，因此调用移动构造函数。

实现：

```cpp
Buffer(Buffer&& other) noexcept
    : data_(other.data_),
      size_(other.size_)
{
    other.data_ = nullptr;
    other.size_ = 0;
}
```

移动构造并不会重新：

```cpp
new[]
```

一块相同大小的内存。

而是直接接管：

```cpp
other.data_
```

原来指向的资源。

因此相比深拷贝，可以避免复制大量数据。

---

## 3. 为什么移动后要把源指针置空

执行：

```cpp
data_ = other.data_;
```

之后，如果不执行：

```cpp
other.data_ = nullptr;
```

就会出现：

```text
当前对象 ----\
             -> 同一块内存
other -------/
```

两个对象都认为自己拥有这块资源。

等两个对象析构时，都可能执行：

```cpp
delete[] data_;
```

最终导致：

```text
double free
```

因此资源转移以后，需要让源对象主动放弃所有权：

```cpp
other.data_ = nullptr;
other.size_ = 0;
```

移动后的源对象仍然存在，只是不再拥有原来的资源。

---

## 4. 移动赋值运算符

移动赋值的形式：

```cpp
Buffer& operator=(Buffer&& other) noexcept
```

例如：

```cpp
Buffer a(10);
Buffer b(20);

b = std::move(a);
```

这里 `b` 在赋值之前就已经存在，因此调用的是移动赋值，而不是移动构造。

移动赋值需要先处理 `b` 自己原来的资源：

```cpp
delete[] data_;
```

然后再接管：

```cpp
data_ = other.data_;
size_ = other.size_;
```

最后：

```cpp
other.data_ = nullptr;
other.size_ = 0;
```

---

## 5. 为什么移动赋值要先释放旧资源

假设：

```text
a -> resource A
b -> resource B
```

现在执行：

```cpp
b = std::move(a);
```

如果直接：

```cpp
b.data_ = a.data_;
```

那么原来 `b` 保存的：

```text
resource B
```

地址就丢失了。

这块内存仍然存在，却无法再找到和释放，从而造成：

```text
内存泄漏
```

因此移动赋值必须遵循：

```text
先释放自己的旧资源
        ↓
接管 other 的资源
        ↓
让 other 放弃所有权
```

---

## 6. 移动构造与移动赋值

移动构造：

```cpp
Buffer b = std::move(a);
```

其中：

```text
b 是新对象
```

所以调用：

```cpp
Buffer(Buffer&& other)
```

移动赋值：

```cpp
Buffer b(20);

b = std::move(a);
```

其中：

```text
b 已经存在
```

所以调用：

```cpp
Buffer& operator=(Buffer&& other)
```

两者最大的区别是：

```text
移动构造：
目标对象刚创建，没有自己的旧资源

移动赋值：
目标对象已经存在，可能已经拥有资源
```

因此移动赋值通常需要先释放当前对象自己的资源。

---

## 7. unique_ptr 与移动语义

前面学习过：

```cpp
std::unique_ptr<int> p1 =
    std::make_unique<int>(10);
```

`unique_ptr` 不允许复制：

```cpp
// 错误
std::unique_ptr<int> p2 = p1;
```

因为复制以后可能出现两个对象同时拥有同一份资源。

但是可以：

```cpp
std::unique_ptr<int> p2 =
    std::move(p1);
```

移动以后：

```text
p1 -> nullptr
p2 -> 原来的资源
```

因此整个过程中仍然只有一个资源拥有者。

这让我进一步理解：

> `unique_ptr` 禁止复制的是“所有权”，但允许通过移动把所有权交给另一个对象。

---

## 8. 函数中的所有权转移

例如：

```cpp
void takeOwnership(std::unique_ptr<int> p)
{
    cout << *p << endl;
}
```

调用：

```cpp
std::unique_ptr<int> p =
    std::make_unique<int>(10);

takeOwnership(std::move(p));
```

表示：

```text
调用者主动放弃资源所有权
        ↓
函数参数 p 接管资源
```

调用以后，原来的：

```cpp
p
```

不再拥有该资源。

因此 `std::move` 在 `unique_ptr` 场景中经常表示：

> 我愿意把这个资源的所有权交出去。

---

## 9. LeetCode 242：有效的字母异位词

今天使用哈希计数解决了字符串字符统计问题。

自己的写法：

```cpp
unordered_map<char, int> mp;

for (int i = 0; i < s.size(); ++i) {
    mp[s[i]]++;
    mp[t[i]]--;
}
```

含义：

```text
s 中出现一个字符 -> +1
t 中出现一个字符 -> -1
```

如果两个字符串由完全相同的字符组成，那么最终所有字符对应的计数都会回到：

```text
0
```

然后检查：

```cpp
for (const auto& item : mp) {
    if (item.second != 0) {
        return false;
    }
}
```

全部为 `0` 就说明两个字符串是字母异位词。

---

## 10. 数组也可以作为简单哈希表

由于题目只包含：

```text
'a' ~ 'z'
```

共 26 个小写字母，因此还可以使用：

```cpp
int count[26] = {0};
```

通过：

```cpp
s[i] - 'a'
```

把字符映射到：

```text
0 ~ 25
```

例如：

```text
'a' - 'a' = 0
'b' - 'a' = 1
'z' - 'a' = 25
```

因此当元素取值范围比较小并且固定时，数组往往比 `unordered_map` 更直接。

---

## 今日易错点

1. `std::move` 本身不会真正移动资源。
2. 真正的资源转移由移动构造函数或移动赋值运算符完成。
3. 移动以后要让源对象放弃资源，否则可能发生重复释放。
4. 移动赋值前需要先释放目标对象自己的旧资源，否则可能发生内存泄漏。
5. `Buffer b = std::move(a)` 虽然有 `=`，但 `b` 是新对象，因此属于移动构造。
6. `b = std::move(a)` 中如果 `b` 已经存在，则属于移动赋值。
7. `unique_ptr` 不能复制所有权，但是可以通过移动转移所有权。
8. 字符范围固定且较小时，可以优先考虑数组进行计数。

---

## 今日总结

今天把前几天学习的 RAII、资源所有权和智能指针进一步与移动语义联系了起来。

现在能够理解：

```text
std::move
    ↓
提供移动条件
    ↓
移动构造 / 移动赋值
    ↓
真正完成资源所有权转移
```

同时也进一步理解了 `unique_ptr` 为什么：

```text
不能复制
但是可以移动
```

移动语义的核心并不是简单地学习一个 `std::move` 函数，而是理解：

```text
资源现在属于谁
原来的资源如何处理
源对象移动后处于什么状态
谁最终负责释放资源
```

LeetCode 242 则开始建立哈希计数思维，并理解了数组在固定取值范围下也可以充当简单高效的哈希表。
