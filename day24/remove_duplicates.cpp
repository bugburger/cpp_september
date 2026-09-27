#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    string removeDuplicates(string s) {
        string res;

        for (char c : s) {
            if (!res.empty() && res.back() == c) {
                res.pop_back();
            } else {
                res.push_back(c);
            }
        }

        return res;
    }
};

int main() {
    Solution solution;

    string s1 = "abbaca";
    string s2 = "azxxzy";

    cout << "input: " << s1
         << ", result: "
         << solution.removeDuplicates(s1)
         << endl;

    cout << "input: " << s2
         << ", result: "
         << solution.removeDuplicates(s2)
         << endl;

    return 0;
}
