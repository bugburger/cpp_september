#include <iostream>
#include <vector>

class Solution {
public:
    std::vector<int> sortedSquares(std::vector<int>& nums)
    {
        int n = static_cast<int>(nums.size());

        std::vector<int> result(n);

        int left = 0;
        int right = n - 1;
        int pos = n - 1;

        while (left <= right)
        {
            int leftSquare = nums[left] * nums[left];
            int rightSquare = nums[right] * nums[right];

            if (leftSquare > rightSquare)
            {
                result[pos] = leftSquare;
                ++left;
            }
            else
            {
                result[pos] = rightSquare;
                --right;
            }

            --pos;
        }

        return result;
    }
};

void printVector(const std::vector<int>& values)
{
    for (int value : values)
    {
        std::cout << value << ' ';
    }

    std::cout << '\n';
}

int main()
{
    Solution solution;

    std::vector<int> nums1 = {-4, -1, 0, 3, 10};
    std::vector<int> result1 = solution.sortedSquares(nums1);

    std::cout << "Test 1: ";
    printVector(result1);

    std::vector<int> nums2 = {-7, -3, 2, 3, 11};
    std::vector<int> result2 = solution.sortedSquares(nums2);

    std::cout << "Test 2: ";
    printVector(result2);

    return 0;
}
