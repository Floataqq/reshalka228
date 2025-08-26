#ifndef LIB_FLOATAQQ_POLYNOMIAL
#define LIB_FLOATAQQ_POLYNOMIAL


#include "complex.h"

typedef struct {
  char var;

  union {
    struct { double a, b, c, d, e; };
    double coeffs[5];
  };

} Polynomial;

extern const int INFINITE_SOLUTIONS;

typedef struct {
  int count;  

  union {
    struct { complex_t x1, x2, x3, x4; };
    complex_t x[5];
  };
} Solutions;

Solutions compute_solutions(Polynomial p);


#endif // LIB_FLOATAQQ_POLYNOMIAL
