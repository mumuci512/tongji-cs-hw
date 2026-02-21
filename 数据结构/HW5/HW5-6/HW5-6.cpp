#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>

class FreqStack {
private:
    std::unordered_map<int, int> freq; // 记录每个值的频率
    std::unordered_map<int, std::stack<int>> group; // 记录每个频率对应的值的堆栈
    int maxFreq; // 当前的最大频率

public:
    FreqStack() : maxFreq(0) {}

    void push(int val) {
        freq[val]++;
        int f = freq[val];
        if (f > maxFreq) {
            maxFreq = f;
        }
        group[f].push(val);
    }

    int pop() {
        int val = group[maxFreq].top();
        group[maxFreq].pop();
        freq[val]--;
        if (group[maxFreq].empty()) {
            maxFreq--;
        }
        return val;
    }
};
