#ifndef FLOATAQQ_TEST_LIB
#define FLOATAQQ_TEST_LIB


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------- PROTOTYPES -------

#define FL_MAX_MSG 1024

typedef enum { 
  FL_UNKNOWN, 
  FL_SUCCESS, 
  FL_FAIL, 
  FL_EXCEPTION 
} _FL_TestStatus;

typedef struct {
  _FL_TestStatus status;
  char *message;
} _FL_TestResult;

typedef struct {
  const char *name;
  void (*func)(_FL_TestResult *);

  _FL_TestResult res;
} _FL_Test;

_FL_Test *_fl_test_data = {};
size_t    _fl_test_count = 0;
uint32_t  _fl_status = 0;

uint32_t _fl_test_add_test(const char test_name[], void (*test)(_FL_TestResult*));
void _fl_run_test(_FL_Test test);
void fl_run_tests();

// ------- TEST MACROS -------

/**
 * Define a test function like this:
 * ```c
 * TEST(example_test) {
 *   // any sort of setup code
 *   int a = 2, b = 3;
 *   
 *   // you can use _fl_test_result parameter to modify 
 *      the test result by hand
 *   _fl_test_result->status = FL_FAIL;
 *
 *   // sequence of assertions
 *   ASSERT_EQ(a + b, 5);
 * }
 * ```
 */
#define TEST(name) void     _FL_TEST_##name(_FL_TestResult *_fl_test_result);                        \
                   uint32_t _FL_TEST_RES_##name = 0 || _fl_test_add_test(#name, &_FL_TEST_##name);   \
                   void     _FL_TEST_##name(_FL_TestResult *_fl_test_result)

#define SUCCESS() { _fl_test_result->status = FL_SUCCESS; }
#define FAIL_WITH_MSG(...) {                                 \
      _fl_test_result->status = FL_FAIL;                     \
      char *message = (char *) malloc(FL_MAX_MSG);           \
      /* freed after running tests */                        \
      _fl_test_result->message = message;                    \
      sprintf(message, __VA_ARGS__);                         \
      return;                                                \
    }                                                        \

#define ASSERT_BOOL_MSG(x, ...)  {      \
    if (x) {                            \
      SUCCESS();                        \
    } else {                            \
      FAIL_WITH_MSG(__VA_ARGS__);       \
    }                                   \
  }

#define ASSERT_BOOL(x)  ASSERT_BOOL_MSG(x,  "Assertion failed: `%s` evaluates to false", #x)
#define ASSERT_EQ(x, y) ASSERT_BOOL_MSG(x == y, "Assertion failed: `%s` != `%s`", #x, #y)
#define ASSERT_NE(x, y) ASSERT_BOOL_MSG(x != y, "Assertion failed: `%s` == `%s`", #x, #y)

// ------- INFRASTRUCTURE -------

uint32_t _fl_test_add_test(const char test_name[], void (*test)(_FL_TestResult *)) {
  _fl_test_count++;
  _fl_test_data = (_FL_Test *)realloc(_fl_test_data, _fl_test_count * sizeof(_FL_Test));
  _fl_test_data[_fl_test_count - 1] = (_FL_Test) {
    .name = test_name,
    .func = test,
    .res = (_FL_TestResult) {
      .status = FL_UNKNOWN,
      .message = NULL
    }
  };
  return 0;
}

// ------- RUNNERS -------

void _fl_run_test(_FL_Test test) {
  if (!test.func) {
    printf("Error: could not load test %s\n", test.name);
    exit(-1);
  }

  test.func(&test.res);
  
  switch(test.res.status) {
  case FL_SUCCESS:
    printf("[+] %s: PASS\n", test.name);
    break;
  case FL_FAIL:
  case FL_EXCEPTION:
    printf("[-] %s: FAIL\n", test.name);
    break;
  case FL_UNKNOWN:
    printf("[?] %s: UNKNOWN. Something broke inside the library!\n", test.name);
    break;
  default:
    printf("[?] %s: unknown status value: %d\n", test.name, test.res.status);
  }

  if (test.res.message) {
    printf("  msg: '%s'\n", test.res.message);
    free(test.res.message);
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
