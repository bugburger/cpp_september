#include <iostream>
#include <vector>

int removeElement(std::vector<int>& nums, int val)
{
    int slow = 0;
    int n = static_cast<int>(nums.size());

    for (int fast = 0; fast < n; ++fast)
    {
        if (nums[fast] != val)
        {
            nums[slow] = nums[fast];
            ++slow;
        }
    }

    return slow;
}

int main()
{
    std::vector<int> nums = {3, 2, 2, 3};
    int val = 3;

    int k = removeElement(nums, val);

    std::cout << "k = " << k << '\n';
    std::cout << "Remaining elements: ";

    for (int i = 0; i < k; ++i)
    {
        std::cout << nums[i] << ' ';
    }

    std::cout << '\n';

    return 0;
}
