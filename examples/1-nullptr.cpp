#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int *ptr = NULL;
    int number1, number2;

    cin >> number1 >> number2;

    if (number1 > 0) {
        ptr = new int(number1);
    }

    // 当第一个输入为非正数时触发空指针解引用
    if (number2 < 0) {
        cout << "(pid:" << getpid() << "): *ptr is " << *ptr << endl;
    } else if (number2 == 0) {
        if (number1 < 10) {
            ptr = new int(number1);
        }
    } else {
        cout << "(pid:" << getpid() << "): *ptr is " << *ptr << endl;
    }

    if (ptr != nullptr) {
        delete ptr;
    }
    
    return 0;
}
