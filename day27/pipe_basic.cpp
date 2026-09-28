#include <iostream>
#include <unistd.h>
#include <cstring>

int main() {
	int fd[2];

	if (pipe(fd) == -1) {
		std::cerr << "pipe failed" << std::endl;
		return 1;
	}

	const char* message = "hello pipe";

	write(fd[1], message, strlen(message));

	char buffer[128] = {0};

	ssize_t bytes = read(fd[0], buffer, sizeof(buffer) - 1);

	if (bytes > 0) {
		buffer[bytes] = '\0';
		std::cout << "read from pipe: "
			  << buffer
			  << std::endl;
	}

	close(fd[0]);
	close(fd[1]);

	return 0;
}
