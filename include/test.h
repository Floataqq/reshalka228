#ifndef FLOATAQQ_TEST_LIB
#define FLOATAQQ_TEST_LIB


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------- PROTOTYPES -------

typedef struct {
  const char *name;
  uint32_t (*func)();
} _FL_Test;

_FL_Test *_fl_test_data = {};
size_t    _fl_test_count = 0;
uint32_t  _fl_status = 0;

uint32_t _fl_test_add_test(const char test_name[], uint32_t (*test)());
void _fl_run_test(_FL_Test test);
void fl_run_tests();

// ------- TEST MACROS -------

#define TEST(name) uint32_t _FL_TEST_##name();                                                  \
                   uint32_t _FL_TEST_RES_##name = _fl_test_add_test(#name, &_FL_TEST_##name);   \
                   uint32_t _FL_TEST_##name()

// ------- INFRASTRUCTURE -------

uint32_t _fl_test_add_test(const char test_name[], uint32_t (*test)()) {
  _fl_test_count++;
  _fl_test_data = (_FL_Test *)realloc(_fl_test_data, _fl_test_count * sizeof(_FL_Test));
  _fl_test_data[_fl_test_count - 1] = (_FL_Test) {
    .name = test_name,
    .func = test
  };
  return 0;
}

// ------- RUNNERS -------

void _fl_run_test(_FL_Test test) {
  if (!test.func) {
    printf("Error: could not load test %s\n", test.name);
    exit(-1);
  }

  uint32_t res = test.func();
  if (res == 0) {
    printf("[+] %s: PASS\n", test.name);
  } else {
    printf("[+] %s: FAIL\n", test.name);
  }
}

/**
 * Run all the registered tests. Just call it from a separate main() and include all TEST* declarations
 */
void fl_run_tests() {
  printf("Running tests...\n");
  for (size_t curr = 0; curr < _fl_test_count; curr++) {
    _fl_run_test(_fl_test_data[curr]);
  }
}

#endif // FLOATAQQ_TEST_LIB
