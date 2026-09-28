#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

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
		const char* message = "hello from child process";

		write(fd[1], message, strlen(message));

		close(fd[1]);

		return 0;
	}

	close(fd[1]);

	char buffer[128] = {0};

	ssize_t bytes = read(fd[0], buffer, sizeof(buffer)- 1);

	if (bytes > 0) {
		buffer[bytes] = '\0';

		std::cout << "parent received" << buffer << std::endl;
	}

	close(fd[0]);

	waitpid(pid, nullptr, 0);

	return 0;
}
