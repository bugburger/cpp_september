#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "start pid = "
              << getpid()
              << std::endl;

    fork();

    std::cout << "after first fork, pid = "
              << getpid()
              << ", ppid = "
              << getppid()
              << std::endl;

    fork();

    std::cout << "after second fork, pid = "
              << getpid()
              << ", ppid = "
              << getppid()
              << std::endl;

    return 0;
}
