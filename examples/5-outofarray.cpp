#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int data[6] = {1, 2, 3, 4, 5, 6};

    int index1, index2;
    cin >> index1 >> index2;


    // i2=3 && (i1<0 || i1>=6), i2=4, i1=3 && (i2<0 || i2>=6), i1=2 触发数组越界
    if (index2 >= 3 && index2 < 5) {
        cout << "(pid:" << getpid() << ") data[index1] + data[index2*2] is: " << data[index1] + data[index2*2] << endl;
    }

    if (index1 < 4 && index1 >= 2) {
        cout << "(pid:" << getpid() << ") data[index1-3] + data[index2] is: " << data[index1-3] + data[index2] << endl;
    }

    return 0;
}
