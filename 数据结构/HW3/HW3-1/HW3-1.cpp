#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <unordered_map>

//定义二叉树节点
struct TreeNode {
    char val;
    TreeNode* left; //左孩子节点
	TreeNode* right;    //右孩子节点
	TreeNode(char x) : val(x), left(nullptr), right(nullptr) {} //构造函数
};

// 后序遍历函数
void postOrderTraversal(TreeNode* root, std::string& result) {
    if (root == nullptr) return;
    postOrderTraversal(root->left, result);
    postOrderTraversal(root->right, result);
    result += root->val;
}

int main() {
    int n;
    std::cin >> n;
    std::stack<TreeNode*> nodeStack;
    TreeNode* root = nullptr;
    TreeNode* lastNode = nullptr;  // 上一个被弹出的节点

    for (int i = 0; i < 2 * n; ++i) {
        std::string operation;
        std::cin >> operation;

        if (operation == "push") {
            char nodeVal;
            std::cin >> nodeVal;
            TreeNode* newNode = new TreeNode(nodeVal);

            if (lastNode != nullptr) {
                lastNode->right = newNode;
            }
            else
            {
                if (!nodeStack.empty()) {
                    // 如果栈顶节点的左子节点为空，则设置为左子节点，否则为右子节点
                    if (nodeStack.top()->left == nullptr) {
                        nodeStack.top()->left = newNode;
                    }
                    else {
                        nodeStack.top()->right = newNode;
                    }
                }
                else if (root == nullptr) {
                    // 根节点
                    root = newNode;
                }
            }

            lastNode = nullptr;
            nodeStack.push(newNode);
        }
        else if (operation == "pop") {
            // 弹出节点
            lastNode = nodeStack.top();
            nodeStack.pop();
        }
    }

    std::string result;
    postOrderTraversal(root, result);
    std::cout << result << std::endl;

    return 0;
}

