#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int numerator, denominator;
    cin >> numerator >> denominator;

    // n>30 && d=1, 10<n<=30 && d=-1, n<=10 && d=0 触发除零错误
    if (numerator > 30) {
        cout << "(pid:" << getpid() << ") Number " << numerator << " divided by " << denominator-1 << " is: " << numerator / (denominator-1) << endl;

    } else if (numerator > 10) {

        if (denominator > 10) {
            cout << "(pid:" << getpid() << ") Number " << numerator << " divided by " << denominator << " is: " << numerator / denominator << endl;

        } else {
            cout << "(pid:" << getpid() << ") Number " << numerator << " divided by " << denominator+1 << " is: " << numerator / (denominator+1) << endl;
        }

    } else {
        cout << "(pid:" << getpid() << ") Number " << numerator << " divided by " << denominator << " is: " << numerator / denominator << endl;
    }

    return 0;
}
