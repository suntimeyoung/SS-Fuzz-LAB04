#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int number1, number2;

    cin >> number1 >> number2;

    int* ptr = new int(number1);

    if (number1 == 2) {
        number2 = number2 * 3;
    }

    if (number2 == -2) {
        number1 = number1 * 5;
    }

    // n1是5的倍数, n2是3的倍数, n1=2, n2=-2 触发双重释放
    if (number1 % 5 == 0 || number2 % 3 == 0) {
        delete ptr;
    }

    delete ptr;

    return 0;
}
