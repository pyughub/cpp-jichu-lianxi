# C++ 基础练习

上课自己敲的 C++ 基础语法练习。每个 `.cpp` 对应一个知识点，可以单独编译运行。

目录：

| 文件夹 | 内容 |
| --- | --- |
| `src/` | 源码。新练习建在这里 |
| `bin/` | 编译出的可执行文件 |
| `dsym/` | macOS 调试符号 |

在 Cursor 里打开 `src/` 下的文件，编译或按 F5 调试，输出会自动进 `bin/` 和 `dsym/`。

也可以用命令行（macOS / Linux 用 `make`，Windows 用 `mingw32-make`）：

```bash
make                     # 编译 src/ 下全部 .cpp
make 01_comments.cpp     # 只编译这一个
make run 01_comments.cpp # 编译并运行
make clean               # 删掉 bin/ 和 dsym/
```

| 文件 | 练习内容 |
| --- | --- |
| `src/01_comments.cpp` | 注释 |
| `src/03_variable_types.cpp` | 变量定义与声明 |
| `src/04_variable_scope.cpp` | 变量作用域 |
| `src/05_constants.cpp` | 常量与字符串字面量 |
| `src/06_modifier_types.cpp` | 类型修饰符 |
| `src/09_loops.cpp` | 循环 |
| `src/10_decision.cpp` | 判断 / switch |
| `src/11_functions.cpp` | 函数声明与定义 |
| `src/12_numbers.cpp` | 数字与数学函数 |
| `src/13_arrays.cpp` | 数组 |
| `src/14_strings.cpp` | C 风格字符串 |
| `src/15_pointers.cpp` | 指针 |
| `src/16_references.cpp` | 引用 |
| `src/17_date_time.cpp` | 日期时间 |
| `src/18_basic_io.cpp` | 输入输出 |
| `src/19_struct.cpp` | 结构体 |
| `src/20_vector.cpp` | vector |
| `src/21_data_structures.cpp` | 简单数据结构 |
