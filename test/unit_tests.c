#include "test.h"

int my_add(int a, int b);

int my_add(int a, int b) {
  return a + b;
}

TEST(example) {
  int a = 2;
  int b = 3;

  return !(a + b == my_add(a, b));
}

int main() {
  fl_run_tests();
}

