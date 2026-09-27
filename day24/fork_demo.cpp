#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int num = 10;

    std::cout << "before fork" << std::endl;
    std::cout << "current pid = " << getpid() << std::endl;

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) {
        num = 20;

        std::cout << "\n===== child process =====" << std::endl;
        std::cout << "fork return value = " << pid << std::endl;
        std::cout << "child pid = " << getpid() << std::endl;
        std::cout << "parent pid = " << getppid() << std::endl;
        std::cout << "child num = " << num << std::endl;
    } else {
        num = 30;

        wait(nullptr);

        std::cout << "\n===== parent process =====" << std::endl;
        std::cout << "fork return value = " << pid << std::endl;
        std::cout << "parent pid = " << getpid() << std::endl;
        std::cout << "parent num = " << num << std::endl;
    }

    return 0;
}
