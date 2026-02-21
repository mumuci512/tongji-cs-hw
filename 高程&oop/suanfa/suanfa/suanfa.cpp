#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <utility>

using namespace std;

bool canDominate(const pair<int, int>& a, const pair<int, int>& b) {
    return a.first <= b.first && a.second <= b.second;
}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int n;
    fin >> n;

    vector<int> a(n), b(n);
    for (int i = 0; i < n; ++i) fin >> a[i];
    for (int i = 0; i < n; ++i) fin >> b[i];

    //int n = 6;
    //vector<int> a = { 2, 5, 7, 10, 5, 2 };
    //vector<int> b = { 3, 8, 4, 11, 3, 4 };

    vector<pair<int, int>> dp;
    dp.emplace_back(0, 0);

    for (int i = 0; i < n; ++i) {
        vector<pair<int, int>> newStates;
        for (const auto& state : dp) {            
            newStates.emplace_back(state.first + a[i], state.second);// 分配给A            
            newStates.emplace_back(state.first, state.second + b[i]);// 分配给B
        }

        // 剪枝：去除被支配的状态
        sort(newStates.begin(), newStates.end(), [](const pair<int, int>& x, const pair<int, int>& y) {
            return x.first < y.first || (x.first == y.first && x.second < y.second);
            });

        vector<pair<int, int>> pruned;
        for (const auto& s : newStates) {
            bool dominated = false;
            for (const auto& t : pruned) {
                if (canDominate(t, s)) {
                    dominated = true;
                    break;
                }
            }
            if (!dominated) { // 移除被s支配的旧状态                
                vector<pair<int, int>> temp;
                for (const auto& t : pruned) {
                    if (!canDominate(s, t)) {
                        temp.push_back(t);
                    }
                }
                temp.push_back(s);
                pruned.swap(temp);
            }
        }

        dp = pruned;
    }

    int min_time = INT_MAX;
    for (const auto& p : dp) {
        min_time = min(min_time, max(p.first, p.second));
    }

    fout << min_time;

    fin.close();
    fout.close();
    return 0;
}