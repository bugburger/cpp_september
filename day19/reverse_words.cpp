#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    string reverseWords(string s) {
        int slow = 0;

        // 1. 去除多余空格
        for (int fast = 0; fast < static_cast<int>(s.size()); ) {
            while (fast < static_cast<int>(s.size()) &&
                   s[fast] == ' ') {
                ++fast;
            }

            if (fast >= static_cast<int>(s.size())) {
                break;
            }

            if (slow != 0) {
                s[slow++] = ' ';
            }

            while (fast < static_cast<int>(s.size()) &&
                   s[fast] != ' ') {
                s[slow++] = s[fast++];
            }
        }

        s.resize(slow);

        // 2. 整体反转
        reverse(s.begin(), s.end());

        // 3. 每个单词分别反转
        int start = 0;

        for (int i = 0; i < static_cast<int>(s.size()); ++i) {
            if (s[i] == ' ') {
                reverse(s.begin() + start, s.begin() + i);
                start = i + 1;
            }
        }

        reverse(s.begin() + start, s.end());

        return s;
    }
};

int main() {
    Solution solution;

    string s = "  the sky   is blue  ";

    cout << solution.reverseWords(s) << '\n';

    return 0;
}
