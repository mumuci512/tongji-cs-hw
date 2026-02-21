#include <iostream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

int calculateOriginalDepth(const string& directions) {
    int currentDepth = 0, maxDepth = 0;
    for (char dir : directions) {
        if (dir == 'd') {
            currentDepth++;
            maxDepth = max(maxDepth, currentDepth);
        }
        else if (dir == 'u') {
            currentDepth--;
        }
    }
    return maxDepth;
}

static int calculateBinaryTreeDepth(const string& directions) {
    int currentDepth = 0;
    int maxDepth = 0;
    vector<int> curDepthNum(2000000, 0); // 使用 vector 动态分配内存

    for (char c : directions) {
        if (c == 'd') {
            currentDepth++;
            if (curDepthNum[currentDepth] > 0) { // 如果当前深度已经有节点了，说明是兄弟节点
                currentDepth++;
            }
            curDepthNum[currentDepth]++;
            if (currentDepth > maxDepth) {
                maxDepth = currentDepth;
            }
        }
        else if (c == 'u') {
            currentDepth--;
        }
    }

    return maxDepth;
}


int main() {
    string directions;
    int treeNumber = 1;

    while (getline(cin, directions)) {
        if (directions[0] == '#') break;  // Stop input on '#' line
        if (directions[0] == '\0') continue;

        // Calculate the depth of the original tree
        int originalDepth = calculateOriginalDepth(directions);

        // Calculate the depth of the binary tree (child-sibling representation)
        int binaryTreeDepth = calculateBinaryTreeDepth(directions);

        // Output the results
        cout << "Tree " << treeNumber << ": " << originalDepth << " => " << binaryTreeDepth << endl;

        treeNumber++;
    }

    return 0;
}

