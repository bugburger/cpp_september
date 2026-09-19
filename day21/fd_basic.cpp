#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main() {
	int fd = open("test.txt", O_RDONLY);

	if (fd == -1) {
		std::cout << "open failed" << std::endl;
		return 1;
	}

	std::cout << "open success" << std::endl;
	std::cout << "fd" << fd << std::endl;

	close(fd);
	return 0;
}
