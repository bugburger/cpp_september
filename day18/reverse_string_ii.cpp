#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    string reverseStr(string s, int k) {
        int n = static_cast<int>(s.size());

        for (int i = 0; i < n; i += 2 * k) {

            // TODO:
            // 反转从 i 开始的前 k 个字符
            // 最后一组可能不足 k 个
	    reverse(s.begin() + i, s.begin() + min(i + k, n));			
        }

        return s;
    }
};

int main() {
    string s = "abcdefg";
    int k = 2;

    Solution solution;

    cout << solution.reverseStr(s, k)
         << endl;

    return 0;
}
