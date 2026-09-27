#include <iostream>
#include <vector>
#include <string>
#include <stack>

using namespace std;

class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        stack<int> st;

        for (string token : tokens) {
            // 如果当前 token 是运算符
            if (token == "+" ||
                token == "-" ||
                token == "*" ||
                token == "/") {

                // 注意顺序：
                // 第一次弹出的是右操作数 b
                int b = st.top();
                st.pop();

                // 第二次弹出的是左操作数 a
                int a = st.top();
                st.pop();

                if (token == "+") {
                    st.push(a + b);
                }
                else if (token == "-") {
                    st.push(a - b);
                }
                else if (token == "*") {
                    st.push(a * b);
                }
                else {
                    st.push(a / b);
                }
            }
            else {
                // stoi：把字符串转换成 int
                st.push(stoi(token));
            }
        }

        return st.top();
    }
};

int main() {
    Solution solution;

    vector<string> tokens1 = {
        "2", "1", "+", "3", "*"
    };

    vector<string> tokens2 = {
        "4", "13", "5", "/", "+"
    };

    vector<string> tokens3 = {
        "10", "6", "3", "/", "-"
    };

    cout << "test 1 result = "
         << solution.evalRPN(tokens1)
         << endl;

    cout << "test 2 result = "
         << solution.evalRPN(tokens2)
         << endl;

    cout << "test 3 result = "
         << solution.evalRPN(tokens3)
         << endl;

    return 0;
}
