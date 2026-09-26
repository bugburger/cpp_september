#include <sys/stat.h>
#include <iostream>

int main() {

    struct stat fileInfo;

    int result = stat("test.txt", &fileInfo);

    if (result == -1) {
        std::cerr << "stat failed" << std::endl;
        return 1;
    }

    std::cout << "file size: "
              << fileInfo.st_size
              << " bytes"
              << std::endl;

    if (S_ISREG(fileInfo.st_mode)) {
	    std::cout << "type: regular file" << std::endl;
    }
    else if (S_ISDIR(fileInfo.st_mode)) {
	    std::cout << "type: diretory" << std::endl;
    }
    else {
	    std::cout << "type: other" << std::endl;
    }
if (fileInfo.st_mode & S_IRUSR) {
    std::cout << "owner can read" << std::endl;
}

if (fileInfo.st_mode & S_IWUSR) {
    std::cout << "owner can write" << std::endl;
}

if (fileInfo.st_mode & S_IXUSR) {
    std::cout << "owner can execute" << std::endl;
}
    return 0;
}
