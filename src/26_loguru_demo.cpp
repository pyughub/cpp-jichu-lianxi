#include "loguru.hpp"

// LOG_F(FATAL, "...") 会在打印后中止进程，所以这里不调用。

int main(int argc, char* argv[]) {
    loguru::init(argc, argv);
    LOG_F(INFO, "Info Hello World!");
    LOG_F(WARNING, "Warning Hello World!");
    LOG_F(ERROR, "Error Hello World!");
    return 0;
}
