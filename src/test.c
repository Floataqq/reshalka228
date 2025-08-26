/**
 * @file
 * @brief Testing facilities
 */

#include <iterator>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "log.h"

_FL_Test           *_fl_test_data = {};
size_t              _fl_test_count = 0;
size_t              _fl_test_capacity = 0;
size_t thread_local _fl_current_test_index = 0;

int _fl_add_test(const char test_name[], void (*test)()) {
  if (_fl_test_count == _fl_test_capacity) {
    _fl_test_capacity += 1024;
    _fl_test_data = (_FL_Test *)realloc(_fl_test_data, _fl_test_capacity * sizeof(_FL_Test));
  }
  _fl_test_count++;
  _fl_test_data[_fl_test_count - 1] = (_FL_Test) {
    .name = test_name,
    .func = test,
    .res = (_FL_TestResult) {
      .status = FL_UNKNOWN,
      .message = {}
    }
  };
  return 0;
}

// ------- RUNNERS -------

void fl_test_runner(int *failed_num) {
  _FL_Test *test = &_fl_test_data[_fl_current_test_index];
  if (!test->func) {
    LOG_ERROR("Error: could not load test %s\n", test->name);
    exit(-1);
  }

  test->func();

  switch(test->res.status) {
  case FL_SUCCESS:
    LOG_INFO("[+] %s: PASS", test->name);
    break;
  case FL_FAIL:
  case FL_EXCEPTION:
    LOG_ERROR("[-] %s: FAIL", test->name);
    *failed_num += 1;
    break;
  case FL_UNKNOWN:
    LOG_ERROR("[?] %s: UNKNOWN. Something broke inside the library!", test->name);
    break;
  default:
    LOG_ERROR("[?] %s: unknown status value: %d", test->name, test->res.status);
  }

  if (test->res.message[0])
    LOG_DEBUG("  msg: '%s'", test->res.message);
}

[[noreturn]] void fl_run_tests() {
  printf("Running tests...\n");

  int failed_num = 0;

  for (_fl_current_test_index = 0; _fl_current_test_index < _fl_test_count; _fl_current_test_index++) {
    fl_test_runner(&failed_num);
  }

  // Throws number of failed tests as an exit code
  exit(failed_num);
}
