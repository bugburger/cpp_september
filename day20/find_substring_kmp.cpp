#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = needle.size();

        // 本地练习增加的边界处理
        if (n == 0) {
            return 0;
        }

        // =========================
        // 第一部分：构造 next 数组
        // next[i] 表示 needle[0...i]
        // 最长相等真前缀和真后缀的长度
        // =========================
        vector<int> next(n, 0);

        int j = 0;

        for (int i = 1; i < n; ++i) {

            // 当前最长方案失败
            // 尝试更短的可复用前后缀
            while (j > 0 && needle[i] != needle[j]) {
                j = next[j - 1];
            }

            // 当前字符可以继续匹配
            if (needle[i] == needle[j]) {
                ++j;
            }

            next[i] = j;
        }

        // =========================
        // 第二部分：KMP 匹配
        // i：haystack 当前比较位置
        // j：needle 当前比较位置
        // =========================
        for (int i = 0, j = 0;
             i < static_cast<int>(haystack.size());
             ++i) {

            // 失配时：
            // haystack 的 i 不回退
            // needle 的 j 根据 next 数组回退
            while (j > 0 && haystack[i] != needle[j]) {
                j = next[j - 1];
            }

            if (haystack[i] == needle[j]) {
                ++j;
            }

            // needle 已经全部匹配完成
            if (j == n) {
                return i - j + 1;
            }
        }

        return -1;
    }
};

void runTest(
    Solution& solution,
    const string& haystack,
    const string& needle,
    int expected
) {
    int result = solution.strStr(haystack, needle);

    cout << "haystack = \"" << haystack << "\"\n";
    cout << "needle   = \"" << needle << "\"\n";
    cout << "result   = " << result << '\n';
    cout << "expected = " << expected << '\n';

    if (result == expected) {
        cout << "[PASS]\n";
    } else {
        cout << "[FAIL]\n";
    }

    cout << "-----------------------------\n";
}

int main() {
    Solution solution;

    runTest(solution, "sadbutsad", "sad", 0);
    runTest(solution, "leetcode", "leeto", -1);
    runTest(solution, "hello", "ll", 2);

    // 专门用于观察 KMP 重复结构
    runTest(solution, "abababac", "ababac", 2);

    // needle 与 haystack 完全相同
    runTest(solution, "abc", "abc", 0);

    // needle 在最后
    runTest(solution, "abcdef", "def", 3);

    // needle 不存在
    runTest(solution, "aaaaa", "bba", -1);

    return 0;
}
