#include <fcntl.h>
#include <unistd.h>

#include <iostream>

int main() {
    // 打开源文件
    int sourceFd = open("source.txt", O_RDONLY);

    if (sourceFd == -1) {
        std::cerr << "open source failed"
                  << std::endl;
        return 1;
    }

    // 打开 / 创建目标文件
    int destinationFd = open(
        "destination.txt",
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (destinationFd == -1) {
        std::cerr << "open destination failed"
                  << std::endl;

        close(sourceFd);

        return 1;
    }

    char buffer[128];

    while(true) {
	    ssize_t bytesRead = read(sourceFd, buffer, sizeof(buffer));

	    if (bytesRead == -1) {
		    std::cout << "read failed" << std::endl;
		    close(sourceFd);
		    close(destinationFd);

		    return 1;
	    }

	    if (bytesRead == 0) {
		    break;
	    }
	    ssize_t totalWritten = 0;
	    while (totalWritten < bytesRead) {
	   	 ssize_t bytesWritten = 
			 write(destinationFd, buffer + totalWritten, bytesRead - totalWritten);

	         if (bytesWritten == -1) {
		         std::cerr << "write failed" << std::endl;
		         close(sourceFd);
		         close(destinationFd);

		         return 1;
	         }
		 totalWritten += bytesWritten;
	    }
    }

    close(sourceFd);
    close(destinationFd);

    std::cout << "file copy success" << std::endl;

    return 0;
}
