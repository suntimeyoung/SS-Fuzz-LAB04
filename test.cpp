#include <cstdio>
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
    cin >> a >> b;
    if (a > b) {
        cout << FuncC(a, b) << endl;
    }
    cout << FuncA(a) + FuncB(b) << endl;
}