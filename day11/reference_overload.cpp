#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <utility>

using namespace std;

void process(const string& str)
{
    cout << "const lvalue reference: "
         << str << endl;
}

void process(string&& str)
{
    cout << "rvalue reference: "
         << str << endl;
}

int main()
{
    string s = "hello";

    // s 是左值
    process(s);

    // 临时 string 是右值
    process(string("world"));

    // std::move(s) 将 s 转换为可绑定右值引用的表达式
    process(std::move(s));

    return 0;
}
