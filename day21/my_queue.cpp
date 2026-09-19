#include <iostream>
#include <stack>

using namespace std;

class MyQueue {
private:
    stack<int> stackIn;
    stack<int> stackOut;

public:
    MyQueue() = default;

    void push(int x) {
        stackIn.push(x);
    }

    int pop() {
        if (stackOut.empty()) {
            while (!stackIn.empty()) {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }

        int result = stackOut.top();
        stackOut.pop();

        return result;
    }

    int peek() {
        if (stackOut.empty()) {
            while (!stackIn.empty()) {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }

        return stackOut.top();
    }

    bool empty() {
        return stackIn.empty() && stackOut.empty();
    }
};

int main() {
    MyQueue queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);

    cout << "peek: "
         << queue.peek()
         << endl;

    cout << "pop: "
         << queue.pop()
         << endl;

    cout << "pop: "
         << queue.pop()
         << endl;

    queue.push(4);

    cout << "peek: "
         << queue.peek()
         << endl;

    cout << "empty: "
         << queue.empty()
         << endl;

    return 0;
}
