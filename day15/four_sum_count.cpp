#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
    int fourSumCount(vector<int>& nums1,
                     vector<int>& nums2,
                     vector<int>& nums3,
                     vector<int>& nums4) {

        unordered_map<int, int> mp;
        int count = 0;

        // 第一步：
        // 统计 nums1 + nums2 的所有和及出现次数
	for (int i = 0; i < nums1.size(); ++i) {
		for (int j = 0; j < nums2.size(); ++j) {
			int sum = nums1[i] + nums2[j];
			mp[sum]++;
		}
	}

        // 第二步：
        // 遍历 nums3 + nums4
        // 查找相反数
	for (int k = 0; k < nums3.size(); ++k) {
		for (int l = 0; l < nums4.size(); ++l) {
			int tar = -(nums3[k] + nums4[l]);
			if (mp.find(tar) != mp.end()) {
				count += mp[tar];
			}
		}
	}

        return count;
    }
};
