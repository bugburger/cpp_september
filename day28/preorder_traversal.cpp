class Solution {
public:
    void traversal(TreeNode* node, vector<int>& result) {
        if (node == nullptr) {
            return;
        }

        result.push_back(node->val);     // 根
        traversal(node->left, result);  // 左
        traversal(node->right, result); // 右
    }

    vector<int> preorderTraversal(TreeNode* root) {
        vector<int> result;

        traversal(root, result);

        return result;
    }
};
