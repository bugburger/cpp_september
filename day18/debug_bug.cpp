#include <iostream>

int calculateAverage(int total, int count) {
    int average = total / count;
    return average;
}

int main() {
    int total = 100;
    int count = 4;

    count = count - 4;

    int average =
        calculateAverage(total, count);

    std::cout << "average = "
              << average
              << std::endl;

    return 0;
}
