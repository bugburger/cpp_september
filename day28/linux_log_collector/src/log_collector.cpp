#include "log_collector.hpp"
#include "file_descriptor.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int LogCollector::run(const std::string& log_file) {
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		std::cerr << "pipe failed\n";
		return 1;
	}

	pid_t pid = fork();

	if (pid == -1) {
		std::cerr << "fork failed\n";
		return 1;
	}

	if (pid == 0) {
		close(pipefd[0]);

		if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
			std::cerr << "dup2 failed\n";
			return 1;
		}

		close(pipefd[1]);

		execlp("ls", "ls", "-l", nullptr);

		std::cerr << "exec failed\n";
		return 1;
	}

	close(pipefd[1]);

	FileDescriptor pipe_read(pipefd[0]);

	int logfd = open(log_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (logfd == -1) {
		std::cerr << "open log file failed\n";
		return 1;
	}

	FileDescriptor log_fd(logfd);

	char buffer[1024];

	ssize_t bytes = 0;

	while ((bytes = read(pipe_read.get(), buffer, sizeof(buffer))) > 0) {
		ssize_t total_written = 0;
		while (total_written < bytes) {
			ssize_t written = write(log_fd.get(),
			      		  buffer + total_written,
			         	  bytes - total_written);
			if (written == -1) {
				std::cerr << "write failed\n";
				return 1;
			}
			total_written += written;
		}
	}

	int status = 0;

	waitpid(pid, &status, 0);

	return 0;
}
