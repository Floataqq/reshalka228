/**
 * @file
 * @brief A testing library that works quite similarly to gtest
 */

#ifndef FLOATAQQ_TEST_LIB
#define FLOATAQQ_TEST_LIB


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FL_MAX_MSG 1024

typedef enum {
  FL_UNKNOWN,
  FL_SUCCESS,
  FL_FAIL,
  FL_EXCEPTION
} _FL_TestStatus;

typedef struct {
  _FL_TestStatus status;
  char message[1024];
} _FL_TestResult;

typedef struct {
  const char *name;
  void (*func)();

  _FL_TestResult res;
} _FL_Test;

extern _FL_Test *_fl_test_data;
extern size_t    _fl_test_count;
extern size_t    _fl_test_capacity;

// Used by asserts under the hood to understand, what test are they for.
extern thread_local size_t _fl_current_test_index;

// TODO: мега ассерт

int  _fl_add_test(const char test_name[], void (*test)());
void  fl_test_runner(int *failed_num);

/**
 * Run all the registered tests. Just call it from a separate main() and include all #TEST declarations
 */
[[noreturn]] void fl_run_tests();

// ------- TEST MACROS -------

/**
 * Define a test function like this:
 * ```c
 * TEST(example_test) {
 *   // any sort of setup code
 *   int a = 2, b = 3;
 *
 *   // sequence of assertions
 *   ASSERT_EQ(a + b, 5);
 * }
 * ```
 */
#define TEST(name)                                                    \
  void _FL_TEST_##name();                                             \
  int  _FL_TEST_RES_##name = _fl_add_test(#name, &_FL_TEST_##name);   \
  void _FL_TEST_##name()

#define SUCCESS() {                                                   \
    _fl_test_data[_fl_current_test_index].res.status = FL_SUCCESS;    \
  }

#define FAIL_WITH_MSG(...) {                                                        \
      _FL_TestResult *_fl_test_result = &_fl_test_data[_fl_current_test_index].res; \
      _fl_test_result->status = FL_FAIL;                                            \
      snprintf(_fl_test_result->message, FL_MAX_MSG, __VA_ARGS__);                  \
      return;                                                                       \
    }

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

#endif // FLOATAQQ_TEST_LIB
