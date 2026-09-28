#include <iostream>
#include <vector>
#include <deque>

using namespace std;

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        deque<int> dq;
        vector<int> result;

        for (int i = 0; i < nums.size(); ++i) {

            // 1. 删除已经离开窗口的下标
            if (!dq.empty() && dq.front() < i - k + 1) {
                dq.pop_front();
            }

            // 2. 保持队列对应的数值单调递减
            while (!dq.empty() && nums[dq.back()] < nums[i]) {
                dq.pop_back();
            }

            // 3. 当前下标入队
            dq.push_back(i);

            // 4. 窗口形成后记录最大值
            if (i >= k - 1) {
                result.push_back(nums[dq.front()]);
            }
        }

        return result;
    }
};

int main() {
    Solution s;

    vector<int> nums = {
        1, 3, -1, -3, 5, 3, 6, 7
    };

    int k = 3;

    vector<int> result =
        s.maxSlidingWindow(nums, k);

    for (int x : result) {
        cout << x << " ";
    }

    cout << endl;

    return 0;
}
