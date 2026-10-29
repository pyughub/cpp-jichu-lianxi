#include <iostream>
using namespace std;

void a();
void b();
void c();
void d(int);

int main() {
  int x = 6;
  cout << "x in main is " << x << endl;
  a();
  b();
  c();
  d(x);
  x++;
  a();
  b();
  c();
  d(x);
  cout << "x in main is " << x << endl;
  return 0;
}

void a() {
  int x = 25;
  cout << "x in a is " << x << endl;
}

void b() {
  static int x = 50;
  cout << "x in b is " << x++ << endl;
}

void c() {
  extern int x;  // 如果注释掉会如何?
  x *= 10;
  cout << "x in c is " << x << endl;
}

int x = 2;

void d(int x) { cout << "x in d is " << ++x << endl; }
