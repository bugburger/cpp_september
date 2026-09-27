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

        std::cout << "child exits now"
                  << std::endl;

        return 0;
    }

    std::cout << "parent pid = "
              << getpid()
              << std::endl;

    std::cout << "parent waits for child = "
              << pid
              << std::endl;

    pid_t result = waitpid(pid, nullptr, 0);

    std::cout << "child recycled, pid = "
              << result
              << std::endl;

    std::cout << "parent sleeps for 30 seconds"
              << std::endl;

    sleep(30);

    std::cout << "parent finished"
              << std::endl;

    return 0;
}
