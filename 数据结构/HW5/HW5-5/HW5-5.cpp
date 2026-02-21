#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
using namespace std;

// 判断一个数是否为素数
bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i <= sqrt(n); ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

// 找到大于P的最小素数
int next_prime(int P) {
    int M = P;
    while (!is_prime(M)) {
        M++;
    }
    return M;
}

// 计算哈希值
unsigned long long hash_value(const string& key) {
    unsigned long long hash = 0;
    for (char c : key) {
        hash = hash * 37 + c;  // 哈希函数
    }
    return hash;
}

int main() {
    int N, P;
    cin >> N >> P;

    // 选择大于P的最小素数作为表长
    int M = (is_prime(P)) ? P : next_prime(P);

    vector<int> hash_table(M, -1);  // 散列表，-1表示位置为空
    vector<string> names(N);

    for (int i = 0; i < N; ++i) {
        cin >> names[i];
    }

    // 遍历每个名字进行插入
    for (int i = 0; i < N; ++i) {
        unsigned long long hash = hash_value(names[i]);
        int pos = hash % M;  // 初始哈希位置
        int j = 0;
        bool inserted = false;

        // 平方探测法处理冲突
        while (j < M) {
            if (hash_table[pos] == -1) {
                hash_table[pos] = i;  // 插入名字
                cout << pos << " ";
                inserted = true;
                break;
            }
            else {
                j++;
                pos = (pos + j * j) % M;  // 平方探测
            }
        }

        if (!inserted) {
            cout << "- ";
        }
    }

    cout << endl;
    return 0;
}
