#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int num = 10;

	std::cout << "before fork: num = " << num << std::endl;

	pid_t pid = fork();

	if (pid < 0) {
		std::cerr << "fork failed" << std::endl;
		return 1;
	}

	if (pid == 0) {
		num = 100;

		std::cout << "child process" << std::endl;
                std::cout << "child pid = " << getpid() << std::endl;
                std::cout << "child num = " << num << std::endl;
	} else {
		wait(nullptr);

		num = 200;

		std::cout << "parent process" << std::endl;
		std::cout << "parent pid" << getpid() << std::endl;
		std::cout << "parent num = " << num << std::endl;
	}
	return 0;
};
