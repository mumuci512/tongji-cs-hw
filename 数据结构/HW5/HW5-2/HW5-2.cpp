#include <iostream>
#include <map>
#include <set>
using namespace std;

class BST {
private:
    map<int, int> countMap; // 记录每个值的出现次数
    set<int> bstSet;        // 记录所有出现的值（去重）

public:
    void insert(int x) {
        countMap[x]++;
        bstSet.insert(x);
    }

    void remove(int x) {
        if (countMap.find(x) == countMap.end() || countMap[x] == 0) {
            cout << "None" << endl;
            return;
        }
        countMap[x]--;
        if (countMap[x] == 0) {
            bstSet.erase(x);
        }
    }

    int count(int x) {
        if (countMap.find(x) != countMap.end()) {
            return countMap[x];
        }
        return 0;
    }

    int findMin() {
        return *bstSet.begin();
    }

    int findPredecessor(int x) {
        auto it = bstSet.lower_bound(x);
        if (it == bstSet.begin()) {
            return -1; // 没有前驱
        }
        --it;
        return *it;
    }
};

int main() {
    int n;
    cin >> n;

    BST bst;

    for (int i = 0; i < n; ++i) {
        int op, x;
        cin >> op;

        switch (op) {
            case 1:
                cin >> x;
                bst.insert(x);
                break;
            case 2:
                cin >> x;
                bst.remove(x);
                break;
            case 3:
                cin >> x;
                cout << bst.count(x) << endl;
                break;
            case 4:
                cout << bst.findMin() << endl;
                break;
            case 5:
                cin >> x;
                {
                    int pred = bst.findPredecessor(x);
                    if (pred == -1) {
                        cout << "None" << endl;
                    }
                    else {
                        cout << pred << endl;
                    }
                }
                break;
        }
    }

    return 0;
}
