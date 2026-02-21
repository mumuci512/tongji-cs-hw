#include <iostream>
using namespace std;

int main()
{
    const char* p;
    for (p = "¨X"; *p != '\0'; p++)
        cout << *p << "/";
    cout << endl;
    cout << strlen("¨X") << endl;
    for (p = "**¨X¨T¨j¨T¨[ÖÐ"; *p != '\0'; p++)
        cout << *p << "/";
    cout << strlen("**¨X¨T¨j¨T¨[ÖÐ") << endl;
    return 0;
}
