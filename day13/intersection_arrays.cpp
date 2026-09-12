#include <iostream>
#include <unordered_set>
#include <vector>

class Solution {
public:
    std::vector<int> intersection(
        std::vector<int>& nums1,
        std::vector<int>& nums2) {

        // 保存 nums1 中出现过的元素
        std::unordered_set<int> numsSet;

        // 保存两个数组的交集，同时自动去重
        std::unordered_set<int> resultSet;

        for (int num : nums1) {
            numsSet.insert(num);
        }

        for (int num : nums2) {
            // find() 返回迭代器。
            // 如果没有找到，返回 numsSet.end()。
            if (numsSet.find(num) != numsSet.end()) {
                resultSet.insert(num);
            }
        }

        // unordered_set 不能直接作为 vector 返回，
        // 使用迭代器区间构造 vector。
        return std::vector<int>(
            resultSet.begin(),
            resultSet.end()
        );
    }
};

void printVector(const std::vector<int>& nums) {
    std::cout << "[";

    for (std::size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i];

        if (i + 1 < nums.size()) {
            std::cout << ", ";
        }
    }

    std::cout << "]\n";
}

int main() {
    Solution solution;

    std::vector<int> nums1{1, 2, 2, 1};
    std::vector<int> nums2{2, 2};

    std::vector<int> result1 =
        solution.intersection(nums1, nums2);

    std::cout << "Test 1 result: ";
    printVector(result1);

    std::vector<int> nums3{4, 9, 5};
    std::vector<int> nums4{9, 4, 9, 8, 4};

    std::vector<int> result2 =
        solution.intersection(nums3, nums4);

    std::cout << "Test 2 result: ";
    printVector(result2);

    return 0;
}
