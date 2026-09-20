# 用法（macOS / Linux 用 make，Windows MinGW 用 mingw32-make）：
#   make                     编译当前目录全部 .cpp
#   make 01_comments.cpp     只编译这一个文件
#   make run 01_comments.cpp 编译（如有需要）并运行
#   make clean               删除生成的可执行文件

CXX      = c++
CXXFLAGS = -std=c++17 -Wall

# Windows 生成 foo.exe；macOS / Linux 生成不带后缀的 foo
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RUN =
else
    EXE =
    RUN = ./
endif

# 当前目录全部 .cpp，以及对应的可执行文件
SRCS = $(wildcard *.cpp)
BINS = $(SRCS:.cpp=$(EXE))

# 命令行里的那个 .cpp（允许写成 01_comments.cpp、./01_comments.cpp、.\01_comments.cpp）
SRC = $(notdir $(filter %.cpp,$(MAKECMDGOALS)))
BIN = $(SRC:.cpp=$(EXE))

.PHONY: all run clean

# 只敲 make：编译全部
all: $(BINS)

# 通用规则：xx.cpp -> xx 或 xx.exe
%$(EXE): %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# make 01_comments.cpp
# .cpp 已经存在于磁盘上。如果不声明伪目标，make 会说 “is up to date” 然后什么都不做。
# 和 run 一起用时给一个空规则，避免出现 “Nothing to be done for xx.cpp”。
ifneq ($(SRC),)
.PHONY: $(filter %.cpp,$(MAKECMDGOALS))
ifeq ($(filter run,$(MAKECMDGOALS)),)
$(filter %.cpp,$(MAKECMDGOALS)):
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC)
else
$(filter %.cpp,$(MAKECMDGOALS)):
	@:
endif
endif

# make run 01_comments.cpp
# $(MAKE) 会再进一次 make（日志里的 [1]），去更新可执行文件
run:
ifeq ($(SRC),)
	$(error 用法: make run <file.cpp>)
endif
	$(MAKE) $(BIN)
	$(RUN)$(BIN)

clean:
	rm -f $(BINS)
