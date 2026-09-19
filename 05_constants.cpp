#include <iostream>
#include <string>
using namespace std;
int main() {
    string greeting = "hello, C++";
    cout << greeting;
    cout << "\n";     // 换行符
    string greeting2 = "hello, \
                        C++";
    cout << greeting2;
    return 0;
}
