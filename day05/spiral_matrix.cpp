#include <iostream>
#include <vector>

class Solution {
public:
    std::vector<std::vector<int>> generateMatrix(int n) {
        std::vector<std::vector<int>> matrix(
            n, std::vector<int>(n, 0)
        );

        int top = 0;
        int bottom = n - 1;
        int left = 0;
        int right = n - 1;
        int num = 1;

        while (top <= bottom && left <= right) {
            // 第一条边：从左向右
            for (int column = left; column <= right; ++column) {
                matrix[top][column] = num++;
            }
            ++top;

            // 第二条边：从上向下
            for (int row = top; row <= bottom; ++row) {
                matrix[row][right] = num++;
            }
            --right;

            // 第三条边：从右向左
            if (top <= bottom) {
                for (int column = right; column >= left; --column) {
                    matrix[bottom][column] = num++;
                }
                --bottom;
            }

            // 第四条边：从下向上
            if (left <= right) {
                for (int row = bottom; row >= top; --row) {
                    matrix[row][left] = num++;
                }
                ++left;
            }
        }

        return matrix;
    }
};

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const std::vector<int>& row : matrix) {
        for (int value : row) {
            std::cout << value << '\t';
        }

        std::cout << '\n';
    }
}

int main() {
    int n = 0;

    std::cout << "请输入矩阵大小n：";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "n必须是正整数\n";
        return 1;
    }

    Solution solution;

    std::vector<std::vector<int>> result =
        solution.generateMatrix(n);

    std::cout << "生成的螺旋矩阵：\n";
    printMatrix(result);

    return 0;
}
