#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {

        vector<vector<int>> result;

        sort(nums.begin(), nums.end());

	int n = static_cast<int>(nums.size());

        for (int i = 0; i < n; ++i) {

            // 如果 nums[i] 已经大于 0，
            // 后面不可能再找到和为 0 的三元组
	    if (nums[i] > 0) {
	    	break;
	    }		
            // 对固定的第一个数进行去重
	    if (i > 0 && nums[i] == nums[i-1]) {
		continue;
	    }

            int left = i + 1;
            int right = n - 1;

            while (left < right) {

                int sum =
                    nums[i] +
                    nums[left] +
                    nums[right];

                if (sum < 0) {
                    // 总和太小
		    ++left;
                }
                else if (sum > 0) {
                    // 总和太大
		    --right;
                }
                else {
		    result.push_back({nums[i], nums[left], nums[right]});
                    // 找到三元组
		    while (left < right && nums[left] == nums[left+1]) {
			    ++left;
		    }
                    // left 去重
		    while (left < right && nums[right] == nums[right-1]) {
			    --right;
		    }
                    // right 去重
		    ++left;
		    --right;

                    // 移动双指针

                }
            }
        }

        return result;
    }
};

int main() {

    vector<int> nums = {
        -1, 0, 1, 2, -1, -4
    };

    Solution solution;

    vector<vector<int>> result =
        solution.threeSum(nums);

    for (const auto& group : result) {

        cout << "[";

        for (std::size_t i = 0; i < group.size(); ++i) {
            cout << group[i];

            if (i + 1 < group.size()) {
                cout << ", ";
            }
        }

        cout << "]" << endl;
    }

    return 0;
}
