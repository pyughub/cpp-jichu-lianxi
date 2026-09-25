#include <iostream>
class MyClass {
public:
  static int class_var; // 类作用域变量
};
int MyClass::class_var = 30;
int main() {
  std::cout << "类变量: " << MyClass::class_var << std::endl;
  return 0;
}
