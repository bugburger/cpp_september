#include "log_collector.hpp"

#include <iostream>

int main() {
    LogCollector collector;

    int result = collector.run("output.log");

    if (result == 0) {
        std::cout << "log collection success\n";
    } else {
        std::cout << "log collection failed\n";
    }

    return result;
}
