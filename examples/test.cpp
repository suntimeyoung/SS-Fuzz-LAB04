#include <cstdio>
#include <unistd.h>
#include <iostream>

using namespace std;

int FuncA(int a) {
    return 2*a;
}

int FuncB(int b) {
    return 3*b;
}

int FuncC(int a, int b) {
    if (a * 10 < b) {
        return a + b;
    }
    return a - b;
}

int main () {
    int a, b;
    char _a, _b;
    int cnt = 2;

    cin >> _a >> _b;
    a = (int)_a;
    b = (int)_b;

    if (a > b)
        cout << "(pid:" << getpid() << ") Result of FuncC: " << FuncC(a, b) << endl;

    cout << "(pid:" << getpid() << ") Result of FuncA + FuncB: " << FuncA(a) + FuncB(b) << endl;  

    return 0;
}