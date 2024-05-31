#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int number1, number2;
    cin >> number1 >> number2;

    int* ptr1 = new int(number1);
    int* ptr2 = new int(number2);

    if (number1 > 50) {
        delete ptr1;
    } else if (number2 < 30) {
        delete ptr2;
    }

    // n1>50 && n1!=n2, n2<30 && n1=n2 时触发释放后使用
    if (number2 != number1) {
        cout << "(pid:" << getpid() << ") *ptr1 is: " << *ptr1 << endl;
    } else {
        cout << "(pid:" << getpid() << ") *ptr2 is: " << *ptr2 << endl;
    }


    if (ptr1 != nullptr) {
        delete ptr1;
    }

    if (ptr2 != nullptr) {
        delete ptr2;
    }

    return 0;
}
