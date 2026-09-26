#include <cstdio>
#include <iostream>

int main() {
    FILE* fp = fopen("test.txt", "r");

    if (fp == nullptr) {
        std::cerr << "fopen failed\n";
        return 1;
    }

    char buffer[128];

    std::size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);

    buffer[n] = '\0';

    std::cout << "read " << n << " bytes\n";
    std::cout << "content: " << buffer;

    fclose(fp);

    return 0;
}
