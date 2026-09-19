#include <iostream>
#include <string>
using namespace std;

class Person {
private:
    string name;
    int age;
public:
    Person(string n, int a) : name(n), age(a) {}
    void printInfo() {
        cout << "Name: " << name << ", Age: " << age << endl;
    }
};

int main() {
    Person p("Bob", 30);
    p.printInfo(); // 输出: Name: Bob, Age: 30
    return 0;
}
