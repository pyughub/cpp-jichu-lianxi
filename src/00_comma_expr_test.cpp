#include <iostream>
using namespace std;

int main() {
  int i;
  for (i = 0, cout << "expr1" << endl; i < 5, cout << "expr2" << endl;
       i++, cout << "expr3" << endl) {
    cout << "in loop" << endl;
  }
  return 0;
}