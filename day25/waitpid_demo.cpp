#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child1 = fork();

    if (child1 < 0) {
        std::cerr << "fork child1 failed" << std::endl;
        return 1;
    }

    if (child1 == 0) {
        std::cout << "child1 pid = "
                  << getpid()
                  << std::endl;

        sleep(2);

        std::cout << "child1 finished"
                  << std::endl;

        return 0;
    }

    pid_t child2 = fork();

    if (child2 < 0) {
        std::cerr << "fork child2 failed" << std::endl;
        return 1;
    }

    if (child2 == 0) {
        std::cout << "child2 pid = "
                  << getpid()
                  << std::endl;

        sleep(1);

        std::cout << "child2 finished"
                  << std::endl;

        return 0;
    }

    std::cout << "parent pid = "
              << getpid()
              << std::endl;

    std::cout << "waiting for child1 = "
              << child1
              << std::endl;

    pid_t result1 = waitpid(child1, nullptr, 0);

    std::cout << "waitpid returned = "
              << result1
              << std::endl;

    pid_t result2 = wait(nullptr);

    std::cout << "wait returned = "
              << result2
              << std::endl;

    std::cout << "parent finished"
              << std::endl;

    return 0;
}
