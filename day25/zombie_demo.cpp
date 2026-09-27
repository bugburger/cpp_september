#include <iostream>
#include <unistd.h>

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

        std::cout << "child exits now"
                  << std::endl;

        return 0;
    }

    std::cout << "parent pid = "
              << getpid()
              << std::endl;

    std::cout << "child pid created by parent = "
              << pid
              << std::endl;

    std::cout << "parent does NOT call wait()"
              << std::endl;

    std::cout << "parent sleeps for 30 seconds"
              << std::endl;

    sleep(30);

    std::cout << "parent finished"
              << std::endl;

    return 0;
}
