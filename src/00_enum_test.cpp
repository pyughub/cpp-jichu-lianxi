#include <iostream>
using namespace std;

int main() {
  enum Color { RED = 2, GREEN = 4, BLUE };
  Color c = BLUE;
  cout << c << endl;
  return 0;
}