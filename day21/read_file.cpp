#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
	int fd = open("test.txt", O_RDONLY);

	if (fd == -1) {
		std::cerr << "open failed" << std::endl;
		return 1;
	}
	char buffer[128];

	int bytesRead = read(fd, buffer, sizeof(buffer) - 1);

	if (bytesRead > 0) {
		buffer[bytesRead] = '\0';

		std::cout << buffer << std::endl;
	}

	if (bytesRead == -1) {
		std::cerr << "read failed" << std::endl;
		close(fd);
		return 1;
	}

	std::cout << "bytes read = " << bytesRead << std::endl;

	close(fd);

	return 0;
}
