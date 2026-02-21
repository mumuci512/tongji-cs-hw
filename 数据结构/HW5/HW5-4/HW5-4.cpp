#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

using namespace std;

// 使用哈希表
unordered_map<string, unordered_set<string>> children;  // 记录每个人的孩子
unordered_map<string, string> parents;  // 记录每个人的父母
unordered_map<string, unordered_set<string>> ancestors;  // 记录每个人的祖先

// 递归填充祖先信息
void findAncestors(const string& person) {
    if (ancestors.find(person) != ancestors.end()) return;  // 已经计算过了
    if (parents.find(person) != parents.end()) {
        string parent = parents[person];
        ancestors[person].insert(parent);
        ancestors[person].insert(ancestors[parent].begin(), ancestors[parent].end());
    }
}

// 处理家谱数据
void processFamilyTree(const vector<string>& familyTree) {
    children.clear();
    parents.clear();
    ancestors.clear();

    string ancestor = familyTree[0];  // 第一个人是祖先
    unordered_map<int, string> indentations;  // 记录每个名字对应的缩进深度
    indentations[0] = ancestor;

    for (int i = 1; i < familyTree.size(); ++i) {
        string name = familyTree[i];
        int indent = 0;
        while (name[indent] == ' ') ++indent;  // 计算当前名字的缩进
        name = name.substr(indent);  // 去掉前导空格
        indentations[indent] = name;

        // 计算父母
        string parent = indentations[indent - 1];
        parents[name] = parent;
        children[parent].insert(name);  // 记录父母和孩子的关系
    }

    // 计算每个人的所有祖先
    for (const auto& entry : children) {
        findAncestors(entry.first);
    }
}

// 处理查询并返回结果
bool isChild(const string& x, const string& y) {
    return parents.find(x) != parents.end() && parents[x] == y;
}

bool isParent(const string& x, const string& y) {
    return parents.find(y) != parents.end() && parents[y] == x;
}

bool isSibling(const string& x, const string& y) {
    return parents.find(x) != parents.end() && parents.find(y) != parents.end() && parents[x] == parents[y];
}

bool isDescendant(const string& x, const string& y) {
    return ancestors.find(x) != ancestors.end() && ancestors[x].count(y);
}

bool isAncestor(const string& x, const string& y) {
    return ancestors.find(y) != ancestors.end() && ancestors[y].count(x);
}

int main() {
    int n, m;

    while (cin >> n >> m, n != 0 && m != 0) {
        cin.ignore();  // 忽略掉换行符

        vector<string> familyTree;
        string line;

        // 读取家谱
        for (int i = 0; i < n; ++i) {
            getline(cin, line);
            familyTree.push_back(line);
        }

        // 处理家谱
        processFamilyTree(familyTree);

        // 处理查询语句
        for (int i = 0; i < m; ++i) {
            getline(cin, line);
            istringstream iss(line);
            string word1, word2, word3, word4;
            iss >> word1 >> word2 >> word3 >> word4;

            bool result = false;

            if (word2 == "is" && word3 == "a" && word4 == "child") {
                result = isChild(word1, word4);
            }
            else if (word2 == "is" && word3 == "the" && word4 == "parent") {
                result = isParent(word1, word4);
            }
            else if (word2 == "is" && word3 == "a" && word4 == "sibling") {
                result = isSibling(word1, word4);
            }
            else if (word2 == "is" && word3 == "a" && word4 == "descendant") {
                result = isDescendant(word1, word4);
            }
            else if (word2 == "is" && word3 == "an" && word4 == "ancestor") {
                result = isAncestor(word1, word4);
            }

            if (result) {
                cout << "True" << endl;
            }
            else {
                cout << "False" << endl;
            }
        }

        cout << endl;
    }

    return 0;
}
