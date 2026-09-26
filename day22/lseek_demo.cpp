#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
    int fd = open("test.txt", O_RDONLY);

    if (fd == -1) {
        std::cerr << "open failed" << std::endl;
        return 1;
    }

    char buffer[6] = {};

    // 第一次读取 5 个字节
    ssize_t n = read(fd, buffer, 5);

    std::cout << "first read: " << buffer << std::endl;

    // TODO：使用 lseek 回到文件开头
    lseek(fd, 0, SEEK_SET);

    // 清空 buffer
    for (int i = 0; i < 6; ++i) {
        buffer[i] = '\0';
    }

    // 第二次读取 5 个字节
    n = read(fd, buffer, 5);

    std::cout << "second read: " << buffer << std::endl;

    close(fd);

    return 0;
}
