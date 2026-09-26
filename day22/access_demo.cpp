#include <unistd.h>
#include <iostream>

int main() {
    const char* filename = "test.txt";

    if (access(filename, F_OK) == 0) {
        std::cout << "file exists" << std::endl;
    } else {
        std::cout << "file does not exist" << std::endl;
    }

    if (access(filename, R_OK) == 0) {
        std::cout << "readable: yes" << std::endl;
    } else {
        std::cout << "readable: no" << std::endl;
    }

    if (access(filename, W_OK) == 0) {
        std::cout << "writable: yes" << std::endl;
    } else {
        std::cout << "writable: no" << std::endl;
    }

    if (access(filename, X_OK) == 0) {
        std::cout << "executable: yes" << std::endl;
    } else {
        std::cout << "executable: no" << std::endl;
    }

    return 0;
}
