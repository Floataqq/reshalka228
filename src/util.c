/**
 * @file
 * @brief Utility function definitions
 */

#include <stdlib.h>

#include "lib.h"

[[noreturn]] void abort_with_ndebug(void) {
  #ifndef NDEBUG
    exit(-1);
  #endif
}
