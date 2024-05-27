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
    int cnt = 2;
    do {
        cin >> a >> b;
        if (a > b)
            cout << "(pid:" << getpid() << ") Result of FuncC: " << FuncC(a, b) << endl;
        cout << "(pid:" << getpid() << ") Result of FuncA + FuncB: " << FuncA(a) + FuncB(b) << endl;  
    } while (--cnt > 0);

    return 0;
}