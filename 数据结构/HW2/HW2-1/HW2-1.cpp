#include <iostream>
#include <stack>
#include <string>

using namespace std;

bool isValidSequence(const string& inSeq, const string& outSeq) {
    stack<char> station;
    int inIndex = 0, outIndex = 0;

    while (inIndex < inSeq.size()) {
        // 将车从入口放入车站
        station.push(inSeq[inIndex++]);

        // 当车站栈顶的列车可以按顺序出站时，弹出
        while (!station.empty() && station.top() == outSeq[outIndex]) {
            station.pop();
            outIndex++;
        }
    }

    // 如果最终栈空且所有车出站完毕，返回true
    return station.empty() && outIndex == outSeq.size();
}

int main() {
    string inSeq;

    // 读入入站序列
    cin >> inSeq;

    string outSeq;

    // 处理多组出栈序列直到EOF
    while (cin >> outSeq) {
        if (isValidSequence(inSeq, outSeq)) {
            cout << "yes" << endl;
        }
        else {
            cout << "no" << endl;
        }
    }

    return 0;
}
int main() {
    string inSeq;
    cin >> inSeq;  // 读入入站序列

    string outSeq;

    // 处理多组出栈序列直到EOF
    while (cin >> outSeq) {
        // 判断每个出栈序列是否有效
        if (isValidSequence(inSeq, outSeq)) {
            cout << "yes" << endl;  // 可以得到该出栈序列
        }
        else {
            cout << "no" << endl;   // 不可以得到该出栈序列
        }
    }

    return 0;
}