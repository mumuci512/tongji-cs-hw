#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

// Function to recursively find the postorder traversal
string reconstructPostorder(const string& preorder, const string& inorder) {
    if (preorder.empty()) return "";
    char root = preorder[0];
    int rootIndex = inorder.find(root);

    // If root is not found in inorder, return "Error"
    if (rootIndex == string::npos) return "Error";

    // Split the inorder sequence into left and right subtrees
    string leftInorder = inorder.substr(0, rootIndex);
    string rightInorder = inorder.substr(rootIndex + 1);

    // Corresponding preorder sequences for left and right subtrees
    string leftPreorder = preorder.substr(1, leftInorder.size());
    string rightPreorder = preorder.substr(1 + leftInorder.size());

    // Check if sizes match; otherwise, it's an error
    if (leftPreorder.size() != leftInorder.size() || rightPreorder.size() != rightInorder.size()) {
        return "Error";
    }

    // Recursively reconstruct the left and right postorder traversals
    string leftPostorder = reconstructPostorder(leftPreorder, leftInorder);
    string rightPostorder = reconstructPostorder(rightPreorder, rightInorder);

    // If any subtree returns "Error", propagate the error
    if (leftPostorder == "Error" || rightPostorder == "Error") {
        return "Error";
    }

    // Postorder is left + right + root
    return leftPostorder + rightPostorder + root;
}

int main() {
    string line;
    int case_num = 1;
    while (getline(cin, line)) {
        if (line.empty()) break;

        size_t spacePos = line.find(' ');
        string preorder = line.substr(0, spacePos);
        string inorder = line.substr(spacePos + 1);

        // Compute the postorder traversal
        string postorder = reconstructPostorder(preorder, inorder);
        cout << postorder << endl;
    }

    return 0;
}
