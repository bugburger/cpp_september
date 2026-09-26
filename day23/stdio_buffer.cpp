#include <cstdio>
#include <unistd.h>
#include <iostream>

int main() {
    FILE* fp = fopen("buffer_test.txt", "w");

    if (fp == nullptr) {
        std::cerr << "fopen failed\n";
        return 1;
    }

    fprintf(fp, "Hello buffered IO\n");

    std::cout << "data has been passed to fprintf()\n";
    std::cout << "waiting 5 seconds before fflush...\n";

    sleep(5);

    fflush(fp);

    std::cout << "fflush finished\n";
    std::cout << "waiting another 5 seconds before fclose...\n";

    sleep(5);

    fclose(fp);

    std::cout << "fclose finished\n";

    return 0;
}
