#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <algorithm>

using namespace std;

// 定义表达式树节点
struct TreeNode {
    string val;
    TreeNode* left, * right;
    TreeNode(string v) : val(v), left(nullptr), right(nullptr) {}
};

// 根据中缀表达式构建表达式树
TreeNode* buildExpressionTree(const string& expression) {
    stack<TreeNode*> values; // 栈用于操作数
    stack<char> ops;         // 栈用于操作符

    for (size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];

        // 跳过空格
        if (isspace(c)) continue;

        // 如果是数字或变量，创建一个叶子节点并入栈
        if (isalnum(c)) {
            values.push(new TreeNode(string(1, c)));
        }
        // 如果是左括号，入栈
        else if (c == '(') {
            ops.push(c);
        }
        // 如果是右括号，弹出操作符直到遇到左括号
        else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                char op = ops.top();
                ops.pop();
                TreeNode* right = values.top(); values.pop();
                TreeNode* left = values.top(); values.pop();
                TreeNode* node = new TreeNode(string(1, op));
                node->left = left;
                node->right = right;
                values.push(node);
            }
            ops.pop(); // 弹出 '('
        }
        // 如果是操作符
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            // 处理优先级
            while (!ops.empty() && (ops.top() == '*' || ops.top() == '/') && c == '+' || c == '-' ) {
                char op = ops.top();
                ops.pop();
                TreeNode* right = values.top(); values.pop();
                TreeNode* left = values.top(); values.pop();
                TreeNode* node = new TreeNode(string(1, op));
                node->left = left;
                node->right = right;
                values.push(node);
            }
            ops.push(c);
        }
    }

    // 处理栈中的剩余操作符
    while (!ops.empty()) {
        char op = ops.top();
        ops.pop();
        TreeNode* right = values.top(); values.pop();
        TreeNode* left = values.top(); values.pop();
        TreeNode* node = new TreeNode(string(1, op));
        node->left = left;
        node->right = right;
        values.push(node);
    }

    // 返回表达式树的根节点
    return values.top();
}

// 计算树的高度
int height(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(height(root->left), height(root->right));
}

// 打印树的辅助函数
void printTree(TreeNode* root, int l, int r, int level, char** tree, char** connections) {
    if (!root) return;

    int mid = (l + r) / 2;
    tree[level][mid] = root->val[0]; // 将节点的第一个字符放到正确的位置

    // 打印左子树的连接符号
    if (root->left) {
        printTree(root->left, l, mid - 1, level + 1, tree, connections);
        connections[level][(l + mid - 1) / 2] = '/';
    }

    // 打印右子树的连接符号
    if (root->right) {
        printTree(root->right, mid + 1, r, level + 1, tree, connections);
        connections[level][(mid + 1 + r) / 2] = '\\';
    }
}

// 打印表达式树
void printExpressionTree(TreeNode* root) {
    if (!root) return;

    int h = height(root); // 树的高度
    int width = (1 << h) - 1; // 2^h - 1，计算树的宽度

    // 动态分配二维数组
    char** tree = new char* [h];
    for (int i = 0; i < h; ++i) {
        tree[i] = new char[width];
        fill(tree[i], tree[i] + width, ' ');
    }

    char** connections = new char* [h - 1];
    for (int i = 0; i < h - 1; ++i) {
        connections[i] = new char[width];
        fill(connections[i], connections[i] + width, ' ');
    }

    printTree(root, 0, width - 1, 0, tree, connections);

    for (int i = 0; i < h; ++i) {
        cout << tree[i] << endl;
        if (i < h - 1)
            cout << connections[i] << endl;
    }

    // 释放动态分配的内存
    for (int i = 0; i < h; ++i) {
        delete[] tree[i];
    }
    delete[] tree;

    for (int i = 0; i < h - 1; ++i) {
        delete[] connections[i];
    }
    delete[] connections;
}

// 后根遍历（逆波兰式）
void postOrder(TreeNode* root, string* result, int& index) {
    if (!root) return;
    postOrder(root->left, result, index);
    postOrder(root->right, result, index);
    result[index++] = root->val;
}

// 计算表达式树的值
int evaluateExpressionTree(TreeNode* root, char variables[], int values[], int numVariables) {
    if (!root) return 0;
    if (root->val.size() == 1 && isalpha(root->val[0])) {
        for (int i = 0; i < numVariables; ++i) {
            if (variables[i] == root->val[0]) {
                return values[i];
            }
        }
    }

    int leftVal = evaluateExpressionTree(root->left, variables, values, numVariables);
    int rightVal = evaluateExpressionTree(root->right, variables, values, numVariables);

    if (root->val == "+") return leftVal + rightVal;
    if (root->val == "-") return leftVal - rightVal;
    if (root->val == "*") return leftVal * rightVal;
    if (root->val == "/") return leftVal / rightVal; // 整数除法

    return 0;
}

int main() {
    // 输入中缀表达式
    string expression;
    cin >> expression;

    // 输入变量值的数量
    int numVariables;
    cin >> numVariables;

    char* variables = new char[numVariables];
    int* values = new int[numVariables];
    char var;
    int value;

    // 输入变量及其值
    for (int i = 0; i < numVariables; ++i) {
        cin >> var >> value;
        variables[i] = var;
        values[i] = value;
    }

    // 构建表达式树
    TreeNode* root = buildExpressionTree(expression);

    // 1. 输出逆波兰式
    string* postfixResult = new string[expression.size()];
    int index = 0;
    postOrder(root, postfixResult, index);
    for (int i = 0; i < index; ++i) {
        cout << postfixResult[i];
    }
    cout << endl;

    // 2. 打印表达式树
    printExpressionTree(root);

    // 3. 计算表达式的值
    int result = evaluateExpressionTree(root, variables, values, numVariables);
    cout << result << endl;

    // 释放动态分配的内存
    delete[] postfixResult;
    delete[] variables;
    delete[] values;

    return 0;
}
