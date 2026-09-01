
#include <iostream>
#include <vector>

class Solution {
public:
    int search(std::vector<int>& nums, int target) {
        int n = static_cast<int>(nums.size());

        int left = 0;
        int right = n - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (nums[mid] == target) {
                return mid;
            }
            else if (nums[mid] < target) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }

        return -1;
    }
};

int main()
{
    std::vector<int> nums{-1, 0, 3, 5, 9, 12};

    Solution solution;

    std::cout << solution.search(nums, 9) << '\n';
    std::cout << solution.search(nums, 2) << '\n';

    return 0;
}
