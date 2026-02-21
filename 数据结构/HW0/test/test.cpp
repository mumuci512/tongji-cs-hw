#include <iostream>
using namespace std;

int Josephus(int n, int m)
{
    if (n == 0)
        return 0;
    else
        return (Josephus(n - 1, m) + m) % n;
}

int main()
{
    int n, m;
    cin >> n >> m;
    cout << Josephus(n, m);

    return 0;
}