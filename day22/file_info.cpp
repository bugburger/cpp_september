#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

int main() {
    const char* filename = "test.txt";

    // 1. access：检查文件是否存在
    if (access(filename, F_OK) != 0) {
        std::cerr << "file does not exist" << std::endl;
        return 1;
    }

    std::cout << "===== File Info =====" << std::endl;
    std::cout << "file: " << filename << std::endl;

    // 2. stat：获取文件属性
    struct stat fileInfo;

    if (stat(filename, &fileInfo) == -1) {
        std::cerr << "stat failed" << std::endl;
        return 1;
    }

    std::cout << "size: "
              << fileInfo.st_size
              << " bytes"
              << std::endl;

    // 判断文件类型
    if (S_ISREG(fileInfo.st_mode)) {
        std::cout << "type: regular file" << std::endl;
    }
    else if (S_ISDIR(fileInfo.st_mode)) {
        std::cout << "type: directory" << std::endl;
    }
    else {
        std::cout << "type: other" << std::endl;
    }

    // 输出完整权限
    std::cout << "permissions: ";

    std::cout << ((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXUSR) ? "x" : "-");

    std::cout << ((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXGRP) ? "x" : "-");

    std::cout << ((fileInfo.st_mode & S_IROTH) ? "r" : "-");
    std::cout << ((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
    std::cout << ((fileInfo.st_mode & S_IXOTH) ? "x" : "-");

    std::cout << std::endl;

    // 3. access：检查实际访问能力
    std::cout << "readable: "
              << (access(filename, R_OK) == 0 ? "yes" : "no")
              << std::endl;

    std::cout << "writable: "
              << (access(filename, W_OK) == 0 ? "yes" : "no")
              << std::endl;

    std::cout << "executable: "
              << (access(filename, X_OK) == 0 ? "yes" : "no")
              << std::endl;

    // 4. open + lseek：练习文件偏移量
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        std::cerr << "open failed" << std::endl;
        return 1;
    }

    off_t current = lseek(fd, 0, SEEK_CUR);
    std::cout << "current offset: "
              << current
              << std::endl;

    off_t end = lseek(fd, 0, SEEK_END);
    std::cout << "end offset: "
              << end
              << std::endl;

    lseek(fd, 0, SEEK_SET);

    std::cout << "reset offset: "
              << lseek(fd, 0, SEEK_CUR)
              << std::endl;

    close(fd);

    return 0;
}
