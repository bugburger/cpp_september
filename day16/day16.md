# Day16 学习总结

## 今日学习内容

今天主要学习了 C++ 多目录工程的组织方式，并在 Day15 单目录 CMake 的基础上进一步学习：

- `include/`、`src/`、`examples/`、`build/` 的目录职责
- 多目录工程中的头文件搜索路径
- `-Iinclude` 的作用
- `add_library()` 创建静态库
- Linux 静态库 `.a`
- `ar` 查看静态库内容
- `nm -C` 查看库中的符号
- `target_include_directories()`
- `PUBLIC` 与 `PRIVATE`
- `target_link_libraries()`
- CMake target 之间的依赖关系

算法部分完成了 LeetCode 15「三数之和」，学习了：

- 排序
- 双指针
- 固定一个数，将三数问题转换成两数问题
- `i`、`left`、`right` 三层去重
- 双指针移动方向
- `std::size_t` 与 `int` 的 signed / unsigned 警告

---

# 一、多目录 C++ 工程

Day15 中的工程结构比较简单：

```text
buffer_cmake/
├── CMakeLists.txt
├── buffer.hpp
├── buffer.cpp
├── main.cpp
└── build/
