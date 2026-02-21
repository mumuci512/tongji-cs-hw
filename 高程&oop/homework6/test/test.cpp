#include <iostream>
#include <cmath>
#include <string>

using namespace std;

void splitNumber(const string& num, string& high, string& low) {
    int len = num.size();
    int mid = len / 2;
    high = num.substr(0, mid);
    low = num.substr(mid);
}

//字符串加法
string addStrings(const string& a, const string& b) {
    string res;
    int carry = 0, i = a.size() - 1, j = b.size() - 1;
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += a[i--] - '0';
        if (j >= 0) sum += b[j--] - '0';
        res.push_back(sum % 10 + '0');
        carry = sum / 10;
    }
    reverse(res.begin(), res.end());
    return res;
}

//字符串减法（假设 a >= b）
string subtractStrings(const string& a, const string& b) {
    string res;
    int borrow = 0, i = a.size() - 1, j = b.size() - 1;
    while (i >= 0) {
        int diff = (a[i] - '0') - (j >= 0 ? (b[j--] - '0') : 0) - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        res.push_back(diff + '0');
        i--;
    }
    while (res.size() > 1 && res.back() == '0') res.pop_back();
    reverse(res.begin(), res.end());
    return res;
}

//乘10^n，相当于字符串右边加n个0
string shiftLeft(const string& num, int n) {
    return num + string(n, '0');
}

//递归乘法
string multiply(string num1, string num2) {
    int len = max(num1.size(), num2.size());
    if (len == 1) return to_string((num1[0] - '0') * (num2[0] - '0'));

    //填充前导零，使长度为偶数
    string a = string(len - num1.size(), '0') + num1;
    string b = string(len - num2.size(), '0') + num2;

    //拆分高低部分
    string A, B, C, D;
    splitNumber(a, A, B);
    splitNumber(b, C, D);

    //计算三个关键乘积
    string AC = multiply(A, C);
    string BD = multiply(B, D);
    string AB_CD = multiply(addStrings(A, B), addStrings(C, D));
    string middle = subtractStrings(subtractStrings(AB_CD, AC), BD);

    //组合结果
    return addStrings(addStrings(shiftLeft(AC, 2 * (len / 2)), shiftLeft(middle, len / 2)), BD);
}

int main() {
    string num1, num2;
    cout << "输入两个大整数: ";
    cin >> num1 >> num2;

    cout << "乘积结果: " << multiply(num1, num2) << endl;
    return 0;
}
