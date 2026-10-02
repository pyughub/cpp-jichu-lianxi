#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

int main() {
  srand(time(0));
  int randmax = RAND_MAX;
  cout << "RAND_MAX: " << randmax << endl;
  return 0;
}