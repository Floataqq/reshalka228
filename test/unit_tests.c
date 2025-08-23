#include "test.h"
#include "log.h"

int my_add(int a, int b);

int my_add(int a, int b) {
  return a + b;
}

TEST(example) {
  ASSERT_EQ(2, 3);
}

int main() {
  LOG_DEBUG ("Meow!");
  LOG_INFO  ("Meow! format: %d", 1231);
  LOG_WARN  ("Meow!");
  LOG_ERROR ("Meow!");

  fl_run_tests();
}

