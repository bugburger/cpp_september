#include <iostream>
#include <unistd.h>
#include <cstdio>

int main() {
    std::cout << "before exec" << std::endl;

    execlp("ls", "ls", "-l", nullptr);

    perror("execlp");

    std::cout << "after exec" << std::endl;

    return 0;
}
