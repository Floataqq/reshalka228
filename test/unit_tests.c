#include "test.h"

int my_add(int a, int b);

int my_add(int a, int b) {
  return a + b;
}

TEST(example) {
  ASSERT_EQ(2, 3);
}

int main() {
  fl_run_tests();
}

