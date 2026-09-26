#include <iostream>
#include <queue>

class MyStack {
private:
    std::queue<int> q;

public:
    MyStack() = default;

    void push(int x) {
        int n = q.size();

        q.push(x);

        for (int i = 0; i < n; ++i) {
            int y = q.front();
            q.pop();
            q.push(y);
        }
    }

    int pop() {
        int x = q.front();
        q.pop();
        return x;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }
};

int main() {
    MyStack s;

    s.push(1);
    s.push(2);
    s.push(3);

    std::cout << "top: " << s.top() << std::endl;

    std::cout << "pop: " << s.pop() << std::endl;
    std::cout << "pop: " << s.pop() << std::endl;
    std::cout << "pop: " << s.pop() << std::endl;

    std::cout << "empty: "
              << std::boolalpha
              << s.empty()
              << std::endl;

    return 0;
}
