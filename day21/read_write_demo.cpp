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

    ssize_t bytesRead =
        read(fd, buffer, sizeof(buffer));

    if (bytesRead == -1) {
        std::cerr << "read failed" << std::endl;
        close(fd);
        return 1;
    }

    if (bytesRead > 0) {
        write(STDOUT_FILENO,
              buffer,
              bytesRead);
    }

    close(fd);

    return 0;
}
