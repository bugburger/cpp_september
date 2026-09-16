#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    void reverseString(vector<char>& s) {
        int left = 0;
        int right = static_cast<int>(s.size()) - 1;

        while (left < right) {

            // TODO: 交换 s[left] 与 s[right]
	    swap(s[left], s[right]);

            // TODO: 移动左右指针
	    left++;
	    right--;

        }
    }
};

int main() {
    vector<char> s = {
        'h', 'e', 'l', 'l', 'o'
    };

    Solution solution;
    solution.reverseString(s);

    for (char c : s) {
        cout << c << " ";
    }

    cout << endl;

    return 0;
}
