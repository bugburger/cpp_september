#include <iostream>

void stack_demo()
{
    int stack_value = 10;

    std::cout << "Stack value = "
              << stack_value << '\n';

    std::cout << "Stack address = "
              << &stack_value << '\n';

    // 函数结束时，stack_value 自动销毁
}

void heap_demo()
{
    int* heap_value = new int(20);

    std::cout << "Heap value = "
              << *heap_value << '\n';

    std::cout << "Heap address = "
              << heap_value << '\n';

    delete heap_value;
    heap_value = nullptr;

    std::cout << "Heap memory released\n";

    if (heap_value == nullptr)
    {
        std::cout << "heap_value is nullptr\n";
    }
}

void heap_array_demo()
{
    int* numbers = new int[5]{10, 20, 30, 40, 50};

    std::cout << "Heap array: ";

    for (int i = 0; i < 5; ++i)
    {
        std::cout << numbers[i] << ' ';
    }

    std::cout << '\n';

    delete[] numbers;
    numbers = nullptr;

    std::cout << "Heap array released\n";
}

int main()
{
    std::cout << "===== Stack demo =====\n";
    stack_demo();

    std::cout << "\n===== Heap demo =====\n";
    heap_demo();

    std::cout << "\n===== Heap array demo =====\n";
    heap_array_demo();

    return 0;
}
