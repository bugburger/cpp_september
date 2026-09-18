# Day20 学习总结

## 今日学习主题

今天主要完成了两条学习主线：

1. **Git 分支、HEAD、merge 与冲突解决**
2. **LeetCode 28：找出字符串中第一个匹配项的下标**
3. 在暴力字符串匹配的基础上进一步学习 **KMP 字符串匹配算法**

今天不仅学习了命令，还实际创建了独立 Git 仓库，从零完成了一次：

```text
创建仓库
→ commit
→ 创建 feature 分支
→ feature 独立开发
→ main 独立修改
→ 制造 merge conflict
→ 手动解决冲突
→ merge commit
→ 删除 feature 分支
```

算法部分则从自己写的暴力匹配代码出发，分析错误后继续学习了 KMP。

---

# 第一部分：Git 基础概念

## 1. commit 是什么

`commit` 可以理解为：

> 项目在某一个时间点的一次代码快照。

例如项目连续产生三个提交：

```text
A ---- B ---- C
```

A、B、C 都是不同的 commit。

每一个 commit 都有自己的哈希值，例如：

```text
7ef669e
1237966
5a24384
c02e0ba
```

今天练习仓库中实际产生过：

```text
7ef669e Initial commit
1237966 Add login feature
5a24384 Update app on main
c02e0ba Merge feature/login into main
```

---

## 2. git add 与 git commit

基本流程：

```bash
git add app.txt
git commit -m "Initial commit"
```

可以理解为：

```text
工作区
  ↓
git add
  ↓
暂存区
  ↓
git commit
  ↓
Git 历史
```

### git add

普通情况下：

```bash
git add file
```

表示：

> 把当前文件修改加入暂存区。

### git commit

```bash
git commit -m "message"
```

表示：

> 将暂存区中的内容正式保存为一个 commit。

---

# 第二部分：Git branch

## 1. branch 到底是什么

今天最重要的 Git 概念之一：

> branch 不是一整份代码副本。

branch 本质上是：

> **指向某一个 commit 的可移动指针。**

例如：

```text
A ---- B ---- C
             ↑
            main
```

此时 `main` 指向 commit C。

创建：

```bash
git switch -c feature/login
```

以后：

```text
A ---- B ---- C
             ↑
            main
            feature/login
```

此时两个 branch 都指向 C。

并没有复制一套完整项目。

---

## 2. 为什么需要 branch

实际开发时，不应该所有开发都直接在 `main` 上完成。

例如：

```text
main
feature/login
feature/payment
bugfix/null-pointer
```

可以：

```text
main
   ↓
稳定版本

feature/login
   ↓
登录功能开发

feature/payment
   ↓
支付功能开发
```

这样新功能开发不会直接破坏 main。

---

# 第三部分：HEAD

## 1. HEAD 是什么

HEAD 可以先理解成：

> 我当前正在操作哪个分支。

例如：

```text
7ef669e (HEAD -> main)
```

表示：

```text
当前所在分支 = main
```

如果执行：

```bash
git switch feature/login
```

可能看到：

```text
7ef669e (HEAD -> feature/login, main)
```

表示：

```text
main              → 7ef669e
feature/login     → 7ef669e
HEAD              → feature/login
```

---

## 2. 今天的重要纠错

### 错误理解

```text
HEAD = main
```

这是错误的。

### 正确理解

```text
HEAD = 当前所在的位置/分支
```

如果当前在 main：

```text
HEAD -> main
```

如果当前在 feature/login：

```text
HEAD -> feature/login
```

因此：

> HEAD 不是固定代表 main。

---

# 第四部分：git switch

## 1. 切换分支

例如：

```bash
git switch main
```

或者：

```bash
git switch feature/login
```

`git switch` 不只是“修改当前分支名字”。

它还会：

> 将工作区调整为目标 branch 所对应 commit 的文件状态。

---

## 2. 为什么切换分支后文件内容会变化

今天实际观察到了：

feature 分支中：

```text
Version: 1.1-feature
Message: Login feature added
```

切回 main 后可能又变成：

```text
Version: 1.0
Message: Hello
```

原因不是文件丢了。

而是：

```text
main            → 一个 commit
feature/login   → 另一个 commit
```

两个 commit 保存了不同版本的代码。

所以：

```text
切换 branch
=
切换 HEAD
+
工作区切换到相应版本
```

---

# 第五部分：分支独立开发

今天实际创建了：

```text
main
feature/login
```

最初：

```text
        7ef669e
        ↑     ↑
      main  feature/login
```

然后在 feature/login 上提交：

```text
7ef669e ---- 1237966
   ↑            ↑
 main      feature/login
```

说明：

```text
main 没有移动
feature/login 向前移动
```

这就是：

> 分支独立开发。

---

# 第六部分：分支分叉

随后切回 main：

```bash
git switch main
```

并在 main 上修改同一个文件，然后提交：

```text
* 5a24384 (HEAD -> main) Update app on main
| * 1237966 (feature/login) Add login feature
|/
* 7ef669e Initial commit
```

可以画成：

```text
              1237966
             ↑
      feature/login
            /
7ef669e
            \
             5a24384
                ↑
               main
               HEAD
```

此时两个分支从共同祖先：

```text
7ef669e
```

分别向两个方向发展。

这种情况可以理解为：

> branch divergence，分支发生分叉。

---

# 第七部分：git merge

## 1. merge 是什么

如果当前位于 main：

```bash
git switch main
```

然后执行：

```bash
git merge feature/login
```

含义是：

> 把 feature/login 的开发成果合并进当前 main。

注意：

```text
git merge X
```

不是“进入 X”。

而是：

> 把 X 合并到当前分支。

所以执行 merge 之前必须知道：

```text
HEAD 当前在哪里？
```

---

# 第八部分：为什么产生 merge conflict

今天执行：

```bash
git merge feature/login
```

出现：

```text
自动合并 app.txt
冲突（内容）：合并冲突于 app.txt
自动合并失败，修正冲突然后提交修正的结果。
```

原因：

main 修改了：

```text
Version: 1.1-main
Message: Main branch update
```

feature/login 修改了：

```text
Version: 1.1-feature
Message: Login feature added
```

双方修改了同一个文件的同一个区域。

Git 不知道最终应该：

```text
保留 main？
保留 feature？
还是两边组合？
```

所以 Git 停下来让开发者决定。

---

## 1. conflict 不等于 Git 出错

今天必须记住：

> Merge conflict 并不代表 Git 坏了。

它真正表示：

> Git 无法自动判断两份修改最终应该如何组合。

这是一个需要开发者作出决定的情况。

---

# 第九部分：冲突标记

发生 conflict 后，文件会出现：

```text
<<<<<<< HEAD
Version: 1.1-main
Message: Main branch update
=======
Version: 1.1-feature
Message: Login feature added
>>>>>>> feature/login
```

需要这样理解：

```text
<<<<<<< HEAD

当前分支的内容

=======

另一个准备合并进来的分支内容

>>>>>>> feature/login
```

---

## 1. HEAD 为什么代表 main

今天曾经不清楚：

```text
<<<<<<< HEAD
```

为什么上面是 main。

原因是：

执行 merge 前：

```text
HEAD -> main
```

所以：

```text
HEAD
=
当前分支 main
```

如果当时处于 feature/login：

```text
HEAD -> feature/login
```

那么 HEAD 就代表 feature/login。

因此不能死记：

```text
HEAD = main
```

正确理解应该是：

```text
HEAD = 当前所在分支
```

---

# 第十部分：解决 conflict

解决冲突本质上不是运行某条神奇命令。

真正的解决过程是：

> 开发者人工决定最终文件应该长什么样。

今天最终决定：

```text
Version: 1.1-merged
Message: Main update + Login feature
```

然后删除：

```text
<<<<<<<
=======
>>>>>>>
```

冲突标记。

---

# 第十一部分：冲突中的 git add

人工修改完成后：

```bash
git add app.txt
```

这里 `git add` 有两层含义：

### 普通含义

```text
将修改后的文件加入暂存区
```

### conflict 中的额外含义

```text
告诉 Git：

这个文件的冲突我已经解决完了。
最终版本就是暂存区中的这个版本。
```

今天自己的理解：

> “告诉 Git 已经确定把文件改成什么了。”

这个理解基本正确。

更准确说法：

> `git add` 将解决后的最终版本放入暂存区，同时把这个文件标记为 resolved。

---

# 第十二部分：merge commit

冲突解决后进行了 merge commit：

```text
c02e0ba Merge feature/login into main
```

最后的历史：

```text
*   c02e0ba (HEAD -> main) Merge feature/login into main
|\
| * 1237966 Add login feature
* | 5a24384 Update app on main
|/
* 7ef669e Initial commit
```

---

## 1. merge commit 和普通 commit 的区别

普通 commit 通常：

```text
A ---- B
```

B 只有一个 parent。

而 merge commit：

```text
      B
     / \
    /   \
   A     M
    \   /
     \ /
      C
```

M 一般有两个 parent。

今天的：

```text
c02e0ba
```

有：

```text
parent 1 → main 原来的提交
parent 2 → feature/login 的提交
```

因此：

> merge commit 将两条开发历史连接起来。

---

# 第十三部分：删除 feature 分支

合并完成后执行：

```bash
git branch -d feature/login
```

输出：

```text
已删除分支 feature/login（曾为 1237966）。
```

删除以后：

```text
* main
```

但历史仍然存在：

```text
*   c02e0ba (HEAD -> main)
|\
| * 1237966 Add login feature
* | 5a24384 Update app on main
|/
* 7ef669e Initial commit
```

说明：

> 删除 branch 不等于删除 commit。

因为：

```text
branch = 指针
```

删除的只是：

```text
feature/login → 1237966
```

这个指针。

提交已经被 main 的 merge 历史引用，所以仍然存在。

---

## 1. -d 和 -D

推荐：

```bash
git branch -d feature/login
```

`-d`：

```text
安全删除
```

Git 会检查 branch 是否已经合并。

而：

```bash
git branch -D feature/login
```

表示：

```text
强制删除
```

即使没有 merge 也可能删除。

因此日常优先：

```bash
git branch -d
```

---

# 第十四部分：git status

今天多次使用：

```bash
git status
```

它可以帮助判断：

```text
当前 branch
工作区是否有修改
是否有暂存内容
是否处于 merge
是否有 conflict
是否存在 untracked file
```

例如：

```text
位于分支 main
无文件要提交，干净的工作区
```

表示：

```text
当前 branch = main
工作区无修改
暂存区无待提交内容
```

---

# 第十五部分：git log --graph

今天非常重要的观察命令：

```bash
git log --oneline --decorate --graph --all
```

其中：

```text
--oneline
```

简化 commit 信息。

```text
--decorate
```

显示：

```text
HEAD
main
feature
```

等指针。

```text
--graph
```

显示分支图。

```text
--all
```

显示所有 branch 的历史。

今天看到：

```text
*   c02e0ba (HEAD -> main) Merge feature/login into main
|\
| * 1237966 Add login feature
* | 5a24384 Update app on main
|/
* 7ef669e Initial commit
```

这张图非常适合理解 Git 历史。

---

# 第十六部分：git reflog

今天第一次使用：

```bash
git reflog
```

它可以帮助排查：

> “我刚才到底执行了什么？”

例如之前实际看到：

```text
HEAD@{0}: commit (merge)
HEAD@{1}: commit
HEAD@{2}: checkout
HEAD@{3}: commit
```

当 Git 操作比较混乱时，可以通过：

```bash
git reflog
```

查看 HEAD 最近发生过的移动。

它对恢复误操作、检查 checkout / commit / merge 历史非常有用。

---

# 第十七部分：origin/main

今天还观察到：

```text
main
```

和：

```text
origin/main
```

不是同一个东西。

### main

```text
本地 main
```

### origin/main

可以理解成：

```text
本地所记录的远程仓库 main 的位置
```

例如：

```text
origin/main → A
main        → D
```

Git 可能提示：

```text
您的分支领先 origin/main 共若干个提交
```

表示：

> 本地有新的 commit，还没有 push 到远程。

---

# 第十八部分：Git 今日完整流程

今天亲手完成：

```text
git init
   ↓
Initial commit
   ↓
git switch -c feature/login
   ↓
feature/login 修改代码
   ↓
commit
   ↓
git switch main
   ↓
main 修改同一位置
   ↓
commit
   ↓
git merge feature/login
   ↓
CONFLICT
   ↓
人工修改文件
   ↓
git add
   ↓
git commit
   ↓
merge commit
   ↓
git branch -d feature/login
```

---

# 第十九部分：Git 今日必须记住的核心概念

```text
commit
=
一次项目版本快照
```

```text
branch
=
指向 commit 的可移动指针
```

```text
HEAD
=
当前所在分支/位置
```

```text
switch
=
切换 branch，并切换工作区版本
```

```text
merge
=
将另一个 branch 的开发成果合并到当前 branch
```

```text
conflict
=
Git 无法自动决定最终代码
```

```text
merge commit
=
连接两条开发历史，通常有两个 parent
```

---

# 第二十部分：LeetCode 28

题目：

> 找出字符串中第一个匹配项的下标。

例如：

```text
haystack = "sadbutsad"
needle   = "sad"
```

返回：

```text
0
```

因为：

```text
s a d b u t s a d
↑
0
```

---

# 第二十一部分：最开始的暴力匹配思路

核心：

```text
枚举 needle 在 haystack 中所有可能起点
       ↓
逐字符比较
       ↓
全部一样
       ↓
返回起点
```

标准思路：

```cpp
for (int i = 0; i <= m - n; ++i) {
    int j = 0;

    while (j < n && haystack[i + j] == needle[j]) {
        ++j;
    }

    if (j == n) {
        return i;
    }
}

return -1;
```

---

# 第二十二部分：暴力匹配中的 i 和 j

这是今天非常重要的下标关系：

```text
i
=
needle 在 haystack 中尝试匹配的起点
```

```text
j
=
needle 内部当前比较的位置
```

所以真正比较：

```cpp
haystack[i + j]
```

和：

```cpp
needle[j]
```

---

# 第二十三部分：自己最开始代码中的问题

最开始写过类似：

```cpp
int j = 0， flag = 0;

for (int i = 0; i < m; ) {
    if (haystack[i] == needle[j]) {
        while (j < n && haystack[i] == needle[j]) {
            ++i;
            ++j;
        }

        if (j == n) {
            return i-j;
            break;
        }

        flag = 0;
    }

    ++i;
}
```

这里有多个问题。

---

## 错误 1：使用中文逗号

写成：

```cpp
int j = 0， flag = 0;
```

这里是中文：

```text
，
```

C++ 需要：

```cpp
int j = 0, flag = 0;
```

否则会产生编译错误。

---

## 错误 2：flag 没有真正发挥作用

定义：

```cpp
flag
```

但后续并没有有效参与逻辑。

属于：

```text
冗余变量
```

应该删除。

---

## 错误 3：内部匹配直接修改 i

这是最重要的错误。

原本：

```text
i
```

应该表示：

> 当前匹配起点。

但是代码在内部：

```cpp
++i;
++j;
```

同时修改 i 和 j。

会导致匹配失败后：

> 下一个合法候选起点被跳过。

例如：

```text
haystack = "aaab"
needle   = "aab"
```

正确答案：

```text
1
```

如果从：

```text
i = 0
```

开始：

```text
a == a
a == a
a != b
```

失败时 i 已经被推进。

再执行外层：

```cpp
++i;
```

可能直接跳过真正应该尝试的：

```text
i = 1
```

所以：

> 暴力法中最好让 i 始终表示本轮匹配起点，不要在内部改变 i。

内部使用：

```cpp
haystack[i + j]
```

即可。

---

## 错误 4：匹配失败后 j 没有正确复位

如果：

```text
j
```

已经匹配了一部分 needle，

后来失败，

下一轮新的起点应该重新：

```cpp
j = 0;
```

否则会从 needle 中间继续比较，逻辑错误。

更好的方法是：

```cpp
for (...) {
    int j = 0;
```

每一次新起点都重新创建：

```text
j = 0
```

---

## 错误 5：return 后的 break 永远执行不到

原代码：

```cpp
return i - j;
break;
```

一旦：

```cpp
return
```

执行，整个函数已经结束。

所以：

```cpp
break;
```

永远不可能运行。

这种代码称为：

```text
unreachable code
```

应该直接删除。

---

## 错误 6：外层循环边界可以更精确

不是所有位置都可能成为 needle 的合法起点。

如果：

```text
haystack 长度 = m
needle 长度   = n
```

最后一个合法起点：

```text
m - n
```

所以：

```cpp
i <= m - n
```

比：

```cpp
i < m
```

更合理。

---

# 第二十四部分：暴力算法复杂度

最坏情况下：

```text
haystack 每一个可能位置
```

都需要比较很多 needle 字符。

所以复杂度：

```text
O(m × n)
```

其中：

```text
m = haystack 长度
n = needle 长度
```

因此继续学习更优的：

```text
KMP
```

---

# 第二十五部分：KMP 是什么

KMP 全称：

```text
Knuth-Morris-Pratt Algorithm
```

通常中文直接说：

```text
KMP 字符串匹配算法
```

主要用于：

> 在文本串 haystack 中高效寻找模式串 needle。

---

# 第二十六部分：KMP 为什么出现

暴力算法的问题：

```text
已经匹配很多字符
       ↓
某个位置失败
       ↓
大量已经知道的信息被丢掉
       ↓
重新比较
```

KMP 的核心思想：

> **已经比较过的信息不要浪费。**

---

# 第二十七部分：KMP 最核心的一句话

必须记住：

> **失配时，haystack 的 i 不回退，needle 的 j 根据 next 数组回退。**

即：

```text
i 继续向前
j 寻找更合理的位置
```

而不是暴力算法：

```text
i 回到前面重新开始比较
```

---

# 第二十八部分：什么是前缀和后缀

例如：

```text
abab
```

真前缀：

```text
a
ab
aba
```

真后缀：

```text
b
ab
bab
```

共同部分：

```text
ab
```

最长：

```text
ab
```

所以：

```text
最长相等真前后缀长度 = 2
```

---

# 第二十九部分：今天关于前后缀的易错点

曾经对于：

```text
abab
```

选择：

```text
abab
```

作为最长相等前后缀。

这是错误的。

因为 KMP 讨论的是：

```text
真前缀
真后缀
```

不能包含整个字符串自己。

否则任何字符串都可以说：

```text
最长相等前后缀 = 自己
```

就失去了意义。

因此：

```text
abab
```

正确答案：

```text
ab
```

长度：

```text
2
```

---

# 第三十部分：前后缀练习

### aba

前缀：

```text
a
ab
```

后缀：

```text
a
ba
```

最长共同：

```text
a
```

所以长度：

```text
1
```

---

### ababa

前缀：

```text
a
ab
aba
abab
```

后缀：

```text
a
ba
aba
baba
```

最长共同：

```text
aba
```

长度：

```text
3
```

---

### ababac

真前缀：

```text
a
ab
aba
abab
ababa
```

真后缀：

```text
c
ac
bac
abac
babac
```

没有共同项。

所以：

```text
0
```

今天曾经误认为：

```text
ababac → 3
```

错误原因：

把前一个子串：

```text
ababa
```

的 next 值直接延续到了：

```text
ababac
```

但添加新字符以后：

```text
最长相等前后缀可能突然变小甚至归零
```

所以 next 不能简单每次：

```text
+1
```

---

# 第三十一部分：next 数组

今天使用的 next 定义：

> `next[i]` 表示 `needle[0...i]` 的最长相等真前缀与真后缀长度。

例如：

```text
needle = ababac
```

最终：

```text
下标:   0 1 2 3 4 5
字符:   a b a b a c
next:   0 0 1 2 3 0
```

---

## next 数组每一项

```text
a
→ 0
```

```text
ab
→ 0
```

```text
aba
→ 1
```

因为：

```text
a == a
```

---

```text
abab
→ 2
```

因为：

```text
前缀 ab
后缀 ab
```

---

```text
ababa
→ 3
```

因为：

```text
前缀 aba
后缀 aba
```

---

```text
ababac
→ 0
```

因为没有相等的真前后缀。

---

# 第三十二部分：next 中存的不是下标

这是今天一个重要纠错。

曾经把：

```text
next[4] = 3
```

理解成：

> haystack 下一次匹配的起点下标。

这是错误的。

正确：

```text
next[4] = 3
```

表示：

> `needle[0...4]` 中，存在长度为 3 的最长相等前后缀。

例如：

```text
ababa
```

有：

```text
前缀 aba
后缀 aba
```

所以：

```text
next[4] = 3
```

这个 3 是：

```text
长度
```

不是 haystack 的下标。

---

# 第三十三部分：构造 next 的两个指针

模板：

```cpp
int j = 0;

for (int i = 1; i < n; ++i) {
    ...
}
```

### i

表示：

> 当前正在计算 `next[i]`。

### j

表示：

> 当前已经匹配成功的前缀长度。

同时：

```text
j
```

也恰好是：

> 下一次应该比较的 needle 下标。

例如：

```text
j = 3
```

说明：

```text
needle[0]
needle[1]
needle[2]
```

已经匹配。

下一步自然比较：

```text
needle[3]
```

---

# 第三十四部分：为什么从 i = 1 开始

因为：

```text
next[0] = 0
```

一个字符：

```text
a
```

没有真前缀和真后缀。

所以第一个位置天然：

```text
0
```

从：

```cpp
i = 1
```

开始计算即可。

---

# 第三十五部分：next 构造核心代码

```cpp
vector<int> next(n, 0);

int j = 0;

for (int i = 1; i < n; ++i) {

    while (j > 0 && needle[i] != needle[j]) {
        j = next[j - 1];
    }

    if (needle[i] == needle[j]) {
        ++j;
    }

    next[i] = j;
}
```

---

# 第三十六部分：为什么要有 while

最关键的例子：

```text
needle = ababac
```

计算最后一个：

```text
c
```

之前：

```text
j = 3
```

代表当前候选长度：

```text
3
```

即：

```text
aba
```

想继续匹配：

```text
needle[5] = c
needle[3] = b
```

失败：

```text
c != b
```

如果直接保留：

```text
j = 3
```

就会错误得到：

```text
next[5] = 3
```

显然错误。

---

# 第三十七部分：为什么不能直接 j = 0

长度 3 的候选失败：

```text
aba
```

不代表所有可能性都失败。

因为：

```text
aba
```

自身还有更短的相等前后缀：

```text
a
```

长度：

```text
1
```

所以应该：

```text
3
↓
1
↓
0
```

而不是直接：

```text
3
↓
0
```

也就是：

> 当前最长方案失败后，尝试次长方案。

如果还失败，再尝试更短方案。

---

# 第三十八部分：j = next[j - 1]

核心代码：

```cpp
j = next[j - 1];
```

可以翻译成人话：

> 当前最长可复用前后缀失败了，去寻找这个前缀内部还能利用的更短前后缀。

例如：

```text
j = 3
```

代表之前成功长度：

```text
3
```

其范围：

```text
needle[0...2]
```

所以查询：

```cpp
next[2]
```

得到：

```text
1
```

于是：

```text
j: 3 → 1
```

如果还是不匹配：

```text
j: 1 → next[0] → 0
```

---

# 第三十九部分：为什么是 next[j - 1]

如果：

```text
j = 5
```

意味着已经成功匹配：

```text
needle[0]
needle[1]
needle[2]
needle[3]
needle[4]
```

也就是说：

```text
最后一个成功位置 = j - 1
```

当前：

```text
needle[j]
```

正是失败的位置。

所以应该研究：

```text
已经成功匹配的部分
```

也就是：

```cpp
next[j - 1]
```

而不是：

```cpp
next[j]
```

---

# 第四十部分：为什么必须是 while，不是 if

可能出现：

```text
j = 3
```

第一次回退：

```text
3 → 1
```

但：

```text
needle[i] != needle[1]
```

仍然失败。

还需要：

```text
1 → 0
```

因此：

```cpp
if (...)
```

只能退一次，不够。

必须：

```cpp
while (...)
```

一直尝试：

```text
最长候选
↓
次长候选
↓
更短候选
↓
0
```

直到：

```text
匹配成功
```

或者：

```text
j = 0
```

---

# 第四十一部分：KMP 匹配部分

构造好 next 以后：

```cpp
for (int i = 0, j = 0; i < haystack.size(); ++i) {

    while (j > 0 && haystack[i] != needle[j]) {
        j = next[j - 1];
    }

    if (haystack[i] == needle[j]) {
        ++j;
    }

    if (j == needle.size()) {
        return i - j + 1;
    }
}
```

---

# 第四十二部分：KMP 匹配中的 i 和 j

### i

```text
haystack 当前正在比较的位置
```

### j

```text
needle 当前正在比较的位置
```

也是：

```text
当前已经成功匹配的 needle 长度
```

---

# 第四十三部分：KMP 为什么 i 不回退

今天专门问过：

> `j = next[j - 1]` 后，haystack 的 i 会不会变小？

答案：

```text
不会
```

原因：

> 只回退 needle 的 j。

KMP 已经利用 next 数组保存了之前的匹配信息。

所以没有必要重新读取：

```text
haystack 之前已经比较过的字符
```

这是 KMP 高效的重要原因。

---

# 第四十四部分：实际匹配示例

例如：

```text
haystack = abababac
needle   = ababac
```

最开始：

```text
haystack: a b a b a b a c
needle:   a b a b a c
```

成功匹配：

```text
ababa
```

之后：

```text
haystack[5] = b
needle[5]   = c
```

失败。

此时：

```text
j = 5
```

查询：

```text
next[j - 1]
=
next[4]
=
3
```

所以：

```text
j = 3
```

注意：

```text
i 仍然停在 haystack[5]
```

然后重新比较：

```text
haystack[5] = b
needle[3]   = b
```

匹配成功。

继续：

```text
a == a
c == c
```

最终找到 needle。

---

# 第四十五部分：为什么 next[4] = 3 可以复用

因为已经成功匹配：

```text
ababa
```

其中：

```text
前缀 aba
后缀 aba
```

所以最后这三个字符：

```text
aba
```

已经知道和 needle 开头：

```text
aba
```

相同。

不需要重新比较。

这就是：

> KMP 利用已经匹配的信息。

---

# 第四十六部分：为什么返回 i - j + 1

完整匹配成功时：

```text
i
```

停在：

```text
匹配结果的最后一个字符
```

而：

```text
j = needle 长度
```

例如：

```text
haystack = hello
needle   = ll
```

完整匹配时：

```text
下标:
h e l l o
0 1 2 3 4
      ↑
      i = 3
```

needle 长度：

```text
j = 2
```

起点：

```text
3 - 2 + 1
=
2
```

因此：

```cpp
return i - j + 1;
```

---

# 第四十七部分：KMP 通用模板

当前学习的 next 定义下，可以把下面作为固定模板。

## 构造 next

```cpp
vector<int> next(n, 0);

for (int i = 1, j = 0; i < n; ++i) {

    while (j > 0 && needle[i] != needle[j]) {
        j = next[j - 1];
    }

    if (needle[i] == needle[j]) {
        ++j;
    }

    next[i] = j;
}
```

## 正式匹配

```cpp
for (int i = 0, j = 0; i < haystack.size(); ++i) {

    while (j > 0 && haystack[i] != needle[j]) {
        j = next[j - 1];
    }

    if (haystack[i] == needle[j]) {
        ++j;
    }

    if (j == needle.size()) {
        return i - j + 1;
    }
}
```

---

# 第四十八部分：KMP 模板最核心的结构

可以压缩成：

```cpp
while (j > 0 && 当前字符不匹配) {
    j = next[j - 1];
}

if (当前字符匹配) {
    ++j;
}
```

翻译：

```text
不匹配
↓
当前最长方案失败
↓
尝试更短方案
↓
如果仍失败继续回退

匹配
↓
已经匹配长度 +1
```

---

# 第四十九部分：KMP 时间复杂度

假设：

```text
m = haystack.size()
n = needle.size()
```

构建 next：

```text
O(n)
```

正式匹配：

```text
O(m)
```

总时间复杂度：

```text
O(m + n)
```

相比暴力：

```text
O(m × n)
```

在长字符串、重复结构较多时更加高效。

---

# 第五十部分：当前阶段 KMP 需要掌握到什么程度

目前不需要学习所有 next 版本。

不同资料可能看到：

```text
next[0] = -1
```

或者其他 next 定义。

现在不要混学。

当前只固定掌握：

```text
next[i]
=
needle[0...i] 最长相等真前后缀长度
```

对应：

```text
next[0] = 0
```

这一版即可。

目前需要达到：

```text
1. 能解释 KMP 为什么比暴力快
2. 能解释前缀、后缀
3. 能手算简单 next 数组
4. 能解释 j = next[j - 1]
5. 知道为什么用 while
6. 知道为什么 i 不回退
7. 能根据模板写出 KMP
```

---

# 第五十一部分：今天所有重点易错点汇总

## Git 易错点

### 1. branch 不是代码副本

错误：

```text
一个 branch = 一份完整项目
```

正确：

```text
branch = 指向 commit 的指针
```

---

### 2. HEAD 不等于 main

错误：

```text
HEAD 永远就是 main
```

正确：

```text
HEAD = 当前所在 branch
```

---

### 3. conflict 不代表 Git 坏了

正确理解：

```text
Git 无法自动确定最终代码
```

---

### 4. git add 在冲突中有额外作用

```text
加入暂存区
+
告诉 Git 此文件冲突已经解决
```

---

### 5. 删除 branch 不等于删除 commit

```text
git branch -d feature/login
```

删除的是：

```text
branch 指针
```

不是已经合并的提交。

---

### 6. merge 是把别人合到“当前分支”

执行：

```bash
git merge feature/login
```

一定先问：

```text
HEAD 当前在哪里？
```

---

## 字符串匹配易错点

### 1. 不要在暴力匹配内部随意修改起点 i

正确：

```text
i 固定表示当前起点
j 在内部移动
```

比较：

```cpp
haystack[i + j]
```

---

### 2. 新起点必须重新 j = 0

否则 needle 会从中间位置继续比较。

---

### 3. return 后不能再写 break

```cpp
return value;
break;
```

其中 break 永远执行不到。

---

### 4. 中文标点不能出现在 C++ 语法中

错误：

```cpp
int j = 0， flag = 0;
```

正确：

```cpp
int j = 0, flag = 0;
```

---

## KMP 易错点

### 1. 真前后缀不能是字符串自身

```text
abab
```

不能选：

```text
abab
```

正确最长：

```text
ab
```

---

### 2. next 存的是长度，不是 haystack 下标

例如：

```text
next[4] = 3
```

表示：

```text
最长相等前后缀长度 = 3
```

---

### 3. next 不一定一直增长

例如：

```text
ababa
→ 3
```

但：

```text
ababac
→ 0
```

---

### 4. 失配不能直接 j = 0

应该：

```cpp
j = next[j - 1];
```

尝试次长候选。

---

### 5. 需要 while，不是 if

因为可能：

```text
3 → 1 → 0
```

连续回退多次。

---

### 6. 回退的是 j，不是 i

KMP：

```text
haystack 的 i 不回退
needle 的 j 回退
```

---

### 7. next[j - 1] 中为什么是 j - 1

因为：

```text
j
=
下一次准备匹配的位置
```

真正已经成功匹配的最后位置：

```text
j - 1
```

---

# 第五十二部分：今日面试/口述题

## Git

### Q1：Git branch 的本质是什么？

答：

> branch 本质上是一个指向 commit 的可移动指针，而不是一份完整的代码副本。

### Q2：HEAD 是什么？

答：

> HEAD 表示当前所在的位置，通常指向当前 branch。

### Q3：什么情况下会产生 merge conflict？

答：

> 两条开发历史对相同区域进行了 Git 无法自动协调的修改时可能产生 conflict。

### Q4：解决 conflict 的本质是什么？

答：

> 开发者人工确定最终代码，并通过 git add 将解决后的版本标记为 resolved。

### Q5：merge commit 有什么特点？

答：

> merge commit 通常有两个父提交，用来连接两条开发历史。

---

## KMP

### Q1：KMP 解决了什么问题？

答：

> 暴力字符串匹配在失配后会重复比较很多已经比较过的字符。KMP 利用模式串自身的前后缀结构避免重复比较。

### Q2：next[i] 表示什么？

答：

> `needle[0...i]` 的最长相等真前缀和真后缀长度。

### Q3：为什么失配时 j = next[j - 1]？

答：

> 当前最长前后缀方案失败后，利用已经匹配部分自身的 next 信息找到次长的可利用前后缀，而不是全部重新开始。

### Q4：为什么必须用 while？

答：

> 因为一次回退以后仍可能失配，需要继续尝试更短候选。

### Q5：KMP 为什么 i 不回退？

答：

> 已经比较过的 haystack 信息可以通过 next 数组和模式串前后缀关系继续利用，因此没有必要重新比较。

---

# 第五十三部分：今日常用命令整理

```bash
git init
```

初始化 Git 仓库。

```bash
git status
```

查看当前 Git 状态。

```bash
git branch
```

查看 branch。

```bash
git branch --show-current
```

查看当前 branch。

```bash
git switch -c feature/login
```

创建并切换 branch。

```bash
git switch main
```

切换回 main。

```bash
git add file
```

加入暂存区。

```bash
git commit -m "message"
```

创建 commit。

```bash
git merge feature/login
```

将 feature/login 合并到当前 branch。

```bash
git branch -d feature/login
```

安全删除已经合并的 branch。

```bash
git log --oneline --decorate --graph --all
```

查看完整分支图。

```bash
git reflog
```

查看 HEAD 最近的操作历史。

---

# 第五十四部分：Day20 最终知识地图

```text
Day20
│
├── Git
│   ├── commit
│   ├── branch
│   ├── HEAD
│   ├── switch
│   ├── merge
│   ├── conflict
│   ├── conflict markers
│   ├── git add 的 resolved 含义
│   ├── merge commit
│   ├── 两个 parent
│   ├── branch -d
│   ├── git log --graph
│   ├── git reflog
│   └── origin/main
│
└── 字符串匹配
    ├── 暴力匹配
    │   ├── i = 起点
    │   ├── j = needle 偏移
    │   ├── haystack[i+j]
    │   └── O(m*n)
    │
    └── KMP
        ├── 前缀
        ├── 后缀
        ├── 真前缀/真后缀
        ├── next 数组
        ├── next[i] 是长度
        ├── j 的含义
        ├── j = next[j-1]
        ├── while 连续回退
        ├── i 不回退
        ├── return i-j+1
        └── O(m+n)
```

---

# 今日最终总结

今天 Git 部分从“会 git add / commit / push”进一步进入了真正的多人开发基础：

```text
branch
→ 独立开发
→ merge
→ conflict
→ resolve
→ merge commit
```

最重要的是理解：

```text
branch 是指针
HEAD 表示当前所在分支
conflict 是 Git 无法替开发者决定最终代码
```

算法部分首先发现了自己暴力字符串匹配中的多个指针控制问题，包括：

```text
i 在内部被修改
j 没有正确复位
中文逗号
无用 flag
return 后继续 break
```

随后学习 KMP。

KMP 今天最核心的理解不是死背代码，而是：

> **已经匹配过的信息不要浪费。**

失配时：

```text
haystack 的 i 不回退
needle 的 j 根据 next 数组回退
```

而：

```cpp
j = next[j - 1];
```

可以理解成：

> 当前最长方案失败后，继续寻找次长的可复用前后缀。

今天需要长期记住的 KMP 核心模板：

```cpp
while (j > 0 && 当前字符不匹配) {
    j = next[j - 1];
}

if (当前字符匹配) {
    ++j;
}
```

这是 Day20 最重要的算法知识点之一。

