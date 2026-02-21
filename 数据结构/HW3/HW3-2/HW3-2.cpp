#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct TreeNode {
    char val;   //树节点存储的值为char型
	int left, right;    //树节点的左右孩子节点的下标
};

int buildTree(vector<TreeNode>& nodes) {
    int n = nodes.size();
    if (n == 0) return -1;

    vector<bool> isChild(n, false);
    for (int i = 0; i < n; ++i) {
        if (nodes[i].left != -1) isChild[nodes[i].left] = true;
        if (nodes[i].right != -1) isChild[nodes[i].right] = true;
    }

    for (int i = 0; i < n; ++i) {
        if (!isChild[i]) return i; // Root is the one that's not a child of any node
    }
    return -1;
}

int getDepth(const vector<TreeNode>& nodes, int root) {
    if (root == -1) return 0;
    int leftDepth = getDepth(nodes, nodes[root].left);
    int rightDepth = getDepth(nodes, nodes[root].right);
    return max(leftDepth, rightDepth) + 1;
}

bool isIsomorphic(const vector<TreeNode>& nodes1, int root1, const vector<TreeNode>& nodes2, int root2) {
    if (root1 == -1 && root2 == -1) return true;  // Both trees are empty
    if (root1 == -1 || root2 == -1) return false; // One tree is empty, the other is not
    if (nodes1[root1].val != nodes2[root2].val) return false; // Values at roots do not match

    bool withoutSwap = isIsomorphic(nodes1, nodes1[root1].left, nodes2, nodes2[root2].left) &&
        isIsomorphic(nodes1, nodes1[root1].right, nodes2, nodes2[root2].right);

    bool withSwap = isIsomorphic(nodes1, nodes1[root1].left, nodes2, nodes2[root2].right) &&
        isIsomorphic(nodes1, nodes1[root1].right, nodes2, nodes2[root2].left);

    return withoutSwap || withSwap;
}

int main() {
    int n1, n2;
    cin >> n1;

    vector<TreeNode> tree1(n1);
    for (int i = 0; i < n1; ++i) {
        char val;
        string left, right;
        cin >> val >> left >> right;
        tree1[i].val = val;
        tree1[i].left = (left == "-") ? -1 : stoi(left);
        tree1[i].right = (right == "-") ? -1 : stoi(right);
    }

    cin >> n2;

    vector<TreeNode> tree2(n2);
    for (int i = 0; i < n2; ++i) {
        char val;
        string left, right;
        cin >> val >> left >> right;
        tree2[i].val = val;
        tree2[i].left = (left == "-") ? -1 : stoi(left);
        tree2[i].right = (right == "-") ? -1 : stoi(right);
    }

    int root1 = buildTree(tree1);
    int root2 = buildTree(tree2);

    bool areIsomorphic = isIsomorphic(tree1, root1, tree2, root2);
    int depth1 = getDepth(tree1, root1);
    int depth2 = getDepth(tree2, root2);

    cout << (areIsomorphic ? "Yes" : "No") << endl;
    cout << depth1 << endl;
    cout << depth2 << endl;

    return 0;
}
