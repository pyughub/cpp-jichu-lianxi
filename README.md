# C++ 基础练习

上课自己敲的 C++ 基础语法练习。每个 `.cpp` 对应一个知识点，可以单独编译运行。

macOS / Linux 用 `make`，Windows（MinGW）用 `mingw32-make`：

```bash
make                     # 编译全部 .cpp
make 01_comments.cpp     # 只编译这一个
make run 01_comments.cpp # 编译并运行
make clean               # 删掉生成的可执行文件
```

也可以直接调用编译器：

```bash
c++ -std=c++17 -Wall 11_functions.cpp -o 11_functions
./11_functions
```

| 文件 | 练习内容 |
| --- | --- |
| `01_comments.cpp` | 注释 |
| `03_variable_types.cpp` | 变量定义与声明 |
| `04_variable_scope.cpp` | 变量作用域 |
| `05_constants.cpp` | 常量与字符串字面量 |
| `06_modifier_types.cpp` | 类型修饰符 |
| `09_loops.cpp` | 循环 |
| `10_decision.cpp` | 判断 / switch |
| `11_functions.cpp` | 函数声明与定义 |
| `12_numbers.cpp` | 数字与数学函数 |
| `13_arrays.cpp` | 数组 |
| `14_strings.cpp` | C 风格字符串 |
| `15_pointers.cpp` | 指针 |
| `16_references.cpp` | 引用 |
| `17_date_time.cpp` | 日期时间 |
| `18_basic_io.cpp` | 输入输出 |
| `19_struct.cpp` | 结构体 |
| `20_vector.cpp` | vector |
| `21_data_structures.cpp` | 简单数据结构 |
