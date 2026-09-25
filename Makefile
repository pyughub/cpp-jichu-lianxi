# 用法（macOS / Linux 用 make，Windows MinGW 用 mingw32-make）：
#   make                     编译 src/ 下全部 .cpp
#   make 01_comments.cpp     只编译这一个文件
#   make run 01_comments.cpp 编译（如有需要）并运行
#   make clean               删除 bin/ 和 dsym/

CXX      = c++
CXXFLAGS = -std=c++17 -g -Wall
SRC_DIR  = src
BIN_DIR  = bin
DSYM_DIR = dsym

# Windows 生成 foo.exe；macOS / Linux 生成不带后缀的 foo
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RUN =
    UNAME_S =
else
    EXE =
    RUN = ./
    UNAME_S := $(shell uname -s)
endif

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
BINS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%$(EXE),$(SRCS))

# 命令行里的那个 .cpp（允许写成 01_comments.cpp 或 src/01_comments.cpp）
SRC = $(notdir $(filter %.cpp,$(MAKECMDGOALS)))
BIN = $(BIN_DIR)/$(SRC:.cpp=$(EXE))

.PHONY: all run clean $(BIN_DIR) $(DSYM_DIR)

all: $(BINS)

$(BIN_DIR) $(DSYM_DIR):
	mkdir -p $@

define MOVE_DSYM
	@if [ "$(UNAME_S)" = "Darwin" ] && [ -d $@.dSYM ]; then \
		rm -rf $(DSYM_DIR)/$(notdir $@).dSYM; \
		mv $@.dSYM $(DSYM_DIR)/; \
	fi
endef

$(BIN_DIR)/%$(EXE): $(SRC_DIR)/%.cpp | $(BIN_DIR) $(DSYM_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<
	$(MOVE_DSYM)

ifneq ($(SRC),)
.PHONY: $(filter %.cpp,$(MAKECMDGOALS))
ifeq ($(filter run,$(MAKECMDGOALS)),)
$(filter %.cpp,$(MAKECMDGOALS)):
	$(MAKE) $(BIN)
else
$(filter %.cpp,$(MAKECMDGOALS)):
	@:
endif
endif

run:
ifeq ($(SRC),)
	$(error 用法: make run <file.cpp>)
endif
	$(MAKE) $(BIN)
	$(RUN)$(BIN)

clean:
	rm -rf $(BIN_DIR) $(DSYM_DIR)
