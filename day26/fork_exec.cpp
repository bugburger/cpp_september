#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        std::cout << "child pid = "
                  << getpid()
                  << std::endl;

        std::cout << "child will exec ls -l"
                  << std::endl;

        execlp("ls", "ls", "-l", nullptr);

        perror("execlp");
        return 1;
    }

    std::cout << "parent pid = "
              << getpid()
              << std::endl;

    std::cout << "parent waits child = "
              << pid
              << std::endl;

    int status = 0;

    pid_t result = waitpid(pid, &status, 0);

    if (result == -1) {
	    perror("waitpid");
	    return 1;
    }

    if (WIFEXITED(status)) {
	    std::cout << "child exit code = "
		      << WEXITSTATUS(status)
		      << std::endl;
    }

    std::cout << "child finished"
	      << std::endl;

    return 0;
}

