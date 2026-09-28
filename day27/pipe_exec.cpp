#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int fd[2];

	if (pipe(fd) == -1) {
		std::cerr << "pipe failed" << std::endl;
		return 1;
	}

	pid_t pid = fork();

	if (pid < 0) {
		std::cerr << "fork failed" << std::endl;
		return 1;
	}

	if (pid == 0) {
		close(fd[0]);

		dup2(fd[1], STDOUT_FILENO);

		close(fd[1]);

		execlp("ls", "ls", "-l", nullptr);

		std::cerr << "exec failed" << std::endl;

		return 1;
	}
	 close(fd[1]);

   	 char buffer[256];

    while (true) {
        ssize_t bytes =
            read(fd[0],
                 buffer,
                 sizeof(buffer));

        if (bytes > 0) {
            std::cout.write(buffer, bytes);
        }
        else if (bytes == 0) {
            break;
        }
        else {
            std::cerr << "read failed" << std::endl;
            break;
        }
    }

    close(fd[0]);

    waitpid(pid, nullptr, 0);

    return 0;
}
