位置按当前带注释的源码核对。继续在源码里加注释后，行号会再往后移。
 这两个文件以 **C++11** 为上限。`src/loguru.hpp` 放接口、宏和模板，`src/loguru.cpp` 放实现。没有 `constexpr`、`static_assert`、`enum class`、`if constexpr`、折叠表达式、结构化绑定，也没有智能指针。

## C++11 语言特性

| 特性                               | English                                                   | 位置                                                         | 讲解                                                         |
| ---------------------------------- | --------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| `nullptr`                          | nullptr                                                   | [src/loguru.hpp:308](src/loguru.hpp#L308)、[701](src/loguru.hpp#L701)、[840](src/loguru.hpp#L840)；[src/loguru.cpp:216](src/loguru.cpp#L216)–[218](src/loguru.cpp#L218) | 空指针常量，类型是 `std::nullptr_t`，可隐式转成任意对象指针、函数指针或成员指针。写法是 `p = nullptr`、`p == nullptr`。它不是整数；`NULL` 常为 `0`，重载时可能选中 `int` 参数。解引用空指针是未定义行为。 |
| `auto`                             | auto                                                      | [src/loguru.hpp:352](src/loguru.hpp#L352)、[1049](src/loguru.hpp#L1049)；[src/loguru.cpp:191](src/loguru.cpp#L191)、[524](src/loguru.cpp#L524)、[1011](src/loguru.cpp#L1011) | 用初始化式推导变量类型，声明时必须初始化。`auto x = e;` 会丢掉顶层引用和 `const`，要保留就写 `const auto&` 或 `auto&&`。C++11 里 `auto` 不能作函数参数；`auto x{1}` 的类型是 `std::initializer_list<int>`，不是 `int`。 |
| 类型别名 `using`                   | alias declaration                                         | [src/loguru.hpp:409](src/loguru.hpp#L409) `using Verbosity = int`，[1034](src/loguru.hpp#L1034) `using Printer`；[src/loguru.cpp:187](src/loguru.cpp#L187)–[189](src/loguru.cpp#L189)、[1840](src/loguru.cpp#L1840) | `using 名字 = 类型;` 给已有类型起别名，还能写成模板别名。别名和原类型是同一类型，不能靠它区分重载。不要和 using 声明（`using std::string;`）、using 指令（`using namespace std;`）混在一起。 |
| 带底层类型的 `enum`                | enumeration with fixed underlying type                    | [src/loguru.hpp:423](src/loguru.hpp#L423) `enum NamedVerbosity : Verbosity`。这不是 `enum class`，枚举项在 `loguru` 命名空间里 | `enum 名 : 底层类型 { 枚举项 };` 固定枚举的整数宽度。枚举项注入外层作用域，和整型仍可隐式互转。这不是 `enum class`：没有自己的作用域，名字会污染外层，也不会禁止整型转换。 |
| 右值引用、移动构造                 | rvalue reference, move constructor                        | [src/loguru.hpp:305](src/loguru.hpp#L305) `Text(Text&&)`，[824](src/loguru.hpp#L824) `LogScopeRAII(LogScopeRAII&&)`，[1036](src/loguru.hpp#L1036) `Printer&&` | `T&&` 绑即将销毁的对象。移动构造 `T(T&&)` 偷走资源，并把源对象留在可析构的空状态。移动后源对象仍要能安全析构。模板里的 `T&&` 是转发引用，左右值都能绑。移动构造最好是 `noexcept`，否则 `vector` 扩容可能退回拷贝。 |
| `= delete`                         | deleted function                                          | [src/loguru.hpp:316](src/loguru.hpp#L316)–[318](src/loguru.hpp#L318)、[832](src/loguru.hpp#L832)–[834](src/loguru.hpp#L834)、[999](src/loguru.hpp#L999)–[1002](src/loguru.hpp#L1002) | `函数签名 = delete;` 声明该函数存在，但禁止调用，也阻止编译器再生成它。常用来删掉拷贝构造和拷贝赋值，做成只移动类型。被删函数仍参与重载决议，一旦被选中就是编译错误，比“不声明”更能拦住隐式转换。 |
| `= default`                        | defaulted function                                        | [src/loguru.hpp:824](src/loguru.hpp#L824) 默认移动构造               | `函数签名 = default;` 让编译器按默认规则生成特殊成员。类内默认是 inline，类外默认则不是。自己声明了析构或拷贝后，移动的隐式生成会被抑制，需要移动时要显式 `= default`。成员里有不可移动的，默认移动会失败。 |
| 非静态成员默认初始化               | non-static data member initializer                        | [src/loguru.hpp:518](src/loguru.hpp#L518)–[546](src/loguru.hpp#L546)、[839](src/loguru.hpp#L839)–[843](src/loguru.hpp#L843)、[1014](src/loguru.hpp#L1014) | 在成员声明处写 `类型 名 = 初值;` 或 `名{初值}`。构造函数的初始化列表没提到它时，用这个初值。初始化顺序仍是声明顺序。C++11 里带这种初值的类不再是聚合，不能再当聚合做花括号初始化；C++14 才恢复。 |
| 花括号初始化                       | list initialization                                       | [src/loguru.hpp:612](src/loguru.hpp#L612) `options = {}`，[844](src/loguru.hpp#L844) `_name[...] = {}`；[src/loguru.cpp:221](src/loguru.cpp#L221) `{0}`，[1226](src/loguru.cpp#L1226) `REPLACE_LIST = { ... }` | `T x{}`、`T x = {}`、`f({})` 用花括号给对象初值，并禁止窄化转换（例如 `int` 到 `char` 会报错）。空花括号是值初始化：标量变 0，类类型调用默认构造。若类型有 `initializer_list` 构造函数，花括号会优先选它。 |
| 变参模板                           | variadic template                                         | [src/loguru.hpp:383](src/loguru.hpp#L383)、[756](src/loguru.hpp#L756)、[763](src/loguru.hpp#L763)、[853](src/loguru.hpp#L853)。这些都在 `#if LOGURU_USE_FMTLIB` 里，默认不参与编译 | `template<class... Args>` 接受任意个数、任意类型的参数，用 `Args...` 展开。空参数包合法。它和 C 的 `...` 不同，每个实参都有类型。展开必须出现在包可见的地方；只写 `Args` 而不展开是语法错误。 |
| 函数模板显式特化 `template<>`      | explicit specialization                                   | [src/loguru.hpp:874](src/loguru.hpp#L874)–[878](src/loguru.hpp#L878) `format_value` | `template<> 返回类型 函数<具体类型>(参数) { }` 为某一组模板实参另写一份实现。特化必须在第一次使用前已经被看到，否则会去实例化主模板。函数模板不能偏特化，只能全特化或另写重载。 |
| 类模板偏特化、数组引用             | partial class template specialization, reference to array | [src/loguru.hpp:1084](src/loguru.hpp#L1084)–[1085](src/loguru.hpp#L1085) `const char(&)[N]` | 类模板可以只特化部分参数：`template<size_t N> struct X<const char(&)[N]>`。`const char(&)[N]` 是长度为 N 的数组的引用，能保住长度。数组作函数参数会退化成指针，匹配不到这个偏特化。函数模板没有偏特化。 |
| `decltype`                         | decltype                                                  | [src/loguru.hpp:1146](src/loguru.hpp#L1146)；[src/loguru.cpp:171](src/loguru.cpp#L171) | `decltype(表达式)` 取该表达式的类型，不求值。变量名会保留引用和 cv 限定；多加一层括号 `decltype((x))` 会得到引用。它不丢信息，和 `auto` 的推导规则不同。 |
| `override`                         | override                                                  | [src/loguru.hpp:1047](src/loguru.hpp#L1047)                          | 写在派生类虚函数末尾，要求它确实覆盖了基类的虚函数，否则编译失败。签名必须一致，包括参数、`const` 和引用限定。漏写 `const` 时，没有 `override` 会变成隐藏基类函数，而不是覆盖。 |
| `noexcept(false)`                  | noexcept specifier                                        | [src/loguru.hpp:1450](src/loguru.hpp#L1450)、[1485](src/loguru.hpp#L1485)；定义在 [src/loguru.cpp:1809](src/loguru.cpp#L1809)、[1814](src/loguru.cpp#L1814)。析构被允许抛异常 | `noexcept` 表示不抛异常，`noexcept(false)` 表示允许抛。析构函数默认是 `noexcept`。声明了不抛却真的抛出，会调用 `std::terminate`。栈展开过程中析构再抛异常，也会直接终止。 |
| `thread_local`                     | thread storage duration                                   | [src/loguru.cpp:1105](src/loguru.cpp#L1105)、[1846](src/loguru.cpp#L1846) | 每个线程各有一份对象。首次经过声明时初始化，线程结束时析构。各线程的构造、析构互不影响。不要把这份对象的指针交给别的线程去使用。动态初始化的 `thread_local` 有一次性开销。 |
| lambda                             | lambda expression                                         | [src/loguru.cpp:230](src/loguru.cpp#L230) 立即调用，[1572](src/loguru.cpp#L1572) 作为线程入口 | `[捕获](参数) { 函数体 }` 定义匿名函数对象，后面加 `()` 就是立即调用。`[=]` 按值捕获，`[&]` 按引用捕获。按引用捕获的局部变量若在 lambda 之后才用，会变成悬空引用。C++11 的 lambda 默认不能改按值捕获的副本，需要 `mutable`。 |
| 范围 `for`                         | range-based for                                           | [src/loguru.cpp:614](src/loguru.cpp#L614)、[994](src/loguru.cpp#L994)、[1246](src/loguru.cpp#L1246) `auto&&`、[1555](src/loguru.cpp#L1555)、[1656](src/loguru.cpp#L1656)、[1893](src/loguru.cpp#L1893) | `for (声明 : 范围)` 用 `begin`/`end` 遍历。`auto&&` 既绑左值也绑右值，避免拷贝元素。循环里不要让迭代器失效（例如中途 `push_back` 导致扩容）。`auto` 会拷贝元素。范围是临时对象时，它的生命期延长到整个循环。 |
| 原始字符串 `R"(...)"`              | raw string literal                                        | [src/loguru.cpp:1273](src/loguru.cpp#L1273)、[1280](src/loguru.cpp#L1280) | `R"(内容)"` 里反斜杠和引号都按字面保留，不做转义。适合正则和 Windows 路径。内容里不能出现 `)"`；要出现时改用自定义分隔符，例如 `R"del(内容)del"`。换行会原样进入字符串。 |
| 列表初始化 `std::initializer_list` | initializer list                                          | [src/loguru.cpp:1226](src/loguru.cpp#L1226)                          | 花括号列表可以生成 `std::initializer_list<T>`，它只是指向临时数组的轻量视图，元素是 `const`。构造函数参数写成它，就能接受 `{a, b, c}`。不要把这个列表存下来以后再用，底层数组是临时的。拷贝列表只拷贝两个指针，不拷贝元素。 |

移动和删除特殊成员集中在 `Text`：

```305:318:src/loguru.hpp
		Text(Text&& t)   //- 定义移动构造函数
		{
			_str = t._str;
			t._str = nullptr;
			//^ nullptr 是 C++11 的空指针常量，类型是 std::nullptr_t。
			//^ 它可以隐式转换成任何对象指针、函数指针或成员指针，转换结果是对应类型的空指针。
			//^ s_fatal_handler = nullptr 表示这个函数指针当前不指向任何函数。
			//^ 用指针之前要判断它是不是 nullptr，对空指针解引用是未定义行为。
			//^ 它不是整数。NULL 常被定义成 0，重载时可能匹配到 int 参数；nullptr 只会匹配指针参数。
			//^ 和指针比较、赋值都可以：p == nullptr、p = nullptr。
		}
		Text(Text& t) = delete; //- 禁止拷贝构造函数
		Text& operator=(Text& t) = delete; //- 禁止拷贝赋值运算符
		void operator=(Text&& t) = delete; //- 禁止移动赋值运算符
```

## C++98 仍在大量使用的机制

| 特性                                  | English                                                | 位置                                                         | 讲解                                                         |
| ------------------------------------- | ------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 命名空间、匿名命名空间                | namespace, anonymous namespace                         | [src/loguru.hpp:215](src/loguru.hpp#L215)–[219](src/loguru.hpp#L219)、[289](src/loguru.hpp#L289)、[294](src/loguru.hpp#L294) | `namespace 名 { }` 把声明放进独立作用域，用 `名::` 访问；同名命名空间可以跨文件重新打开、合并。`namespace { }` 是匿名命名空间，里面的名字具有内部链接，只在本翻译单元可见。不要在头文件里放匿名命名空间的定义，每个包含它的翻译单元都会有一份。 |
| `class` / `struct`、访问控制          | class, struct, access control                          | [Text](src/loguru.hpp#L297)，[Message](src/loguru.hpp#L457)，[LogScopeRAII](src/loguru.hpp#L783)，[EcEntryBase](src/loguru.hpp#L994) | `class` 默认 `private`，`struct` 默认 `public`，除此之外语法相同。`public` 对外，`protected` 给派生类，`private` 只留在类内。这是编译期检查，不是运行时防护。`struct` 一样可以有构造函数和不变量。 |
| 继承、虚析构、纯虚函数                | inheritance, virtual destructor, pure virtual function | [src/loguru.hpp:998](src/loguru.hpp#L998)、[1004](src/loguru.hpp#L1004)、[1031](src/loguru.hpp#L1031) `EcEntryData : public EcEntryBase` | `struct 派生 : public 基类` 表示公有继承。基类析构标 `virtual`，才能用基类指针 `delete` 到派生对象。`虚函数 = 0` 是纯虚函数，含有它的类不能实例化。用非虚析构的基类指针删除派生对象是未定义行为。构造和析构期间不要调用会派发到派生类的虚函数。 |
| `explicit`、构造初始化列表            | explicit constructor, member initializer list          | [src/loguru.hpp:300](src/loguru.hpp#L300)、[1037](src/loguru.hpp#L1037)、[1449](src/loguru.hpp#L1449) | `explicit` 禁止单参数构造被用来做隐式转换，`T x = 值` 会失败，`T x(值)` 仍可以。成员初始化列表写在构造函数体之前：`T() : a(1), b(2) {}`。`const` 成员和引用成员只能在这里初始化。顺序按成员声明顺序，不按列表书写顺序。 |
| RAII                                  | RAII                                                   | [Text](src/loguru.hpp#L297)、[LogScopeRAII](src/loguru.hpp#L783)、[EcEntryBase](src/loguru.hpp#L994)、[StreamLogger](src/loguru.hpp#L1446)；[src/loguru.cpp:1002](src/loguru.cpp#L1002) `lock_guard` | 构造时取得资源，析构时释放。局部对象离开作用域（包括因异常离开）都会析构，所以加锁、缓冲、日志作用域可以自动配对。资源要由对象自己拥有。析构函数不要抛异常。只有 `new` 而没有放进析构里的 `delete`，就不是 RAII。 |
| `const` 成员函数                      | const member function                                  | [src/loguru.hpp:335](src/loguru.hpp#L335)、[348](src/loguru.hpp#L348)、[1004](src/loguru.hpp#L1004) | 成员函数末尾的 `const` 表示不修改非 `mutable` 成员，`this` 的类型是 `const T*`。只有 `const` 对象才能调用它；非 `const` 对象两种都能调用。`mutable` 成员在 `const` 函数里仍可改。漏写 `const` 会使本可对常量对象调用的函数无法调用。 |
| 函数重载                              | function overloading                                   | [src/loguru.hpp:402](src/loguru.hpp#L402) 与 [407](src/loguru.hpp#L407) 的 `textprintf`；[1461](src/loguru.hpp#L1461) 与 [1468](src/loguru.hpp#L1468) 的 `operator<<` | 同一作用域里同名函数靠参数个数和类型区分，返回类型不参与。调用时编译器做重载决议，选出最匹配的一个。默认参数和隐式转换会让决议偏离直觉。`= delete` 的候选若被选中，直接编译失败。不能只靠返回类型区分重载。 |
| 运算符重载                            | operator overloading                                   | [src/loguru.hpp:1461](src/loguru.hpp#L1461) `operator<<`，返回 `*this` 形成链式调用 | `operator符号` 可以是成员函数，也可以是非成员函数。成员形式的左操作数必须是类对象本身。返回 `*this` 的引用才能写成 `a << b << c`。至少要有一个操作数是类类型或枚举。优先级和结合性改不了，只改含义。 |
| 函数指针 `typedef`                    | pointer to function                                    | [src/loguru.hpp:496](src/loguru.hpp#L496)–[501](src/loguru.hpp#L501)     | `typedef 返回类型 (*名字)(参数列表);` 给函数指针类型起名。调用前要确认不是空指针。函数类型作参数时会自动调整成指针，写成指针 typedef 更明确。成员函数指针是 `返回类型 (类::*)(参数)`，和普通函数指针不是同一类型。 |
| 默认参数                              | default argument                                       | [src/loguru.hpp:612](src/loguru.hpp#L612)、[701](src/loguru.hpp#L701)–[702](src/loguru.hpp#L702) | 声明里从右往左给参数默认值，调用时可以省略这些实参。默认实参在调用点求值，不是在函数定义点。只写在声明里一次（通常在头文件），定义里不要再写。它不是重载，省略实参和显式传入走的是同一个函数。 |
| `static` 成员函数、`static` 局部/全局 | static member function, static storage duration        | [src/loguru.hpp:548](src/loguru.hpp#L548) `SignalOptions::none()`；[src/loguru.cpp:210](src/loguru.cpp#L210)–[230](src/loguru.cpp#L230) | 静态成员函数没有 `this`，只能访问静态成员，用 `类::函数()` 调用。函数内 `static` 局部变量程序期内只初始化一次，C++11 起这次初始化是线程安全的。文件作用域的 `static` 表示内部链接。这三种 `static` 含义不同。静态局部对象在 `main` 返回后析构。 |
| `extern` 全局                         | external linkage                                       | [src/loguru.hpp:475](src/loguru.hpp#L475) 起一组 `g_*`，定义在 [src/loguru.cpp:193](src/loguru.cpp#L193) | `extern 类型 名;` 是声明不是定义，表示对象在别的翻译单元定义，全程序只有一份。头文件里不要漏掉 `extern`，否则每个包含它的翻译单元都有一份定义，链接失败。不同翻译单元里的全局对象，初始化顺序不确定。 |
| `inline`                              | inline function                                        | [src/loguru.hpp:874](src/loguru.hpp#L874)；[src/loguru.cpp:324](src/loguru.cpp#L324) | `inline` 允许同一函数在多个翻译单元各有一份相同定义，链接时合并。类体内定义的成员函数隐式 `inline`。它不保证编译器真的把调用展开进调用点。每一份定义的标记必须一致。 |
| 前向声明                              | forward declaration                                    | [src/loguru.hpp:988](src/loguru.hpp#L988) `struct StringStream`，定义在 [src/loguru.cpp:1829](src/loguru.cpp#L1829) | `struct 名;` 只引入名字，之后可以用这个名字的指针和引用。在看到完整定义之前，不能创建对象、访问成员、求 `sizeof`，也不能继承它。用来减少头文件互相包含。按值成员和基类都需要完整类型。 |
| 引用参数                              | reference parameter                                    | 贯穿 [const Message&](src/loguru.hpp#L496)、`const T&`           | `T&` 绑定已有对象并可修改它，`const T&` 只读且能绑定临时对象。引用没有空值，必须绑定到对象。不要返回局部变量的引用。`const T&` 参数会把临时实参的生命期延长到这次函数调用结束。 |
| `static_cast` / `reinterpret_cast`    | explicit type conversion                               | [src/loguru.hpp:1149](src/loguru.hpp#L1149)；[src/loguru.cpp:325](src/loguru.cpp#L325)、[369](src/loguru.cpp#L369)、[499](src/loguru.cpp#L499)、[532](src/loguru.cpp#L532) | `static_cast<T>(e)` 做相关类型之间的转换：数值、`void*` 转回原来的对象指针、沿继承向下转（程序员保证类型正确）。`reinterpret_cast` 按位重新解释，几乎不检查。能用 `static_cast` 就不要用 `reinterpret_cast`。违反严格别名规则的重新解释是未定义行为。 |
| `new` / `delete`                      | new-expression, delete-expression                      | [src/loguru.cpp:369](src/loguru.cpp#L369)、[751](src/loguru.cpp#L751)、[893](src/loguru.cpp#L893)、[1572](src/loguru.cpp#L1572)、[1867](src/loguru.cpp#L1867) | `new T` 分配内存并调用构造函数，`delete p` 先析构再释放。`new T[n]` 必须配 `delete[]`。`delete` 空指针是空操作。同一指针 `delete` 两次是未定义行为。异常路径上也要释放，所以更适合包进析构函数。 |
| `try` / `catch`，含 `catch (...)`     | exception handling                                     | [src/loguru.cpp:529](src/loguru.cpp#L529)、[531](src/loguru.cpp#L531) `catch (...)`、[1272](src/loguru.cpp#L1272)、[2114](src/loguru.cpp#L2114) `catch (...)` | `try` 里抛出异常后，已构造的自动对象按相反顺序析构，然后匹配 `catch`。`catch (...)` 接住任何类型。应按 `const T&` 捕获，按值捕获会发生切片。`catch (...)` 里若要继续抛出，写 `throw;`，不要写 `throw e;`。析构函数里不要让异常逃出去。 |
| `typeid`（RTTI）                      | runtime type identification                            | [src/loguru.cpp:1219](src/loguru.cpp#L1219)；头文件 [190](src/loguru.hpp#L190)–[211](src/loguru.hpp#L211) 用编译器宏决定是否开启 | `typeid(表达式)` 得到 `std::type_info`。操作数是多态类型的左值时会求值，给出动态类型；否则不求值，给出静态类型。需要编译器打开 RTTI。对空的多态指针做 `typeid(*p)` 是未定义行为。它只能识别类型，不能代替 `dynamic_cast` 做向下转换。 |
| C 风格变参 `...`、`va_list`           | variadic function, `va_list`                           | [src/loguru.hpp:402](src/loguru.hpp#L402)、[771](src/loguru.hpp#L771)、[775](src/loguru.hpp#L775)；[src/loguru.cpp:474](src/loguru.cpp#L474)、[1630](src/loguru.cpp#L1630)、[1637](src/loguru.cpp#L1637) | `void f(固定参数, ...)` 的剩余实参没有类型。用 `va_start`、`va_arg`、`va_end` 按约定的类型逐个取出，`va_end` 不能省。编译器不检查个数和类型（除非另加 format 属性）。传非平凡类类型是未定义行为。C++ 里更安全的是重载或变参模板。 |
| 三元运算符当控制流                    | conditional operator                                   | [src/loguru.hpp:1264](src/loguru.hpp#L1264) `VLOG_F`                 | `条件 ? 表达式1 : 表达式2` 是表达式，不是语句。只求值被选中的那一边，但两边都要能转换成同一个公共类型。这个转换可能出乎意料（例如整型提升）。它有值，可以放进宏或初始化式；两边类型差太多时编译失败。 |

模板元编程用来把 `ERROR_CONTEXT` 的实参收成指针或按值保存：

```1081:1096:src/loguru.hpp
	template <class T>
	struct decay_char_array { using type = T; };

	template <unsigned long long  N>
	struct decay_char_array<const char(&)[N]> { using type = const char*; };
	// ...
	struct make_ec_type { using type = typename make_const_ptr<typename decay_char_array<T>::type>::type; };
```

## 预处理

宏是接口的主体，不是边角。

| 特性                                                         | English                                    | 位置                                                         | 讲解                                                         |
| ------------------------------------------------------------ | ------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| `#if` / `#ifdef` / `#define` / `#undef` / `#error` / `#pragma` | preprocessing directives                   | [src/loguru.hpp:83](src/loguru.hpp#L83)–[94](src/loguru.hpp#L94)、[140](src/loguru.hpp#L140) | 预处理在编译前改源码文本。`#define` 定义宏，`#undef` 取消，`#if` / `#ifdef` 按条件留下或删掉一段，`#error` 让编译停止，`#pragma` 是给编译器的指令。宏没有类型，也不遵守 C++ 作用域。条件为假的分支不做语法分析，但里面的文本仍要能切成预处理记号。 |
| `#` 字符串化、`##` 拼接、`__VA_ARGS__`                       | stringizing, token pasting, variadic macro | [src/loguru.hpp:225](src/loguru.hpp#L225)–[226](src/loguru.hpp#L226)、[275](src/loguru.hpp#L275)、[1268](src/loguru.hpp#L1268) `Verbosity_ ## verbosity_name` | 宏参数前的 `#` 把该参数变成字符串字面量。`##` 把两侧记号拼成一个记号，结果必须仍是合法记号。`__VA_ARGS__` 代表可变宏里省略号对应的全部实参。C++11 在实参为空时，不会自动吃掉 `__VA_ARGS__` 前面的逗号；`##__VA_ARGS__` 是 GNU 扩展。 |
| `__FILE__`、`__LINE__`、`__func__`、`__COUNTER__`            | predefined macro                           | [src/loguru.hpp:228](src/loguru.hpp#L228)、[1148](src/loguru.hpp#L1148)、[1265](src/loguru.hpp#L1265)、[1294](src/loguru.hpp#L1294) | `__FILE__` 换成当前源文件名，`__LINE__` 换成当前行号；写在宏里时，行号是宏的调用点。`__func__` 不是宏，是编译器为每个函数准备的静态字符数组，内容是函数名。`__COUNTER__` 不是标准，每展开一次加一，常用来拼出不重复的局部名字。 |
| 宏展开成局部 RAII 对象                                       | macro expanding to a scoped RAII object    | [src/loguru.hpp:1278](src/loguru.hpp#L1278) `VLOG_SCOPE_F`，[1291](src/loguru.hpp#L1291) `LOG_SCOPE_F`，[1145](src/loguru.hpp#L1145) `ERROR_CONTEXT` | 宏展开结果是一条局部变量定义。对象在这一行构造，离开所在块时析构，从而成对地开始和结束一段工作。变量名不能和周围代码冲突，常用 `__LINE__` 或 `__COUNTER__` 拼接。宏本身不构成作用域，展开结果必须是一条合法声明或语句。 |

## 标准库

| 组件                                      | English                                | 位置                                              | 讲解                                                         |
| ----------------------------------------- | -------------------------------------- | ------------------------------------------------- | ------------------------------------------------------------ |
| `<chrono>`、`std::thread`                 | chrono library, thread support library | [src/loguru.cpp:161](src/loguru.cpp#L161)、[224](src/loguru.cpp#L224)、[1572](src/loguru.cpp#L1572) | `std::chrono::duration` 表示时长，`time_point` 表示时间点，`sleep_for` 让当前线程睡眠。`std::thread` 的构造函数接受可调用对象，并开始新线程。线程对象析构前必须 `join` 或 `detach`，否则调用 `std::terminate`。线程函数里的异常不能传到外面。 |
| `std::atomic`                             | atomic operations library              | [src/loguru.cpp:221](src/loguru.cpp#L221)、[226](src/loguru.cpp#L226) | `std::atomic<T>` 上的读、写和读改写不会被别的线程从中间打断。`++a` 是一次原子操作，`a = a + 1` 不是。不是所有 `T` 都无锁。原子变量只保护自己，旁边的普通变量仍要另加同步。 |
| `std::recursive_mutex`、`std::lock_guard` | mutual exclusion, RAII lock            | [src/loguru.cpp:210](src/loguru.cpp#L210)、[1002](src/loguru.cpp#L1002) | `std::mutex` 同一线程不能重复加锁，否则是未定义行为。`recursive_mutex` 允许同一线程多次加锁，解锁次数必须相同。`lock_guard` 在构造时加锁、析构时解锁，不能提前解锁，也不能转移。离开作用域（含异常）就会解锁。 |
| `std::string`、`std::vector`、`std::pair` | string, sequence container, pair       | [src/loguru.cpp:187](src/loguru.cpp#L187)–[189](src/loguru.cpp#L189) | `string` 拥有自己的字符缓冲，`vector` 是动态数组，`pair<T,U>` 把两个值放在一起。`push_back` 扩容后，原先的迭代器、指针和引用失效。C++11 起它们可以移动：移动构造偷走缓冲，源对象变成空的有效对象。 |
| `std::regex`、`std::regex_error`          | regular expressions library            | [src/loguru.cpp:1273](src/loguru.cpp#L1273)–[1283](src/loguru.cpp#L1283) | `std::regex` 在构造时编译正则，语法非法抛 `std::regex_error`。默认语法是 ECMAScript。普通字符串里的反斜杠要写成 `\\`，原始字符串不用。构造开销大，同一模式不要在热路径上反复构造。 |
| `std::ostringstream`、`operator<<` 操纵符 | string stream, stream manipulator      | [src/loguru.hpp:1461](src/loguru.hpp#L1461)、[1478](src/loguru.hpp#L1478)，仅 `LOGURU_WITH_STREAMS` | `ostringstream` 把 `operator<<` 写入的内容收成 `string`，用 `.str()` 取出。`std::setw`、`std::setfill` 这类操纵符改变后续输出的格式。`setw` 只作用于下一次插入。流可能进入失败状态，之后的插入会被忽略。 |
| `std::hash<std::thread::id>`              | hash function object                   | [src/loguru.cpp:1192](src/loguru.cpp#L1192)               | `std::hash<T>` 是函数对象，`operator()` 把 `T` 映成 `size_t`。标准库为 `std::thread::id` 提供了特化。哈希允许碰撞，不相等的键可能得到同一个值。不要为标准库类型再写 `std::hash` 特化；哈希值也不保证跨进程、跨标准库实现相同。 |

## 编译器扩展

这些不是标准 C++。

| 扩展                                                   | English                            | 位置                  | 讲解                                                         |
| ------------------------------------------------------ | ---------------------------------- | --------------------- | ------------------------------------------------------------ |
| `__attribute__((format))`、`__attribute__((noreturn))` | GNU function attribute             | [src/loguru.hpp:239](src/loguru.hpp#L239)、[262](src/loguru.hpp#L262) | GCC/Clang 写在函数声明后。`format(printf, 格式串下标, 首个变参下标)` 让编译器按 printf 规则检查实参，下标从 1 开始，成员函数要算上隐式的 `this`。`noreturn` 表示函数不会返回。标了 `noreturn` 却返回了，是未定义行为。MSVC 不认识这套属性。 |
| `__declspec(noreturn)`、SAL 注解 `_In_z_`              | Microsoft declspec, SAL annotation | [src/loguru.hpp:252](src/loguru.hpp#L252)、[260](src/loguru.hpp#L260) | `__declspec(noreturn)` 是 MSVC 的“函数不返回”标记，对应 GCC 的 `noreturn`。`_In_z_` 是 SAL 注解，告诉静态分析器该参数是以 NUL 结尾的输入字符串，不改变生成的代码。写错只影响分析结果。GCC/Clang 默认不认这些记号，要用宏包起来。 |
| `__builtin_expect`                                     | GCC branch prediction builtin      | [src/loguru.hpp:269](src/loguru.hpp#L269)–[270](src/loguru.hpp#L270) | `__builtin_expect(表达式, 期望值)` 告诉编译器这个表达式多半等于期望值，用来安排分支代码的布局。它不改变表达式的值。只是提示，CPU 和基于剖析的优化常常更准。包进宏时要注意参数被求值一次还是两次。 |
| `#pragma clang/GCC system_header`                      | system header pragma               | [src/loguru.hpp:88](src/loguru.hpp#L88)、[94](src/loguru.hpp#L94) | 放在头文件里，让编译器把该头文件之后的内容当成系统头，从而压制其中的警告。Clang 和 GCC 的 pragma 名字不同，通常分开放在各自的条件编译里。它会把真的问题也藏掉，只适合第三方头，不适合自己的接口。 |
| `__has_feature(cxx_rtti)`、`__GXX_RTTI`、`_CPPRTTI`    | RTTI feature-test macro            | [src/loguru.hpp:199](src/loguru.hpp#L199)–[211](src/loguru.hpp#L211) | 这三个都不是标准宏，用来判断这次编译有没有打开 RTTI。Clang 用 `__has_feature(cxx_rtti)`，GCC 看 `__GXX_RTTI`，MSVC 看 `_CPPRTTI`。关掉 RTTI 后不能用 `typeid` 和 `dynamic_cast`，虚函数本身仍可用。用之前要确认宏存在，避免未定义标识符。 |
| `abi::__cxa_demangle`                                  | Itanium ABI name demangling        | [src/loguru.cpp:1211](src/loguru.cpp#L1211) | GCC/Clang 在 Itanium ABI 下用它把 `typeid` 给出的改编名还原成接近源码的函数或类型名。返回的缓冲区要用 `free` 释放，失败时返回空指针。MSVC 没有这个函数，名字改编格式也不同。还原结果的文本格式依赖编译器，不宜当成稳定接口解析。 |
