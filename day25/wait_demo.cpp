#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) {
        std::cout << "child pid = "
                  << getpid()
                  << std::endl;

        std::cout << "child is running"
                  << std::endl;

        return 0;
    }

    std::cout << "parent pid = "
              << getpid()
              << std::endl;

    std::cout << "fork returned child pid = "
              << pid
              << std::endl;

    pid_t result = wait(nullptr);

    std::cout << "wait returned pid = "
              << result
              << std::endl;

    std::cout << "parent finished"
              << std::endl;

    return 0;
}
