#include <cstring>
#include <iostream>

using namespace std;

// 声明一个结构体类型 Books
struct Books {
  char title[50];
  char author[50];
  char subject[100];
  int book_id;
};

int main() {
  Books Book1;
  Books Book2;

  // Book1 详述
  strcpy(Book1.title, "C++ 入门笔记");
  strcpy(Book1.author, "foo");
  strcpy(Book1.subject, "编程语言");
  Book1.book_id = 202601;

  // Book2 详述
  strcpy(Book2.title, "算法练习");
  strcpy(Book2.author, "bar");
  strcpy(Book2.subject, "数据结构");
  Book2.book_id = 202602;

  cout << "第一本书标题 : " << Book1.title << endl;
  cout << "第一本书作者 : " << Book1.author << endl;
  cout << "第一本书类目 : " << Book1.subject << endl;
  cout << "第一本书 ID : " << Book1.book_id << endl;

  cout << "第二本书标题 : " << Book2.title << endl;
  cout << "第二本书作者 : " << Book2.author << endl;
  cout << "第二本书类目 : " << Book2.subject << endl;
  cout << "第二本书 ID : " << Book2.book_id << endl;

  return 0;
}
