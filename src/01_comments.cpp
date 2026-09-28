#include "loguru.hpp"

int main(int argc, char* argv[]) {
  loguru::init(argc, argv);
  /* 这是注释 */

  /* C++ 注释也可以
   * 跨行
   */
  LOG_F(INFO, "Info Hello World!");
  LOG_F(WARNING, "Warning Hello World!");
  LOG_F(ERROR, "Error Hello World!");
  LOG_F(FATAL, "Fatal Hello World!");
  return 0;
}
