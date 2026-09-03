#include <iostream>
#include <vector>

class Solution
{
public:
    int minSubArrayLen(int target, const std::vector<int>& nums)
    {
        int n = static_cast<int>(nums.size());

        int slow = 0;
        int fast = 0;
        int sum = 0;
        int result = n + 1;

        while (fast < n)
        {
            // 扩大窗口：加入右端元素
            sum += nums[fast];

            // 当前窗口满足条件时，持续缩小窗口
            while (sum >= target)
            {
                int length = fast - slow + 1;

                if (length < result)
                {
                    result = length;
                }

                // 移除窗口左端元素
                sum -= nums[slow];
                ++slow;
            }

            ++fast;
        }

        if (result == n + 1)
        {
            return 0;
        }

        return result;
    }
};

int main()
{
    Solution solution;

    std::vector<int> nums1 = {2, 3, 1, 2, 4, 3};
    std::cout << "Test 1: "
              << solution.minSubArrayLen(7, nums1)
              << '\n';

    std::vector<int> nums2 = {1, 4, 4};
    std::cout << "Test 2: "
              << solution.minSubArrayLen(4, nums2)
              << '\n';

    std::vector<int> nums3 = {1, 1, 1, 1};
    std::cout << "Test 3: "
              << solution.minSubArrayLen(10, nums3)
              << '\n';

    std::vector<int> nums4 = {8};
    std::cout << "Test 4: "
              << solution.minSubArrayLen(7, nums4)
              << '\n';

    return 0;
}
