#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
	int fd = open("test.txt", O_RDONLY);
	if (fd == -1) {
		std::cerr << "open failed\n";
		return 1;
	}

	char buffer[128];

	ssize_t n = read(fd, buffer, sizeof(buffer) - 1);

	if (n == -1) {
		std::cerr << "read failed\n";
		close(fd);
		return 1;
	}

	buffer[n] = '\0';

	std::cout << "read" << n << " bytes\n";
	std::cout << "content: " << buffer;

	close(fd);
}
