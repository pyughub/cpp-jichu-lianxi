/*
Loguru logging library for C++, by Emil Ernerfeldt.
www.github.com/emilk/loguru
If you find Loguru useful, please let me know on twitter or in a mail!
Twitter: @ernerfeldt
Mail:    emil.ernerfeldt@gmail.com
Website: www.ilikebigbits.com

# License
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to
	copy, modify and distribute it as you see fit.

# Inspiration
	Much of Loguru was inspired by GLOG, https://code.google.com/p/google-glog/.
	The choice of public domain is fully due Sean T. Barrett
	and his wonderful stb libraries at https://github.com/nothings/stb.

# Version history
	* Version 0.1.0 - 2015-03-22 - Works great on Mac.
	* Version 0.2.0 - 2015-09-17 - Removed the only dependency.
	* Version 0.3.0 - 2015-10-02 - Drop-in replacement for most of GLOG
	* Version 0.4.0 - 2015-10-07 - Single-file!
	* Version 0.5.0 - 2015-10-17 - Improved file logging
	* Version 0.6.0 - 2015-10-24 - Add stack traces
	* Version 0.7.0 - 2015-10-27 - Signals
	* Version 0.8.0 - 2015-10-30 - Color logging.
	* Version 0.9.0 - 2015-11-26 - ABORT_S and proper handling of FATAL
	* Version 1.0.0 - 2016-02-14 - ERROR_CONTEXT
	* Version 1.1.0 - 2016-02-19 - -v OFF, -v INFO etc
	* Version 1.1.1 - 2016-02-20 - textprintf vs strprintf
	* Version 1.1.2 - 2016-02-22 - Remove g_alsologtostderr
	* Version 1.1.3 - 2016-02-29 - ERROR_CONTEXT as linked list
	* Version 1.2.0 - 2016-03-19 - Add get_thread_name()
	* Version 1.2.1 - 2016-03-20 - Minor fixes
	* Version 1.2.2 - 2016-03-29 - Fix issues with set_fatal_handler throwing an exception
	* Version 1.2.3 - 2016-05-16 - Log current working directory in loguru::init().
	* Version 1.2.4 - 2016-05-18 - Custom replacement for -v in loguru::init() by bjoernpollex
	* Version 1.2.5 - 2016-05-18 - Add ability to print ERROR_CONTEXT of parent thread.
	* Version 1.2.6 - 2016-05-19 - Bug fix regarding VLOG verbosity argument lacking ().
	* Version 1.2.7 - 2016-05-23 - Fix PATH_MAX problem.
	* Version 1.2.8 - 2016-05-26 - Add shutdown() and remove_all_callbacks()
	* Version 1.2.9 - 2016-06-09 - Use a monotonic clock for uptime.
	* Version 1.3.0 - 2016-07-20 - Fix issues with callback flush/close not being called.
	* Version 1.3.1 - 2016-07-20 - Add LOGURU_UNSAFE_SIGNAL_HANDLER to toggle stacktrace on signals.
	* Version 1.3.2 - 2016-07-20 - Add loguru::arguments()
	* Version 1.4.0 - 2016-09-15 - Semantic versioning + add loguru::create_directories
	* Version 1.4.1 - 2016-09-29 - Customize formating with LOGURU_FILENAME_WIDTH
	* Version 1.5.0 - 2016-12-22 - LOGURU_USE_FMTLIB by kolis and LOGURU_WITH_FILEABS by scinart
	* Version 1.5.1 - 2017-08-08 - Terminal colors on Windows 10 thanks to looki
	* Version 1.6.0 - 2018-01-03 - Add LOGURU_RTTI and LOGURU_STACKTRACES settings
	* Version 1.7.0 - 2018-01-03 - Add ability to turn off the preamble with loguru::g_preamble
	* Version 1.7.1 - 2018-04-05 - Add function get_fatal_handler
	* Version 1.7.2 - 2018-04-22 - Fix a bug where large file names could cause stack corruption (thanks @ccamporesi)
	* Version 1.8.0 - 2018-04-23 - Shorten long file names to keep preamble fixed width
	* Version 1.9.0 - 2018-09-22 - Adjust terminal colors, add LOGURU_VERBOSE_SCOPE_ENDINGS, add LOGURU_SCOPE_TIME_PRECISION, add named log levels
	* Version 2.0.0 - 2018-09-22 - Split loguru.hpp into loguru.hpp and loguru.cpp
	* Version 2.1.0 - 2019-09-23 - Update fmtlib + add option to loguru::init to NOT set main thread name.
	* Version 2.2.0 - 2026-09-10 - Replace LOGURU_CATCH_SIGABRT with struct SignalOptions, add CMake support, syslog support, va_list API, LOGURU_USE_LOCALE, DLOG_SCOPE_F, and many platform fixes

# Compiling
	Just include <loguru.hpp> where you want to use Loguru.
	Then, in one .cpp file #include <loguru.cpp>
	Make sure you compile with -std=c++11 -lstdc++ -lpthread -ldl

# Usage
	For details, please see the official documentation at emilk.github.io/loguru

	#include <loguru.hpp>

	int main(int argc, char* argv[]) {
		loguru::init(argc, argv);

		// Put every log message in "everything.log":
		loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

		LOG_F(INFO, "The magic number is %d", 42);
	}

*/

#if defined(LOGURU_IMPLEMENTATION)
	#error "You are defining LOGURU_IMPLEMENTATION. This is for older versions of Loguru. You should now instead include loguru.cpp (or build it and link with it)"
#endif

// Disable all warnings from gcc/clang:
#if defined(__clang__)
	#pragma clang system_header
	//^ 让 Clang 把本文件此后的内容当成系统头文件处理，从而抑制这里产生的 warning。
	//^ 它只在 __clang__ 下生效。下一行 #pragma GCC system_header 是 GCC 的对应写法。
	//^ 效果接近用 -isystem 包含头文件：库自身的风格、未使用变量等 warning 不会冒到你的编译输出里。
	//^ error 仍然报，加 -Wsystem-headers 时 warning 也会重新出现。
#elif defined(__GNUC__)
	#pragma GCC system_header
#endif

#ifndef LOGURU_HAS_DECLARED_FORMAT_HEADER
#define LOGURU_HAS_DECLARED_FORMAT_HEADER

// Semantic versioning. Loguru version can be printed with printf("%d.%d.%d", LOGURU_VERSION_MAJOR, LOGURU_VERSION_MINOR, LOGURU_VERSION_PATCH);
#define LOGURU_VERSION_MAJOR 2
#define LOGURU_VERSION_MINOR 2
#define LOGURU_VERSION_PATCH 0

#if defined(_MSC_VER)
#include <sal.h>	// Needed for _In_z_ etc annotations
#endif

#if defined(__linux__) || defined(__APPLE__)
#define LOGURU_SYSLOG 1
#else
#define LOGURU_SYSLOG 0
#endif

// ----------------------------------------------------------------------------

#ifndef LOGURU_EXPORT
	// Define to your project's export declaration if needed for use in a shared library.
	#define LOGURU_EXPORT
#endif

#ifndef LOGURU_SCOPE_TEXT_SIZE
	// Maximum length of text that can be printed by a LOG_SCOPE.
	// This should be long enough to get most things, but short enough not to clutter the stack.
	#define LOGURU_SCOPE_TEXT_SIZE 196
#endif

#ifndef LOGURU_FILENAME_WIDTH
	// Width of the column containing the file name
	#define LOGURU_FILENAME_WIDTH 23
#endif

#ifndef LOGURU_THREADNAME_WIDTH
	// Width of the column containing the thread name
	#define LOGURU_THREADNAME_WIDTH 16
#endif

#ifndef LOGURU_SCOPE_TIME_PRECISION
	// Resolution of scope timers. 3=ms, 6=us, 9=ns
	#define LOGURU_SCOPE_TIME_PRECISION 3
#endif

#ifdef LOGURU_CATCH_SIGABRT
	#error "You are defining LOGURU_CATCH_SIGABRT. This is for older versions of Loguru. You should now instead set the options passed to loguru::init"
#endif

#ifndef LOGURU_VERBOSE_SCOPE_ENDINGS
	// Show milliseconds and scope name at end of scope.
	#define LOGURU_VERBOSE_SCOPE_ENDINGS 1
#endif

#ifndef LOGURU_REDEFINE_ASSERT
	#define LOGURU_REDEFINE_ASSERT 0
#endif

#ifndef LOGURU_WITH_STREAMS
	#define LOGURU_WITH_STREAMS 0
#endif

#ifndef LOGURU_REPLACE_GLOG
	#define LOGURU_REPLACE_GLOG 0
#endif

#if LOGURU_REPLACE_GLOG
	#undef LOGURU_WITH_STREAMS
	#define LOGURU_WITH_STREAMS 1
#endif

#if defined(LOGURU_UNSAFE_SIGNAL_HANDLER)
	#error "You are defining LOGURU_UNSAFE_SIGNAL_HANDLER. This is for older versions of Loguru. You should now instead set the unsafe_signal_handler option when you call loguru::init."
#endif

#if LOGURU_IMPLEMENTATION
	#undef LOGURU_WITH_STREAMS
	#define LOGURU_WITH_STREAMS 1
#endif

#ifndef LOGURU_USE_FMTLIB
	#define LOGURU_USE_FMTLIB 0
#endif

#ifndef LOGURU_USE_LOCALE
        #define LOGURU_USE_LOCALE 0
#endif

#ifndef LOGURU_WITH_FILEABS
	#define LOGURU_WITH_FILEABS 0
#endif

#ifndef LOGURU_RTTI
//^ RTTI（Run-Time Type Information）是 C++ 的运行时类型信息。开着它才能用 typeid 和 dynamic_cast 在运行时查对象的真实类型。编译加 -fno-rtti（MSVC 是 /GR-）就会关掉。
//^ 这段在你没手动定义 LOGURU_RTTI 时，按编译器探测 RTTI 是否开启：
//^ 编译器	探测宏
//^ Clang __has_feature(cxx_rtti)
//^ GCC __GXX_RTTI
//^ MSVC _CPPRTTI
//^ 探测到就 #define LOGURU_RTTI 1。库里用它给 stack trace 做名字替换：typeid(std::string).name() 
//^ 经 demangle 后，把冗长的类型名收成 std::string。RTTI 关掉时这段不编译，避免调用 typeid。
#if defined(__clang__)
	#if __has_feature(cxx_rtti)
		#define LOGURU_RTTI 1
	#endif
#elif defined(__GNUG__)
	#if defined(__GXX_RTTI)
		#define LOGURU_RTTI 1
	#endif
#elif defined(_MSC_VER)
	#if defined(_CPPRTTI)
		#define LOGURU_RTTI 1
	#endif
#endif
#endif

#ifdef LOGURU_USE_ANONYMOUS_NAMESPACE
	#define LOGURU_ANONYMOUS_NAMESPACE_BEGIN namespace {
	#define LOGURU_ANONYMOUS_NAMESPACE_END }
#else
	#define LOGURU_ANONYMOUS_NAMESPACE_BEGIN
	#define LOGURU_ANONYMOUS_NAMESPACE_END
#endif

// --------------------------------------------------------------------
// Utility macros

#define LOGURU_CONCATENATE_IMPL(s1, s2) s1 ## s2
#define LOGURU_CONCATENATE(s1, s2) LOGURU_CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#   define LOGURU_ANONYMOUS_VARIABLE(str) LOGURU_CONCATENATE(str, __COUNTER__)
//^ __COUNTER__ 不在 C 和 C++ 标准的预定义宏里。GCC、Clang、MSVC 都提供它：每展开一次就得到一个整数，从 0 开始逐次加 1。
//^ loguru 因此先判断它是否存在，没有就改用标准宏 __LINE__：
#else
#   define LOGURU_ANONYMOUS_VARIABLE(str) LOGURU_CONCATENATE(str, __LINE__)
#endif

#if defined(__clang__) || defined(__GNUC__)
	// Helper macro for declaring functions as having similar signature to printf.
	// This allows the compiler to catch format errors at compile-time.
	#define LOGURU_PRINTF_LIKE(fmtarg, firstvararg) __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
	//^ 这是 GCC/Clang 的函数属性，用来声明「这个函数的参数长得像 printf」，让编译器在编译期检查格式串。
	//^ __attribute__((__format__(__printf__, fmtarg, firstvararg)))
	//^ 片段	含义
	//^ __attribute__((...)) GCC/Clang 扩展，给声明附加元数据
	//^ __format__ 属性名：按格式串函数来检查
	//^ __printf__ 格式规则跟 printf 一样（%d、%s 等）
	//^ fmtarg 格式串是第几个参数，从 1 起算
	//^ firstvararg 格式串消费的第一个实参是第几个；0 表示参数走 va_list，不逐个检查
	//^ 宏 LOGURU_PRINTF_LIKE(fmtarg, firstvararg) 只是把上面这串包起来。调用处写在函数声明末尾
	#define LOGURU_FORMAT_STRING_TYPE const char*
#elif defined(_MSC_VER)
	#define LOGURU_PRINTF_LIKE(fmtarg, firstvararg)
	#define LOGURU_FORMAT_STRING_TYPE _In_z_ _Printf_format_string_ const char*
#else
	#define LOGURU_PRINTF_LIKE(fmtarg, firstvararg)
	#define LOGURU_FORMAT_STRING_TYPE const char*
#endif

// Used to mark log_and_abort for the benefit of the static analyzer and optimizer.
#if defined(_MSC_VER)
#define LOGURU_NORETURN __declspec(noreturn)
#else
#define LOGURU_NORETURN __attribute__((noreturn))
#endif

#if defined(_MSC_VER)
#define LOGURU_PREDICT_FALSE(x) (x)
#define LOGURU_PREDICT_TRUE(x)  (x)
#else
#define LOGURU_PREDICT_FALSE(x) (__builtin_expect(x,     0))
#define LOGURU_PREDICT_TRUE(x)  (__builtin_expect(!!(x), 1))
#endif

#if LOGURU_USE_FMTLIB
	#include <fmt/format.h>
	#define LOGURU_FMT(x) "{:" #x "}"
#else
	#define LOGURU_FMT(x) "%" #x
#endif

#ifdef _WIN32
	#define STRDUP(str) _strdup(str)
#else
	#define STRDUP(str) strdup(str)
#endif

#include <stdarg.h>

// --------------------------------------------------------------------
LOGURU_ANONYMOUS_NAMESPACE_BEGIN

//v namespace 给名字划分作用域，避免不同库里的同名类型、函数撞在一起。
//v 花括号里的 Text、add_callback 全名是 loguru::Text、loguru::add_callback。:: 是作用域解析运算符。同一个命名空间可以在多个文件里再次打开，后写的声明仍然属于 loguru。
//v 调用处要么写全名 loguru::Text，要么先 using namespace loguru;，之后可以直接写 Text。头文件里通常不写后者，以免把名字灌进包含它的所有代码。
namespace loguru
{
	// Simple RAII ownership of a char*.
	class LOGURU_EXPORT Text
	{
	public:
		explicit Text(char* owned_str) : _str(owned_str) {} 
		//^ 禁止 char* 隐式转为 Text；构造函数体执行前，用 owned_str 初始化私有成员 _str
		~Text();  //- 声明析构函数   
		//^ 这就是 RAII：资源获取即初始化。获取放在构造函数里，释放放在析构函数里，不靠调用方记得手动 free。

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
		/*  禁止拷贝构造函数和拷贝赋值运算符的用法：
		char* p = strdup("hello");
		char* q = strdup("world");
		Text src(p);              // 对应显式构造
		Text t1(std::move(src));  // 移动构造 Text(Text&&)

		Text a(q);
		Text b(a);                // 拷贝构造 Text(Text&)，被 delete
		Text c = a;               // 同样是拷贝构造, 被 delete

		Text d(strdup("x"));
		Text e(strdup("y"));
		e = d;                    // 拷贝赋值 operator=(Text&)，被 delete
		e = std::move(d);         // 移动赋值 operator=(Text&&)，被 delete
		*/

		const char* c_str() const { return _str; }  
		//^ const char*：返回的是指向常量字符的指针。调用方不能通过这个指针改字符串内容。指针本身按值返回，调用方可以改自己手里的那份指针。 
		//^ c_str() const：这是 const 成员函数。const Text 对象也能调用它；函数内不能给 _str 重新赋值。
		//^  -------------------------- 关于 c_str() 的解释 --------------------------
		//^ 类内定义的成员函数，按标准是隐式 inline 成员函数（[class.mfct]）。c_str() 这种写法就是声明兼定义，不需要再在类外写一份。
		//^ 调用时通过对象、引用或指针，并带上隐式 this：
		//^ Text t(buf);
		//^ const char* s = t.c_str();   // this 指向 t
		//^ const 成员函数只能由 const 对象调用，函数内不能修改非 mutable 成员。
		//^ inline 的标准含义是：该定义可以出现在多个翻译单元里，只要各份完全相同，就不违反 ODR。
		//^ 所以这些函数可以写在被多处包含的头文件中。编译器是否把函数体展开到调用点，由实现决定。
		//^ 类内只声明、类外再定义的函数（如 ~Text()）没有这个隐式 inline。它在 loguru.cpp 里只有一份定义，头文件里只能放声明。

		bool empty() const { return _str == nullptr || *_str == '\0'; }

		char* release()
		{
			auto result = _str; //- auto 是 C++11 引入的类型推导关键字，用于自动推导变量的类型。
			//^ 在这里，auto 会自动推导 result 的类型为 char*，因为 _str 的类型就是 char*。
			//^ 这样写的好处是，当 _str 的类型发生变化时，result 的类型会自动适应，不需要手动修改。

			_str = nullptr;
			return result;
		}

	//v private 是访问说明符。从它开始，直到下一个 public: / protected: 或类结束，里面的成员只能由本类的成员函数和 friend 使用。
	//v 这是编译期检查，不是运行时把内存藏起来。_str 仍然在对象里，只是别的代码不能按名字访问它。
	//v 派生类也访问不了 private 成员。若希望子类能用、类外不能用，应写成 protected:。
	//v class 默认就是 private，这里先写了 public:，所以要再写一次 private: 才能把 _str 收进去。struct 默认则是 public。
	private:
		char* _str;
	};

	// Like printf, but returns the formated text.
#if LOGURU_USE_FMTLIB
	LOGURU_EXPORT
	Text vtextprintf(const char* format, fmt::format_args args);

	//v 模板是编译期的代码模具。template 开头的声明本身还不是一份可直接调用的函数，而是一份蓝图。
	//v 调用时编译器按实参类型生成具体代码，这个过程叫模板实例化（instantiation）。
	//v 例如 textprintf("n={}", 42) 会生成 Args 里只有 int 的那一份函数。
	//v template<typename T> 里的 T 是类型参数。这里的 typename 和 class 含义相同，不要求 T 必须是类。
	//v 函数模板：template<typename T> void f(T x);
	//v 类模板：template<typename T> class Box { T value; }; 使用时要写成 Box<int>。
	//v typename... Args 是参数包（parameter pack），代表零个或多个类型。这就是可变参数模板。
	//v 函数参数写成 const Args&... args 时，每个实参都按 const 引用绑定。
	//v 调用处写 args... 是包展开（pack expansion），把包里的参数按顺序传给后面的函数。
	//v 模板的定义通常要放在头文件里。实例化发生在调用点，编译器当时必须看见完整定义。
	template<typename... Args>
	LOGURU_EXPORT
	Text textprintf(LOGURU_FORMAT_STRING_TYPE format, const Args&... args) {
		return vtextprintf(format, fmt::make_format_args(args...));
	}
	//^ 这是一个可变参数函数模板（variadic function template）。调用 textprintf("n={}", 42) 时，编译器按实参个数和类型生成一份具体函数。
	//^ ------------------------------------------------------------------------------------------------------------------
	//^ 片段								| 语法角色
	//^ ------------------------------------------------------------------------------------------------------------------
	//^ template<typename... Args> 		   | 声明模板。Args 是 parameter pack，代表零个或多个类型
	//^ LOGURU_EXPORT 						| 宏，控制符号是否导出，不是 C++ 语法 
	//^ Text textprintf(...) 				| 函数名 textprintf，返回类型是前面的 Text
	//^ LOGURU_FORMAT_STRING_TYPE format 	| 普通参数。这个宏在这里展开成 const char*，即格式串
	//^ const Args&... args 				| 函数参数包。每个实参都按 const T& 绑定，避免拷贝
	//^ args... 							| 把参数包展开成 make_format_args 的实参列表
	//^ ------------------------------------------------------------------------------------------------------------------
	//^ 函数体把格式串和展开后的参数交给 vtextprintf。fmt::make_format_args 把各类型参数收成一个 fmt::format_args，所以真正做格式化的 vtextprintf 不必再写成模板。
#else
	LOGURU_EXPORT
	Text textprintf(LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(1, 2);
#endif

	// Overloaded for variadic template matching.
	LOGURU_EXPORT
	Text textprintf();

	using Verbosity = int; //- using 是 C++11 引入的语法，用于定义类型别名。
	//^ 在这里，using Verbosity = int; 定义了一个名为 Verbosity 的类型别名，等价于 typedef int Verbosity;。
	//^ 这样写的好处是，当 Verbosity 的类型发生变化时，只需要修改一处定义，不需要修改所有使用的地方。

#undef FATAL
#undef ERROR
#undef WARNING
#undef INFO
#undef MAX

	//v ":"后面是枚举的底层类型（underlying type），不是继承。
	//v Verbosity 是 int 的别名，所以这行等价于：
	//v enum NamedVerbosity : int
	//v NamedVerbosity 仍是独立的枚举类型。它的枚举值按 int 存储，无作用域枚举可以隐式转换成 int。这里没有基类，也不能当派生类用。
	enum NamedVerbosity : Verbosity 
	{
		// Used to mark an invalid verbosity. Do not log to this level.
		Verbosity_INVALID = -10, // Never do LOG_F(INVALID)

		// You may use Verbosity_OFF on g_stderr_verbosity, but for nothing else!
		Verbosity_OFF     = -9, // Never do LOG_F(OFF)

		// Prefer to use ABORT_F or ABORT_S over LOG_F(FATAL) or LOG_S(FATAL).
		Verbosity_FATAL   = -3,
		Verbosity_ERROR   = -2,
		Verbosity_WARNING = -1,

		// Normal messages. By default written to stderr.
		Verbosity_INFO    =  0,

		// Same as Verbosity_INFO in every way.
		Verbosity_0       =  0,

		// Verbosity levels 1-9 are generally not written to stderr, but are written to file.
		Verbosity_1       = +1,
		Verbosity_2       = +2,
		Verbosity_3       = +3,
		Verbosity_4       = +4,
		Verbosity_5       = +5,
		Verbosity_6       = +6,
		Verbosity_7       = +7,
		Verbosity_8       = +8,
		Verbosity_9       = +9,

		// Do not use higher verbosity levels, as that will make grepping log files harder.
		Verbosity_MAX     = +9,
	};

	struct Message
	{
		// You would generally print a Message by just concatenating the buffers without spacing.
		// Optionally, ignore preamble and indentation.
		Verbosity   verbosity;   // Already part of preamble
		const char* filename;    // Already part of preamble
		unsigned    line;        // Already part of preamble
		const char* preamble;    // Date, time, uptime, thread, file:line, verbosity.
		const char* indentation; // Just a bunch of spacing.
		const char* prefix;      // Assertion failure info goes here (or "").
		const char* message;     // User message goes here.
	};

	/* Everything with a verbosity equal or greater than g_stderr_verbosity will be
	written to stderr. You can set this in code or via the -v argument.
	Set to loguru::Verbosity_OFF to write nothing to stderr.
	Default is 0, i.e. only log ERROR, WARNING and INFO are written to stderr.
	*/
	LOGURU_EXPORT extern Verbosity g_stderr_verbosity;
	LOGURU_EXPORT extern bool      g_colorlogtostderr; // True by default.
	LOGURU_EXPORT extern unsigned  g_flush_interval_ms; // 0 (unbuffered) by default.
	LOGURU_EXPORT extern bool      g_preamble_header; // Prepend each log start by a descriptions line with all columns name? True by default.
	LOGURU_EXPORT extern bool      g_preamble; // Prefix each log line with date, time etc? True by default.

	/* Specify the verbosity used by loguru to log its info messages including the header
	logged when logged::init() is called or on exit. Default is 0 (INFO).
	*/
	LOGURU_EXPORT extern Verbosity g_internal_verbosity;

	// Turn off individual parts of the preamble
	LOGURU_EXPORT extern bool      g_preamble_date; // The date field
	LOGURU_EXPORT extern bool      g_preamble_time; // The time of the current day
	LOGURU_EXPORT extern bool      g_preamble_uptime; // The time since init call
	LOGURU_EXPORT extern bool      g_preamble_thread; // The logging thread
	LOGURU_EXPORT extern bool      g_preamble_file; // The file from which the log originates from
	LOGURU_EXPORT extern bool      g_preamble_verbose; // The verbosity field
	LOGURU_EXPORT extern bool      g_preamble_pipe; // The pipe symbol right before the message

	// May not throw!
	typedef void (*log_handler_t)(void* user_data, const Message& message);
	typedef void (*close_handler_t)(void* user_data);
	typedef void (*flush_handler_t)(void* user_data);

	// May throw if that's how you'd like to handle your errors.
	typedef void (*fatal_handler_t)(const Message& message);

	// Given a verbosity level, return the level's name or nullptr.
	typedef const char* (*verbosity_to_name_t)(Verbosity verbosity);

	// Given a verbosity level name, return the verbosity level or
	// Verbosity_INVALID if name is not recognized.
	typedef Verbosity (*name_to_verbosity_t)(const char* name);

	//v C++ 里用 struct 或 class 定义出来的都是类（class）。标准把这两种写法都叫 class type。
	//v 二者默认访问权限不同：struct 的成员默认 public，class 的成员默认 private。
	//v 除此之外可以同样写构造函数、成员函数、继承和默认成员初始化。SignalOptions 因此既是 struct，也是一个类。
	struct SignalOptions
	{
		/// Make Loguru try to do unsafe but useful things,
		/// like printing a stack trace, when catching signals.
		/// This may lead to bad things like deadlocks in certain situations.
		bool unsafe_signal_handler = true;
		//^ 这是 C++11 的默认成员初始化（default member initializer）。= true 写在类定义里，但不会在定义类型时执行，而是在创建 SignalOptions 对象、且构造函数没有另行指定该成员时，用 true 初始化它。
		//^ SignalOptions options;  // unsafe_signal_handler 此时为 true
		//^ 这个结构体没有手写构造函数，编译器生成的默认构造函数会采用这些初值。
		//^ 后面的 sigabrt = true 等同理。none() 先这样构造，再逐个改成 false：
		//^ SignalOptions options;
		//^ options.unsafe_signal_handler = false;
		//^ 如果某个构造函数的初始化列表写了 unsafe_signal_handler(false)，这次构造就不会再用类里的 = true。

		/// Should Loguru catch SIGABRT ?
		bool sigabrt = true;

		/// Should Loguru catch SIGBUS ?
		bool sigbus = true;

		/// Should Loguru catch SIGFPE ?
		bool sigfpe = true;

		/// Should Loguru catch SIGILL ?
		bool sigill = true;

		/// Should Loguru catch SIGINT ?
		bool sigint = true;

		/// Should Loguru catch SIGSEGV ?
		bool sigsegv = true;

		/// Should Loguru catch SIGTERM ?
		bool sigterm = true;

		static SignalOptions none() //- notes: none 不是构造函数，编译器不会自动调用它
		{
			SignalOptions options;
			options.unsafe_signal_handler = false;
			options.sigabrt = false;
			options.sigbus = false;
			options.sigfpe = false;
			options.sigill = false;
			options.sigint = false;
			options.sigsegv = false;
			options.sigterm = false;
			return options;
		}
	};

	// Runtime options passed to loguru::init
	struct Options
	{
		// This allows you to use something else instead of "-v" via verbosity_flag.
		// Set to nullptr if you don't want Loguru to parse verbosity from the args.
		const char* verbosity_flag = "-v";
		//^ const char * a 里，const 修饰的是 char，也就是 *a。
		//^ *a 不可改：不能写 *a = 'x'。
		//^ a 可以改：可以写 a = 另一个地址，让它改指向别的字符。
		//^ 指针本身不可改的写法是 char * const a。两边都不可改则是 const char * const a。

		// loguru::init will set the name of the calling thread to this.
		// If you don't want Loguru to set the name of the main thread,
		// set this to nullptr.
		// NOTE: on SOME platforms loguru::init will only overwrite the thread name
		// if a thread name has not already been set.
		// To always set a thread name, use loguru::set_thread_name instead.
		const char* main_thread_name = "main thread";

		SignalOptions signal_options;
	};

	/*  Should be called from the main thread.
		You don't *need* to call this, but if you do you get:
			* Signal handlers installed
			* Program arguments logged
			* Working dir logged
			* Optional -v verbosity flag parsed
			* Main thread name set to "main thread"
			* Explanation of the preamble (date, thread name, etc) logged

		loguru::init() will look for arguments meant for loguru and remove them.
		Arguments meant for loguru are:
			-v n   Set loguru::g_stderr_verbosity level. Examples:
				-v 3        Show verbosity level 3 and lower.
				-v 0        Only show INFO, WARNING, ERROR, FATAL (default).
				-v INFO     Only show INFO, WARNING, ERROR, FATAL (default).
				-v WARNING  Only show WARNING, ERROR, FATAL.
				-v ERROR    Only show ERROR, FATAL.
				-v FATAL    Only show FATAL.
				-v OFF      Turn off logging to stderr.

		Tip: You can set g_stderr_verbosity before calling loguru::init.
		That way you can set the default but have the user override it with the -v flag.
		Note that -v does not affect file logging (see loguru::add_file).

		You can you something other than the -v flag by setting the verbosity_flag option.
	*/
	LOGURU_EXPORT
	void init(int& argc, char* argv[], const Options& options = {});

	// Will call remove_all_callbacks(). After calling this, logging will still go to stderr.
	// You generally don't need to call this.
	LOGURU_EXPORT
	void shutdown();

	// What ~ will be replaced with, e.g. "/home/your_user_name/"
	LOGURU_EXPORT
	const char* home_dir();

	/* Returns the name of the app as given in argv[0] but without leading path.
	   That is, if argv[0] is "../foo/app" this will return "app".
	*/
	LOGURU_EXPORT
	const char* argv0_filename();

	// Returns all arguments given to loguru::init(), but escaped with a single space as separator.
	LOGURU_EXPORT
	const char* arguments();

	// Returns the path to the current working dir when loguru::init() was called.
	LOGURU_EXPORT
	const char* current_dir();

	// Returns the part of the path after the last / or \ (if any).
	LOGURU_EXPORT
	const char* filename(const char* path);

	// e.g. "foo/bar/baz.ext" will create the directories "foo/" and "foo/bar/"
	LOGURU_EXPORT
	bool create_directories(const char* file_path_const);

	// Writes date and time with millisecond precision, e.g. "20151017_161503.123"
	LOGURU_EXPORT
	void write_date_time(char* buff, unsigned long long buff_size);

	// Helper: thread-safe version strerror
	LOGURU_EXPORT
	Text errno_as_text();

	/* Given a prefix of e.g. "~/loguru/" this might return
	   "/home/your_username/loguru/app_name/20151017_161503.123.log"

	   where "app_name" is a sanitized version of argv[0].
	*/
	LOGURU_EXPORT
	void suggest_log_path(const char* prefix, char* buff, unsigned long long buff_size);

	enum FileMode { Truncate, Append };

	/*  Will log to a file at the given path.
		Any logging message with a verbosity lower or equal to
		the given verbosity will be included.
		The function will create all directories in 'path' if needed.
		If path starts with a ~, it will be replaced with loguru::home_dir()
		To stop the file logging, just call loguru::remove_callback(path) with the same path.
	*/
	LOGURU_EXPORT
	bool add_file(const char* path, FileMode mode, Verbosity verbosity);

	LOGURU_EXPORT
	// Send logs to syslog with LOG_USER facility (see next call)
	bool add_syslog(const char* app_name, Verbosity verbosity);
	LOGURU_EXPORT
	// Send logs to syslog with your own choice of facility (LOG_USER, LOG_AUTH, ...)
	// see loguru.cpp: syslog_log() for more details.
	bool add_syslog(const char* app_name, Verbosity verbosity, int facility);

	/*  Will be called right before abort().
		You can for instance use this to print custom error messages, or throw an exception.
		Feel free to call LOG:ing function from this, but not FATAL ones! */
	LOGURU_EXPORT
	void set_fatal_handler(fatal_handler_t handler);

	// Get the current fatal handler, if any. Default value is nullptr.
	LOGURU_EXPORT
	fatal_handler_t get_fatal_handler();

	/*  Will be called on each log messages with a verbosity less or equal to the given one.
		Useful for displaying messages on-screen in a game, for example.
		The given on_close is also expected to flush (if desired).
	*/
	LOGURU_EXPORT
	void add_callback(
		const char*     id,
		log_handler_t   callback,
		void*           user_data,
		Verbosity       verbosity,
		close_handler_t on_close = nullptr,
		flush_handler_t on_flush = nullptr);
	//^ 这是函数参数的默认实参（default argument）。= nullptr 写在声明里，表示调用时如果省略这个参数，编译器就在调用点补上 nullptr。
	//^ 因此这三种调用都合法：
	//^ add_callback(id, cb, data, verbosity);
	//^ add_callback(id, cb, data, verbosity, my_close);
	//^ add_callback(id, cb, data, verbosity, my_close, my_flush);
	//^ 前两种里，被省略的参数在调用点变成 nullptr。函数体拿到的仍然是一个实实在在的参数，并不会知道调用方当初写没写。
	//^ 语法上有两条限制：
	//^ 默认实参只能出现在参数列表的尾部。这里 on_close 有默认值，它后面的 on_flush 也必须有。
	//^ 同一个参数的默认值只能写一次，所以头文件里写了，loguru.cpp 里的定义就不再重复 = nullptr。

	/*  Set a callback that returns custom verbosity level names. If callback
		is nullptr or returns nullptr, default log names will be used.
	*/
	LOGURU_EXPORT
	void set_verbosity_to_name_callback(verbosity_to_name_t callback);

	/*  Set a callback that returns the verbosity level matching a name. The
		callback should return Verbosity_INVALID if the name is not
		recognized.
	*/
	LOGURU_EXPORT
	void set_name_to_verbosity_callback(name_to_verbosity_t callback);

	/*  Get a custom name for a specific verbosity, if one exists, or nullptr. */
	LOGURU_EXPORT
	const char* get_verbosity_name(Verbosity verbosity);

	/*  Get the verbosity enum value from a custom 4-character level name, if one exists.
		If the name does not match a custom level name, Verbosity_INVALID is returned.
	*/
	LOGURU_EXPORT
	Verbosity get_verbosity_from_name(const char* name);

	// Returns true iff the callback was found (and removed).
	LOGURU_EXPORT
	bool remove_callback(const char* id);

	// Shut down all file logging and any other callback hooks installed.
	LOGURU_EXPORT
	void remove_all_callbacks();

	// Returns the maximum of g_stderr_verbosity and all file/custom outputs.
	LOGURU_EXPORT
	Verbosity current_verbosity_cutoff();

#if LOGURU_USE_FMTLIB
	// Internal functions
    LOGURU_EXPORT
	void vlog(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, fmt::format_args args);
    LOGURU_EXPORT
	void raw_vlog(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, fmt::format_args args);

	// Actual logging function. Use the LOG macro instead of calling this directly.
	template <typename... Args>
	LOGURU_EXPORT
	void log(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, const Args &... args) {
	    vlog(verbosity, file, line, format, fmt::make_format_args(args...));
	}

	// Log without any preamble or indentation.
	template <typename... Args>
	LOGURU_EXPORT
	void raw_log(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, const Args &... args) {
	    raw_vlog(verbosity, file, line, format, fmt::make_format_args(args...));
	}
#else // LOGURU_USE_FMTLIB?
	// Actual logging function. Use the LOG macro instead of calling this directly.
	LOGURU_EXPORT
	void log(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(4, 5);

	// Actual logging function.
	LOGURU_EXPORT
	void vlog(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, va_list) LOGURU_PRINTF_LIKE(4, 0);

	// Log without any preamble or indentation.
	LOGURU_EXPORT
	void raw_log(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(4, 5);
#endif // !LOGURU_USE_FMTLIB

	// Helper class for LOG_SCOPE_F
	class LOGURU_EXPORT LogScopeRAII
	{
	public:
		LogScopeRAII() : _file(nullptr) {} // No logging
#if LOGURU_USE_FMTLIB
		template <typename... Args>
		LogScopeRAII(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, const Args&... args) :
			_verbosity(verbosity), _file(file), _line(line)
		{
			this->Init(format, fmt::make_format_args(args...));
		}
#else
		LogScopeRAII(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, va_list vlist) LOGURU_PRINTF_LIKE(5, 0);
		LogScopeRAII(Verbosity verbosity, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(5, 6);
#endif
		~LogScopeRAII();

#if LOGURU_USE_FMTLIB
		void Init(LOGURU_FORMAT_STRING_TYPE format, fmt::format_args args);
#else
		void Init(LOGURU_FORMAT_STRING_TYPE format, va_list vlist) LOGURU_PRINTF_LIKE(2, 0);
#endif

#if defined(_MSC_VER) && _MSC_VER > 1800
		// older MSVC default move ctors close the scope on move. See
		// issue #43
		LogScopeRAII(LogScopeRAII&& other)
			: _verbosity(other._verbosity)
			, _file(other._file)
			, _line(other._line)
			, _indent_stderr(other._indent_stderr)
			, _start_time_ns(other._start_time_ns)
		{
			// Make sure the tmp object's destruction doesn't close the scope:
			other._file = nullptr;

			for (unsigned int i = 0; i < LOGURU_SCOPE_TEXT_SIZE; ++i) {
				_name[i] = other._name[i];
			}
		}
#else
		LogScopeRAII(LogScopeRAII&&) = default;
		//^ = default 让编译器生成该成员的默认实现，而不是自己写函数体。
		//^ 这里是移动构造函数：按成员逐个移动，指针和整数就是拷贝其值。
		//^ 上面 MSVC 的手写版本还会把 other._file 置空，默认版本不会。
		//^ 同文件里的 = delete 则是禁止该函数，调用时直接编译失败。
#endif

	private:
		LogScopeRAII(const LogScopeRAII&) = delete;
		LogScopeRAII& operator=(const LogScopeRAII&) = delete;
		void operator=(LogScopeRAII&&) = delete;

		//v 非静态成员默认初始化
		//v 创建对象时，如果构造函数的初始化列表没有另行指定该成员，就用这里的值。
		//v _file、_line、_indent_stderr、_start_time_ns 也是同样的写法。
		Verbosity   _verbosity = Verbosity_INVALID;
		const char* _file = nullptr; // Set to null if we are disabled due to verbosity
		unsigned    _line = 0;
		bool        _indent_stderr = false; // Did we?
		long long   _start_time_ns = 0;
		char        _name[LOGURU_SCOPE_TEXT_SIZE] = {};
		//^ {} 对这个 char 数组做值初始化，每个元素都变成 '\0'。
	};

	// Marked as 'noreturn' for the benefit of the static analyzer and optimizer.
	// stack_trace_skip is the number of extrace stack frames to skip above log_and_abort.
#if LOGURU_USE_FMTLIB
	LOGURU_EXPORT
	LOGURU_NORETURN void vlog_and_abort(int stack_trace_skip, const char* expr, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, fmt::format_args);
	template <typename... Args>
	LOGURU_EXPORT
	LOGURU_NORETURN void log_and_abort(int stack_trace_skip, const char* expr, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, const Args&... args) {
	    vlog_and_abort(stack_trace_skip, expr, file, line, format, fmt::make_format_args(args...));
	}
#else
	LOGURU_EXPORT
	LOGURU_NORETURN void log_and_abort(int stack_trace_skip, const char* expr, const char* file, unsigned line, LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(5, 6);
#endif
	LOGURU_EXPORT
	LOGURU_NORETURN void log_and_abort(int stack_trace_skip, const char* expr, const char* file, unsigned line);

	// Flush output to stderr and files.
	// If g_flush_interval_ms is set to non-zero, this will be called automatically this often.
	// If not set, you do not need to call this at all.
	LOGURU_EXPORT
	void flush();

	//v 这里的 class 表示 T 是类型参数，和 template<typename T> 完全相同。它不要求传进来的必须是类。
	//v T 可以是 int、char、指针或某个类。没有专门特化的类型走这个主模板，打印 "N/A"。
	//v char、int、float 这些内置类型走后面的特化，说明 class 并没有把非类类型排除在外。
	template<class T> inline Text format_value(const T&)                    { return textprintf("N/A");     }
	template<>        inline Text format_value(const char& v)               { return textprintf(LOGURU_FMT(c),   v); }
	template<>        inline Text format_value(const int& v)                { return textprintf(LOGURU_FMT(d),   v); }
	template<>        inline Text format_value(const float& v)              { return textprintf(LOGURU_FMT(f),   v); }
	template<>        inline Text format_value(const double& v)             { return textprintf(LOGURU_FMT(f),   v); }

#if LOGURU_USE_FMTLIB
	template<>        inline Text format_value(const unsigned int& v)       { return textprintf(LOGURU_FMT(d), v); }
	template<>        inline Text format_value(const long& v)               { return textprintf(LOGURU_FMT(d), v); }
	template<>        inline Text format_value(const unsigned long& v)      { return textprintf(LOGURU_FMT(d), v); }
	template<>        inline Text format_value(const long long& v)          { return textprintf(LOGURU_FMT(d), v); }
	template<>        inline Text format_value(const unsigned long long& v) { return textprintf(LOGURU_FMT(d), v); }
#else
	template<>        inline Text format_value(const unsigned int& v)       { return textprintf(LOGURU_FMT(u),   v); }
	template<>        inline Text format_value(const long& v)               { return textprintf(LOGURU_FMT(lu),  v); }
	template<>        inline Text format_value(const unsigned long& v)      { return textprintf(LOGURU_FMT(ld),  v); }
	template<>        inline Text format_value(const long long& v)          { return textprintf(LOGURU_FMT(llu), v); }
	template<>        inline Text format_value(const unsigned long long& v) { return textprintf(LOGURU_FMT(lld), v); }
#endif

	/* Thread names can be set for the benefit of readable logs.
	   If you do not set the thread name, a hex id will be shown instead.
	   These thread names may or may not be the same as the system thread names,
	   depending on the system.
	   Try to limit the thread name to 15 characters or less. */
	LOGURU_EXPORT
	void set_thread_name(const char* name);

	/* Returns the thread name for this thread.
	   On most *nix systems this will return the system thread name (settable from both within and without Loguru).
	   On other systems it will return whatever you set in `set_thread_name()`;
	   If no thread name is set, this will return a hexadecimal thread id.
	   `length` should be the number of bytes available in the buffer.
	   17 is a good number for length.
	   `right_align_hex_id` means any hexadecimal thread id will be written to the end of buffer.
	*/
	LOGURU_EXPORT
	void get_thread_name(char* buffer, unsigned long long length, bool right_align_hex_id);

	/* Generates a readable stacktrace as a string.
	   'skip' specifies how many stack frames to skip.
	   For instance, the default skip (1) means:
	   don't include the call to loguru::stacktrace in the stack trace. */
	LOGURU_EXPORT
	Text stacktrace(int skip = 1);

	/*  Add a string to be replaced with something else in the stack output.

		For instance, instead of having a stack trace look like this:
			0x41f541 some_function(std::basic_ofstream<char, std::char_traits<char> >&)
		You can clean it up with:
			auto verbose_type_name = loguru::demangle(typeid(std::ofstream).name());
			loguru::add_stack_cleanup(verbose_type_name.c_str(); "std::ofstream");
		So the next time you will instead see:
			0x41f541 some_function(std::ofstream&)

		`replace_with_this` must be shorter than `find_this`.
	*/
	LOGURU_EXPORT
	void add_stack_cleanup(const char* find_this, const char* replace_with_this);

	// Example: demangle(typeid(std::ofstream).name()) -> "std::basic_ofstream<char, std::char_traits<char> >"
	LOGURU_EXPORT
	Text demangle(const char* name);

	// ------------------------------------------------------------------------
	/*
	Not all terminals support colors, but if they do, and g_colorlogtostderr
	is set, Loguru will write them to stderr to make errors in red, etc.

	You also have the option to manually use them, via the function below.

	Note, however, that if you do, the color codes could end up in your logfile!

	This means if you intend to use them functions you should either:
		* Use them on the stderr/stdout directly (bypass Loguru).
		* Don't add file outputs to Loguru.
		* Expect some \e[1m things in your logfile.

	Usage:
		printf("%sRed%sGreen%sBold green%sClear again\n",
			   loguru::terminal_red(), loguru::terminal_green(),
			   loguru::terminal_bold(), loguru::terminal_reset());

	If the terminal at hand does not support colors the above output
	will just not have funky \e[1m things showing.
	*/

	// Do the output terminal support colors?
	LOGURU_EXPORT
	bool terminal_has_color();

	// Colors
	LOGURU_EXPORT const char* terminal_black();
	LOGURU_EXPORT const char* terminal_red();
	LOGURU_EXPORT const char* terminal_green();
	LOGURU_EXPORT const char* terminal_yellow();
	LOGURU_EXPORT const char* terminal_blue();
	LOGURU_EXPORT const char* terminal_purple();
	LOGURU_EXPORT const char* terminal_cyan();
	LOGURU_EXPORT const char* terminal_light_gray();
	LOGURU_EXPORT const char* terminal_light_red();
	LOGURU_EXPORT const char* terminal_white();

	// Formating
	LOGURU_EXPORT const char* terminal_bold();
	LOGURU_EXPORT const char* terminal_underline();

	// You should end each line with this!
	LOGURU_EXPORT const char* terminal_reset();

	// --------------------------------------------------------------------
	// Error context related:

	struct StringStream;

	// Use this in your EcEntryBase::print_value overload.
	LOGURU_EXPORT
	void stream_print(StringStream& out_string_stream, const char* text);

	class LOGURU_EXPORT EcEntryBase
	{
	public:
		EcEntryBase(const char* file, unsigned line, const char* descr);
		virtual ~EcEntryBase(); //- 虚函数。用基类指针删除派生类对象时，会先调派生类析构，再调这个基类析构。 
		EcEntryBase(const EcEntryBase&) = delete;
		EcEntryBase(EcEntryBase&&) = delete;
		EcEntryBase& operator=(const EcEntryBase&) = delete;
		EcEntryBase& operator=(EcEntryBase&&) = delete;

		virtual void print_value(StringStream& out_string_stream) const = 0;
		//^ 纯虚函数是在声明末尾写了 = 0 的虚函数。基类不提供可以单独使用的实现，派生类必须自己实现（override），否则派生类同样不能创建对象。
		//^ 含有纯虚函数的类是抽象类，不能直接构造。 EcEntryBase 因此不能写成 EcEntryBase x(...)。

		EcEntryBase* previous() const { return _previous; }

	// private:
		const char*  _file;
		unsigned     _line;
		const char*  _descr;
		EcEntryBase* _previous = nullptr;
	};

	//v 下面三行定义了一个公有继承 EcEntryBase 的类模板。T 换成具体类型后，才会生成一个真正的类。
    //v template<typename T>
    //v class EcEntryData : public EcEntryBase
    //v {
    //v 片段	                     含义
	//v -------------------------------------------------------------------------------------------------
	//v template<typename T>        T 是类型参数。EcEntryData<int> 和 EcEntryData<const char*> 是两个不同的类
    //v class EcEntryData           类名
    //v : public EcEntryBase        公有继承。EcEntryData<T> 含有一个 EcEntryBase 子对象，并能使用基类的公有接口
	//v -------------------------------------------------------------------------------------------------
    //v public 继承表示「是一个」：EcEntryData<T> 对象可以当作 EcEntryBase 使用。基类里的 print_value 是纯虚函数，派生类必须实现它。
	//v 于是基类指针 EcEntryBase* 可以指向不同 T 的 EcEntryData，调用时按实际类型走到对应的 print_value。
    //v 构造时先初始化基类子对象，所以构造函数的初始化列表第一项是 EcEntryBase(file, line, descr)。
	template<typename T>
	class EcEntryData : public EcEntryBase
	{
	public:
		using Printer = Text(*)(T data);

		EcEntryData(const char* file, unsigned line, const char* descr, T data, Printer&& printer)
			: EcEntryBase(file, line, descr), _data(data), _printer(printer) {}

		//v 普通虚函数，同样用 virtual 声明，但没有 = 0，基类里有函数体，派生类可以选择覆盖。
		//v override 标明：这个函数要覆盖基类里的同名虚函数。编译器会核对其签名是否与基类完全一致，对不上就报错。
		//v virtual void print_value(StringStream& out_string_stream) const override
		//v 它对应基类的纯虚函数：
		//v virtual void print_value(StringStream& out_string_stream) const = 0;
		//v 名字、参数、const 都一致，所以 EcEntryData<T> 实现了 print_value。通过 EcEntryBase* 调用时，会执行这里的函数体。
		//v 如果少写 const，或参数类型写错，没有 override 时这会变成另一个新函数，基类的纯虚函数仍然没被实现。
		//v 加上 override 后，这种不匹配会直接编译失败。virtual 在派生类里可以省略，写上只是为了阅读时能看出它是虚函数。
		virtual void print_value(StringStream& out_string_stream) const override
		{
			const auto str = _printer(_data);
			stream_print(out_string_stream, str.c_str());
		}

	private:
		T       _data;
		Printer _printer;
	};

	// template<typename Printer>
	// class EcEntryLambda : public EcEntryBase
	// {
	// public:
	// 	EcEntryLambda(const char* file, unsigned line, const char* descr, Printer&& printer)
	// 		: EcEntryBase(file, line, descr), _printer(std::move(printer)) {}

	// 	virtual void print_value(StringStream& out_string_stream) const override
	// 	{
	// 		const auto str = _printer();
	// 		stream_print(out_string_stream, str.c_str());
	// 	}

	// private:
	// 	Printer _printer;
	// };

	// template<typename Printer>
	// EcEntryLambda<Printer> make_ec_entry_lambda(const char* file, unsigned line, const char* descr, Printer&& printer)
	// {
	// 	return {file, line, descr, std::move(printer)};
	// }

	template <class T>
	struct decay_char_array { using type = T; };

	template <unsigned long long  N>
	struct decay_char_array<const char(&)[N]> { using type = const char*; };
	//^ const char(&)[N] 是数组引用：引用绑定一整块长度为 N 的 const char 数组，数组不会退化成指针。
	//^ 字符串字面量传进来时走这份特化，type 变成 const char*。

	template <class T>
	struct make_const_ptr { using type = T; };

	template <class T>
	struct make_const_ptr<T*> { using type = const T*; };

	template <class T>
	struct make_ec_type { using type = typename make_const_ptr<typename decay_char_array<T>::type>::type; };

	//^^ using type = ... 是结构体内的成员类型别名，名字固定叫 type。
	//^^  这些结构体不保存数据，只做类型变换：传入 T，从 ::type 取出结果。
	//^^  主模板是默认规则，特化是例外。
	//^^ 
	//^^  decay_char_array：默认原样返回；只有 const char 数组的引用才变成指针。
	//^^    decay_char_array<int>::type               -> int
	//^^    decay_char_array<const char(&)[6]>::type  -> const char*
	//^^  字符串字面量的类型是 const char[N]，传参时常变成 const char(&)[N]，这一步把它收成 const char*。
	//^^ 
	//^^  make_const_ptr：默认原样返回；只有指针才给指向的类型加上 const。
	//^^    make_const_ptr<int>::type    -> int
	//^^    make_const_ptr<char*>::type  -> const char*
	//^^ 
	//^^  make_ec_type：先做 decay_char_array，再做 make_const_ptr。
	//^^  ::type 依赖模板参数，所以前面必须写 typename。
	//^^    make_ec_type<const char(&)[6]>::type -> const char*
	//^^    make_ec_type<int*>::type             -> const int*
	//^^    make_ec_type<int>::type              -> int
	//^^  ERROR_CONTEXT 用这个结果作为 EcEntryData<...> 的类型参数。

	/* 	A stack trace gives you the names of the function at the point of a crash.
		With ERROR_CONTEXT, you can also get the values of select local variables.
		Usage:

		void process_customers(const std::string& filename)
		{
			ERROR_CONTEXT("Processing file", filename.c_str());
			for (int customer_index : ...)
			{
				ERROR_CONTEXT("Customer index", customer_index);
				...
			}
		}

		The context is in effect during the scope of the ERROR_CONTEXT.
		Use loguru::get_error_context() to get the contents of the active error contexts.

		Example result:

		------------------------------------------------
		[ErrorContext]                main.cpp:416   Processing file:    "customers.json"
		[ErrorContext]                main.cpp:417   Customer index:     42
		------------------------------------------------

		Error contexts are printed automatically on crashes, and only on crashes.
		This makes them much faster than logging the value of a variable.
	*/
	#define ERROR_CONTEXT(descr, data)                                             \
		const loguru::EcEntryData<loguru::make_ec_type<decltype(data)>::type>      \
			LOGURU_ANONYMOUS_VARIABLE(error_context_scope_)(                       \
				__FILE__, __LINE__, descr, data,                                   \
				static_cast<loguru::EcEntryData<loguru::make_ec_type<decltype(data)>::type>::Printer>(loguru::ec_to_text) ) // For better error messages
				//^ decltype(expr) 在编译期得到 expr 的类型，不计算 expr 的值。
				//^ 这里 decltype(data) 是宏参数 data 那个表达式的类型。
				//^ 若 data 是变量名，得到的是它声明时的类型，不会额外加上引用。
				//^ 结果再交给 make_ec_type：字符数组收成指针，指针补上 const。
				//^ decltype((data)) 多一层括号时，左值会得到 T&，和 decltype(data) 不同。

/*
	#define ERROR_CONTEXT(descr, data)                                 \
		const auto LOGURU_ANONYMOUS_VARIABLE(error_context_scope_)(    \
			loguru::make_ec_entry_lambda(__FILE__, __LINE__, descr,    \
				[=](){ return loguru::ec_to_text(data); }))
*/

	using EcHandle = const EcEntryBase*;

	/*
		Get a light-weight handle to the error context stack on this thread.
		The handle is valid as long as the current thread has no changes to its error context stack.
		You can pass the handle to loguru::get_error_context on another thread.
		This can be very useful for when you have a parent thread spawning several working threads,
		and you want the error context of the parent thread to get printed (too) when there is an
		error on the child thread. You can accomplish this thusly:

		void foo(const char* parameter)
		{
			ERROR_CONTEXT("parameter", parameter)
			const auto parent_ec_handle = loguru::get_thread_ec_handle();

			std::thread([=]{
				loguru::set_thread_name("child thread");
				ERROR_CONTEXT("parent context", parent_ec_handle);
				dangerous_code();
			}.join();
		}

	*/
	LOGURU_EXPORT
	EcHandle get_thread_ec_handle();

	// Get a string describing the current stack of error context. Empty string if there is none.
	LOGURU_EXPORT
	Text get_error_context();

	// Get a string describing the error context of the given thread handle.
	LOGURU_EXPORT
	Text get_error_context_for(EcHandle ec_handle);

	// ------------------------------------------------------------------------

	LOGURU_EXPORT Text ec_to_text(const char* data);
	LOGURU_EXPORT Text ec_to_text(char data);
	LOGURU_EXPORT Text ec_to_text(int data);
	LOGURU_EXPORT Text ec_to_text(unsigned int data);
	LOGURU_EXPORT Text ec_to_text(long data);
	LOGURU_EXPORT Text ec_to_text(unsigned long data);
	LOGURU_EXPORT Text ec_to_text(long long data);
	LOGURU_EXPORT Text ec_to_text(unsigned long long data);
	LOGURU_EXPORT Text ec_to_text(float data);
	LOGURU_EXPORT Text ec_to_text(double data);
	LOGURU_EXPORT Text ec_to_text(long double data);
	LOGURU_EXPORT Text ec_to_text(EcHandle);

	/*
	You can add ERROR_CONTEXT support for your own types by overloading ec_to_text. Here's how:

	some.hpp:
		namespace loguru {
			Text ec_to_text(MySmallType data)
			Text ec_to_text(const MyBigType* data)
		} // namespace loguru

	some.cpp:
		namespace loguru {
			Text ec_to_text(MySmallType small_value)
			{
				// Called only when needed, i.e. on a crash.
				std::string str = small_value.as_string(); // Format 'small_value' here somehow.
				return Text{STRDUP(str.c_str())};
			}

			Text ec_to_text(const MyBigType* big_value)
			{
				// Called only when needed, i.e. on a crash.
				std::string str = big_value->as_string(); // Format 'big_value' here somehow.
				return Text{STRDUP(str.c_str())};
			}
		} // namespace loguru

	Any file that include some.hpp:
		void foo(MySmallType small, const MyBigType& big)
		{
			ERROR_CONTEXT("Small", small); // Copy ´small` by value.
			ERROR_CONTEXT("Big",   &big);  // `big` should not change during this scope!
			....
		}
	*/
} // namespace loguru

LOGURU_ANONYMOUS_NAMESPACE_END

//v 宏由预处理器处理。编译真正开始之前，它按文本替换展开，不是函数，也没有类型检查。
//v #define 定义宏，#undef 取消已有定义。
//v 对象式宏没有参数：#define STRDUP(str) strdup(str) 这种带参数的叫函数式宏。
//v 函数式宏的每个参数，以及整个替换结果，通常都要加括号，避免展开后和周围运算符抢优先级。
//v 参数里写 ... 表示接受任意多个实参，替换时用 __VA_ARGS__ 原样放回去。LOG_F(INFO, "n=%d", 42) 用的就是这个。
//v # 参数 会把参数变成字符串；## 会把两边的记号拼成一个名字，例如 Verbosity_ ## INFO 变成 Verbosity_INFO。
//v __FILE__ 和 __LINE__ 是预定义宏，展开成当前源文件名和行号。
//v 定义写不下时，行末加 \ 续行。\ 必须是这一行的最后一个字符。
//v 宏只做文本替换，不会检查参数类型，也不能递归展开自己。能用函数、constexpr 或模板表达时，通常不用宏。
// --------------------------------------------------------------------
// Logging macros

// LOG_F(2, "Only logged if verbosity is 2 or higher: %d", some_number);
#define VLOG_F(verbosity, ...)                                                                     \
	((verbosity) > loguru::current_verbosity_cutoff()) ? (void)0                                   \
									  : loguru::log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

// LOG_F(INFO, "Foo: %d", some_number);
#define LOG_F(verbosity_name, ...) VLOG_F(loguru::Verbosity_ ## verbosity_name, __VA_ARGS__)

#define VLOG_IF_F(verbosity, cond, ...)                                                            \
	((verbosity) > loguru::current_verbosity_cutoff() || (cond) == false)                          \
		? (void)0                                                                                  \
		: loguru::log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_IF_F(verbosity_name, cond, ...)                                                        \
	VLOG_IF_F(loguru::Verbosity_ ## verbosity_name, cond, __VA_ARGS__)

#define VLOG_SCOPE_F(verbosity, ...)                                                               \
	loguru::LogScopeRAII LOGURU_ANONYMOUS_VARIABLE(error_context_RAII_) =                          \
	((verbosity) > loguru::current_verbosity_cutoff()) ? loguru::LogScopeRAII() :                  \
	loguru::LogScopeRAII(verbosity, __FILE__, __LINE__, __VA_ARGS__)

// Raw logging - no preamble, no indentation. Slightly faster than full logging.
#define RAW_VLOG_F(verbosity, ...)                                                                 \
	((verbosity) > loguru::current_verbosity_cutoff()) ? (void)0                                   \
									  : loguru::raw_log(verbosity, __FILE__, __LINE__, __VA_ARGS__)

#define RAW_LOG_F(verbosity_name, ...) RAW_VLOG_F(loguru::Verbosity_ ## verbosity_name, __VA_ARGS__)

// Use to book-end a scope. Affects logging on all threads.
#define LOG_SCOPE_F(verbosity_name, ...)                                                           \
	VLOG_SCOPE_F(loguru::Verbosity_ ## verbosity_name, __VA_ARGS__)

#define LOG_SCOPE_FUNCTION(verbosity_name) LOG_SCOPE_F(verbosity_name, __func__)

// -----------------------------------------------
// ABORT_F macro. Usage:  ABORT_F("Cause of error: %s", error_str);

// Message is optional
#define ABORT_F(...) loguru::log_and_abort(0, "ABORT: ", __FILE__, __LINE__, __VA_ARGS__)

// --------------------------------------------------------------------
// CHECK_F macros:

#define CHECK_WITH_INFO_F(test, info, ...)                                                         \
	LOGURU_PREDICT_TRUE((test) == true) ? (void)0 : loguru::log_and_abort(0, "CHECK FAILED:  " info "  ", __FILE__,      \
													   __LINE__, ##__VA_ARGS__)

/* Checked at runtime too. Will print error, then call fatal_handler (if any), then 'abort'.
   Note that the test must be boolean.
   CHECK_F(ptr); will not compile, but CHECK_F(ptr != nullptr); will. */
#define CHECK_F(test, ...) CHECK_WITH_INFO_F(test, #test, ##__VA_ARGS__)

#define CHECK_NOTNULL_F(x, ...) CHECK_WITH_INFO_F((x) != nullptr, #x " != nullptr", ##__VA_ARGS__)

#define CHECK_OP_F(expr_left, expr_right, op, ...)                                                 \
	do                                                                                             \
	{                                                                                              \
		auto val_left = expr_left;                                                                 \
		auto val_right = expr_right;                                                               \
		if (! LOGURU_PREDICT_TRUE(val_left op val_right))                                          \
		{                                                                                          \
			auto str_left = loguru::format_value(val_left);                                        \
			auto str_right = loguru::format_value(val_right);                                      \
			auto fail_info = loguru::textprintf("CHECK FAILED:  " LOGURU_FMT(s) " " LOGURU_FMT(s) " " LOGURU_FMT(s) "  (" LOGURU_FMT(s) " " LOGURU_FMT(s) " " LOGURU_FMT(s) ")  ",           \
				#expr_left, #op, #expr_right, str_left.c_str(), #op, str_right.c_str());           \
			auto user_msg = loguru::textprintf(__VA_ARGS__);                                       \
			loguru::log_and_abort(0, fail_info.c_str(), __FILE__, __LINE__,                        \
			                      LOGURU_FMT(s), user_msg.c_str());                                         \
		}                                                                                          \
	} while (false)

#ifndef LOGURU_DEBUG_LOGGING
	#ifndef NDEBUG
		#define LOGURU_DEBUG_LOGGING 1
	#else
		#define LOGURU_DEBUG_LOGGING 0
	#endif
#endif

#if LOGURU_DEBUG_LOGGING
	// Debug logging enabled:
	#define DLOG_F(verbosity_name, ...)     LOG_F(verbosity_name, __VA_ARGS__)
	#define DVLOG_F(verbosity, ...)         VLOG_F(verbosity, __VA_ARGS__)
	#define DLOG_IF_F(verbosity_name, cond, ...)  LOG_IF_F(verbosity_name, cond, __VA_ARGS__)
	#define DVLOG_IF_F(verbosity, cond, ...)      VLOG_IF_F(verbosity, cond, __VA_ARGS__)
	#define DRAW_LOG_F(verbosity_name, ...) RAW_LOG_F(verbosity_name, __VA_ARGS__)
	#define DRAW_VLOG_F(verbosity, ...)     RAW_VLOG_F(verbosity, __VA_ARGS__)
	#define DVLOG_SCOPE_F(verbosity, ...)   VLOG_SCOPE_F(verbosity, __VA_ARGS__)
	#define DLOG_SCOPE_F(verbosity_name, ...) LOG_SCOPE_F(verbosity_name, __VA_ARGS__)
	#define DLOG_SCOPE_FUNCTION(verbosity_name) LOG_SCOPE_FUNCTION(verbosity_name)
#else
	// Debug logging disabled:
	#define DLOG_F(verbosity_name, ...)
	#define DVLOG_F(verbosity, ...)
	#define DLOG_IF_F(verbosity_name, ...)
	#define DVLOG_IF_F(verbosity, ...)
	#define DRAW_LOG_F(verbosity_name, ...)
	#define DRAW_VLOG_F(verbosity, ...)
	#define DVLOG_SCOPE_F(verbosity, ...)
	#define DLOG_SCOPE_F(verbosity_name, ...)
	#define DLOG_SCOPE_FUNCTION(verbosity_name)
#endif

#define CHECK_EQ_F(a, b, ...) CHECK_OP_F(a, b, ==, ##__VA_ARGS__)
#define CHECK_NE_F(a, b, ...) CHECK_OP_F(a, b, !=, ##__VA_ARGS__)
#define CHECK_LT_F(a, b, ...) CHECK_OP_F(a, b, < , ##__VA_ARGS__)
#define CHECK_GT_F(a, b, ...) CHECK_OP_F(a, b, > , ##__VA_ARGS__)
#define CHECK_LE_F(a, b, ...) CHECK_OP_F(a, b, <=, ##__VA_ARGS__)
#define CHECK_GE_F(a, b, ...) CHECK_OP_F(a, b, >=, ##__VA_ARGS__)

#ifndef LOGURU_DEBUG_CHECKS
	#ifndef NDEBUG
		#define LOGURU_DEBUG_CHECKS 1
	#else
		#define LOGURU_DEBUG_CHECKS 0
	#endif
#endif

#if LOGURU_DEBUG_CHECKS
	// Debug checks enabled:
	#define DCHECK_F(test, ...)             CHECK_F(test, ##__VA_ARGS__)
	#define DCHECK_NOTNULL_F(x, ...)        CHECK_NOTNULL_F(x, ##__VA_ARGS__)
	#define DCHECK_EQ_F(a, b, ...)          CHECK_EQ_F(a, b, ##__VA_ARGS__)
	#define DCHECK_NE_F(a, b, ...)          CHECK_NE_F(a, b, ##__VA_ARGS__)
	#define DCHECK_LT_F(a, b, ...)          CHECK_LT_F(a, b, ##__VA_ARGS__)
	#define DCHECK_LE_F(a, b, ...)          CHECK_LE_F(a, b, ##__VA_ARGS__)
	#define DCHECK_GT_F(a, b, ...)          CHECK_GT_F(a, b, ##__VA_ARGS__)
	#define DCHECK_GE_F(a, b, ...)          CHECK_GE_F(a, b, ##__VA_ARGS__)
#else
	// Debug checks disabled:
	#define DCHECK_F(test, ...)
	#define DCHECK_NOTNULL_F(x, ...)
	#define DCHECK_EQ_F(a, b, ...)
	#define DCHECK_NE_F(a, b, ...)
	#define DCHECK_LT_F(a, b, ...)
	#define DCHECK_LE_F(a, b, ...)
	#define DCHECK_GT_F(a, b, ...)
	#define DCHECK_GE_F(a, b, ...)
#endif // NDEBUG


#if LOGURU_REDEFINE_ASSERT
	#undef assert
	#ifndef NDEBUG
		// Debug:
		#define assert(test) CHECK_WITH_INFO_F(!!(test), #test) // HACK
	#else
		#define assert(test)
	#endif
#endif // LOGURU_REDEFINE_ASSERT

#endif // LOGURU_HAS_DECLARED_FORMAT_HEADER

// ----------------------------------------------------------------------------
// .dP"Y8 888888 88""Yb 888888    db    8b    d8 .dP"Y8
// `Ybo."   88   88__dP 88__     dPYb   88b  d88 `Ybo."
// o.`Y8b   88   88"Yb  88""    dP__Yb  88YbdP88 o.`Y8b
// 8bodP'   88   88  Yb 888888 dP""""Yb 88 YY 88 8bodP'

#if LOGURU_WITH_STREAMS
#ifndef LOGURU_HAS_DECLARED_STREAMS_HEADER
#define LOGURU_HAS_DECLARED_STREAMS_HEADER

/* This file extends loguru to enable std::stream-style logging, a la Glog.
   It's an optional feature behind the LOGURU_WITH_STREAMS settings
   because including it everywhere will slow down compilation times.
*/

#include <cstdarg>
#include <sstream> // Adds about 38 kLoC on clang.
#include <string>

LOGURU_ANONYMOUS_NAMESPACE_BEGIN

namespace loguru
{
	// Like sprintf, but returns the formated text.
	LOGURU_EXPORT
	std::string strprintf(LOGURU_FORMAT_STRING_TYPE format, ...) LOGURU_PRINTF_LIKE(1, 2);

	// Like vsprintf, but returns the formated text.
	LOGURU_EXPORT
	std::string vstrprintf(LOGURU_FORMAT_STRING_TYPE format, va_list) LOGURU_PRINTF_LIKE(1, 0);

	class LOGURU_EXPORT StreamLogger
	{
	public:
		StreamLogger(Verbosity verbosity, const char* file, unsigned line) : _verbosity(verbosity), _file(file), _line(line) {}
		~StreamLogger() noexcept(false);
		//^ noexcept(false) 表示这个析构函数允许抛出异常。

		//v operator 用来重载运算符。operator<< 定义的是：当左操作数是 StreamLogger 时，<< 做什么。
		//v 它是成员函数。<< 左边的对象就是 *this，右边的值是参数 t。
		//v logger << 42 等价于 logger.operator<<(42)。函数体里再把 42 送进内部的字符串流 _ss。
		//v 返回 StreamLogger& 是为了连续写：logger << a << b 会先执行左边的 <<，再用返回的自身去接 b。
		//v 返回引用才不会每接一次就拷贝出一个新对象。
		//v 运算符的优先级和操作数个数保持语言原样，重载只改它在这个类型上的含义。
		//v =、[]、() 必须写成成员函数；<< 也可以写成类外的非成员函数。
		template<typename T>
		StreamLogger& operator<<(const T& t)
		{
			_ss << t;
			return *this;
		}

		// std::endl and other iomanip:s.
		StreamLogger& operator<<(std::ostream&(*f)(std::ostream&))
		{
			f(_ss);
			return *this;
		}

	private:
		Verbosity   _verbosity;
		const char* _file;
		unsigned    _line;
		std::ostringstream _ss;
	};

	class LOGURU_EXPORT AbortLogger
	{
	public:
		AbortLogger(const char* expr, const char* file, unsigned line) : _expr(expr), _file(file), _line(line) { }
		LOGURU_NORETURN ~AbortLogger() noexcept(false);

		template<typename T>
		AbortLogger& operator<<(const T& t)
		{
			_ss << t;
			return *this;
		}

		// std::endl and other iomanip:s.
		AbortLogger& operator<<(std::ostream&(*f)(std::ostream&))
		{
			f(_ss);
			return *this;
		}

	private:
		const char*        _expr;
		const char*        _file;
		unsigned           _line;
		std::ostringstream _ss;
	};

	class LOGURU_EXPORT Voidify
	{
	public:
		Voidify() {}
		// This has to be an operator with a precedence lower than << but higher than ?:
		void operator&(const StreamLogger&) { }
		void operator&(const AbortLogger&)  { }
	};

	/*  Helper functions for CHECK_OP_S macro.
		GLOG trick: The (int, int) specialization works around the issue that the compiler
		will not instantiate the template version of the function on values of unnamed enum type. */
	#define DEFINE_CHECK_OP_IMPL(name, op)                                                             \
		template <typename T1, typename T2>                                                            \
		inline std::string* name(const char* expr, const T1& v1, const char* op_str, const T2& v2)     \
		{                                                                                              \
			if (LOGURU_PREDICT_TRUE(v1 op v2)) { return NULL; }                                        \
			std::ostringstream ss;                                                                     \
			ss << "CHECK FAILED:  " << expr << "  (" << v1 << " " << op_str << " " << v2 << ")  ";     \
			return new std::string(ss.str());                                                          \
		}                                                                                              \
		inline std::string* name(const char* expr, int v1, const char* op_str, int v2)                 \
		{                                                                                              \
			return name<int, int>(expr, v1, op_str, v2);                                               \
		}

	DEFINE_CHECK_OP_IMPL(check_EQ_impl, ==)
	DEFINE_CHECK_OP_IMPL(check_NE_impl, !=)
	DEFINE_CHECK_OP_IMPL(check_LE_impl, <=)
	DEFINE_CHECK_OP_IMPL(check_LT_impl, < )
	DEFINE_CHECK_OP_IMPL(check_GE_impl, >=)
	DEFINE_CHECK_OP_IMPL(check_GT_impl, > )
	#undef DEFINE_CHECK_OP_IMPL

	/*  GLOG trick: Function is overloaded for integral types to allow static const integrals
		declared in classes and not defined to be used as arguments to CHECK* macros. */
	template <class T>
	inline const T&           referenceable_value(const T&           t) { return t; }
	inline char               referenceable_value(char               t) { return t; }
	inline unsigned char      referenceable_value(unsigned char      t) { return t; }
	inline signed char        referenceable_value(signed char        t) { return t; }
	inline short              referenceable_value(short              t) { return t; }
	inline unsigned short     referenceable_value(unsigned short     t) { return t; }
	inline int                referenceable_value(int                t) { return t; }
	inline unsigned int       referenceable_value(unsigned int       t) { return t; }
	inline long               referenceable_value(long               t) { return t; }
	inline unsigned long      referenceable_value(unsigned long      t) { return t; }
	inline long long          referenceable_value(long long          t) { return t; }
	inline unsigned long long referenceable_value(unsigned long long t) { return t; }
} // namespace loguru

LOGURU_ANONYMOUS_NAMESPACE_END

// -----------------------------------------------
// Logging macros:

// usage:  LOG_STREAM(INFO) << "Foo " << std::setprecision(10) << some_value;
#define VLOG_IF_S(verbosity, cond)                                                                 \
	((verbosity) > loguru::current_verbosity_cutoff() || (cond) == false)                          \
		? (void)0                                                                                  \
		: loguru::Voidify() & loguru::StreamLogger(verbosity, __FILE__, __LINE__)
#define LOG_IF_S(verbosity_name, cond) VLOG_IF_S(loguru::Verbosity_ ## verbosity_name, cond)
#define VLOG_S(verbosity)              VLOG_IF_S(verbosity, true)
#define LOG_S(verbosity_name)          VLOG_S(loguru::Verbosity_ ## verbosity_name)

// -----------------------------------------------
// ABORT_S macro. Usage:  ABORT_S() << "Causo of error: " << details;

#define ABORT_S() loguru::Voidify() & loguru::AbortLogger("ABORT: ", __FILE__, __LINE__)

// -----------------------------------------------
// CHECK_S macros:

#define CHECK_WITH_INFO_S(cond, info)                                                              \
	LOGURU_PREDICT_TRUE((cond) == true)                                                            \
		? (void)0                                                                                  \
		: loguru::Voidify() & loguru::AbortLogger("CHECK FAILED:  " info "  ", __FILE__, __LINE__)

#define CHECK_S(cond) CHECK_WITH_INFO_S(cond, #cond)
#define CHECK_NOTNULL_S(x) CHECK_WITH_INFO_S((x) != nullptr, #x " != nullptr")

#define CHECK_OP_S(function_name, expr1, op, expr2)                                                \
	while (auto error_string = loguru::function_name(#expr1 " " #op " " #expr2,                    \
													 loguru::referenceable_value(expr1), #op,      \
													 loguru::referenceable_value(expr2)))          \
		loguru::AbortLogger(error_string->c_str(), __FILE__, __LINE__)

#define CHECK_EQ_S(expr1, expr2) CHECK_OP_S(check_EQ_impl, expr1, ==, expr2)
#define CHECK_NE_S(expr1, expr2) CHECK_OP_S(check_NE_impl, expr1, !=, expr2)
#define CHECK_LE_S(expr1, expr2) CHECK_OP_S(check_LE_impl, expr1, <=, expr2)
#define CHECK_LT_S(expr1, expr2) CHECK_OP_S(check_LT_impl, expr1, < , expr2)
#define CHECK_GE_S(expr1, expr2) CHECK_OP_S(check_GE_impl, expr1, >=, expr2)
#define CHECK_GT_S(expr1, expr2) CHECK_OP_S(check_GT_impl, expr1, > , expr2)

#if LOGURU_DEBUG_LOGGING
	// Debug logging enabled:
	#define DVLOG_IF_S(verbosity, cond)     VLOG_IF_S(verbosity, cond)
	#define DLOG_IF_S(verbosity_name, cond) LOG_IF_S(verbosity_name, cond)
	#define DVLOG_S(verbosity)              VLOG_S(verbosity)
	#define DLOG_S(verbosity_name)          LOG_S(verbosity_name)
#else
	// Debug logging disabled:
	#define DVLOG_IF_S(verbosity, cond)                                                     \
		(true || (verbosity) > loguru::current_verbosity_cutoff() || (cond) == false)       \
			? (void)0                                                                       \
			: loguru::Voidify() & loguru::StreamLogger(verbosity, __FILE__, __LINE__)

	#define DLOG_IF_S(verbosity_name, cond) DVLOG_IF_S(loguru::Verbosity_ ## verbosity_name, cond)
	#define DVLOG_S(verbosity)              DVLOG_IF_S(verbosity, true)
	#define DLOG_S(verbosity_name)          DVLOG_S(loguru::Verbosity_ ## verbosity_name)
#endif

#if LOGURU_DEBUG_CHECKS
	// Debug checks enabled:
	#define DCHECK_S(cond)                  CHECK_S(cond)
	#define DCHECK_NOTNULL_S(x)             CHECK_NOTNULL_S(x)
	#define DCHECK_EQ_S(a, b)               CHECK_EQ_S(a, b)
	#define DCHECK_NE_S(a, b)               CHECK_NE_S(a, b)
	#define DCHECK_LT_S(a, b)               CHECK_LT_S(a, b)
	#define DCHECK_LE_S(a, b)               CHECK_LE_S(a, b)
	#define DCHECK_GT_S(a, b)               CHECK_GT_S(a, b)
	#define DCHECK_GE_S(a, b)               CHECK_GE_S(a, b)
#else
// Debug checks disabled:
	#define DCHECK_S(cond)                  CHECK_S(true || (cond))
	#define DCHECK_NOTNULL_S(x)             CHECK_S(true || (x) != nullptr)
	#define DCHECK_EQ_S(a, b)               CHECK_S(true || (a) == (b))
	#define DCHECK_NE_S(a, b)               CHECK_S(true || (a) != (b))
	#define DCHECK_LT_S(a, b)               CHECK_S(true || (a) <  (b))
	#define DCHECK_LE_S(a, b)               CHECK_S(true || (a) <= (b))
	#define DCHECK_GT_S(a, b)               CHECK_S(true || (a) >  (b))
	#define DCHECK_GE_S(a, b)               CHECK_S(true || (a) >= (b))
#endif

#if LOGURU_REPLACE_GLOG
	#undef LOG
	#undef VLOG
	#undef LOG_IF
	#undef VLOG_IF
	#undef CHECK
	#undef CHECK_NOTNULL
	#undef CHECK_EQ
	#undef CHECK_NE
	#undef CHECK_LT
	#undef CHECK_LE
	#undef CHECK_GT
	#undef CHECK_GE
	#undef DLOG
	#undef DVLOG
	#undef DLOG_IF
	#undef DVLOG_IF
	#undef DCHECK
	#undef DCHECK_NOTNULL
	#undef DCHECK_EQ
	#undef DCHECK_NE
	#undef DCHECK_LT
	#undef DCHECK_LE
	#undef DCHECK_GT
	#undef DCHECK_GE
	#undef VLOG_IS_ON

	#define LOG            LOG_S
	#define VLOG           VLOG_S
	#define LOG_IF         LOG_IF_S
	#define VLOG_IF        VLOG_IF_S
	#define CHECK(cond)    CHECK_S(!!(cond))
	#define CHECK_NOTNULL  CHECK_NOTNULL_S
	#define CHECK_EQ       CHECK_EQ_S
	#define CHECK_NE       CHECK_NE_S
	#define CHECK_LT       CHECK_LT_S
	#define CHECK_LE       CHECK_LE_S
	#define CHECK_GT       CHECK_GT_S
	#define CHECK_GE       CHECK_GE_S
	#define DLOG           DLOG_S
	#define DVLOG          DVLOG_S
	#define DLOG_IF        DLOG_IF_S
	#define DVLOG_IF       DVLOG_IF_S
	#define DCHECK         DCHECK_S
	#define DCHECK_NOTNULL DCHECK_NOTNULL_S
	#define DCHECK_EQ      DCHECK_EQ_S
	#define DCHECK_NE      DCHECK_NE_S
	#define DCHECK_LT      DCHECK_LT_S
	#define DCHECK_LE      DCHECK_LE_S
	#define DCHECK_GT      DCHECK_GT_S
	#define DCHECK_GE      DCHECK_GE_S
	#define VLOG_IS_ON(verbosity) ((verbosity) <= loguru::current_verbosity_cutoff())

#endif // LOGURU_REPLACE_GLOG

#endif // LOGURU_WITH_STREAMS

#endif // LOGURU_HAS_DECLARED_STREAMS_HEADER
