#include <math.h>
#include <stdlib.h>

#include "test.h"
#include "equation.h"
#include "complex.h"

#define P(...) __VA_ARGS__
#define SOLVE_EQUATION_TEST(name, a, b, c, status, r1, r2) \
  TEST(name) {                                             \
    Equation eq = (Equation){ a, b, c };                   \
    compute_solutions(&eq);                                \
                                                           \
    ASSERT_EQ(eq.tag, status);                             \
    if (eq.tag == SINGLE) {                                \
      ASSERT_BOOL(cmplx_eq(eq.solutions[0], r1));          \
    } else if (eq.tag == DOUBLE) {                         \
      ASSERT_BOOL(cmplx_eq(eq.solutions[1], r2));          \
    }                                                      \
  }


SOLVE_EQUATION_TEST(solve_equation_three_zeros, 0,  0,  0, INFINITE, {}, {})
SOLVE_EQUATION_TEST(solve_equation_zero_a_b,    0,  0,  1, NONE,     {}, {})
SOLVE_EQUATION_TEST(solve_equation_zero_a_c,    0,  5,  0, SINGLE,   {0}, {})
SOLVE_EQUATION_TEST(solve_equation_zero_b_c,    4,  0,  0, SINGLE,   {0}, {})
SOLVE_EQUATION_TEST(solve_equation_zero_a,      0,  2, -1, SINGLE,   {0.5}, {})
SOLVE_EQUATION_TEST(solve_equation_zero_b,      1,  0, -1, DOUBLE,   {-1}, {1})
SOLVE_EQUATION_TEST(solve_equation_zero_c,      1, -1,  0, DOUBLE,   {0},  {1})

double rand_double();
double rand_double() {
  return (double)rand() / (double)(RAND_MAX);
}

double eval_equation(Equation eq, double x);
double eval_equation(Equation eq, double x) {
  return eq.a * x * x + eq.b * x + eq.c;
}


TEST(solve_equation_stress_test_10000) {
  for (int i = 0; i < 10000; i++) {
    Equation eq = { .a = rand_double(), .b = rand_double(), .c = -rand_double()};
    if (eq.b * eq.b - 4 * eq.a * eq.c < 0)
      continue;
    compute_solutions(&eq);
    
    double root_1 = NAN, root_2 = NAN;
    switch(eq.tag) {
      case DOUBLE:
        root_2 = eval_equation(eq, eq.solutions[1].real);
        ASSERT_BOOL_MSG(
          is_zero(root_2), 
          "The equation %lg*x^2 + %lg*x + %lg has value %lg in it's computed root %lg",
          eq.a, eq.b, eq.c, root_2, eq.solutions[1].real);
        [[fallthrough]];
      case SINGLE:
        root_1 = eval_equation(eq, eq.solutions[0].real);
        ASSERT_BOOL_MSG(
            is_zero(root_1), 
            "The equation %lg*x^2 + %lg*x + %lg has value %lg in it's computed root %lg",
            eq.a, eq.b, eq.c, root_1, eq.solutions[0].real);
        break;
      case NOT_COMPUTED:
        ASSERT_BOOL_MSG(0, "Ran into a NOT_COMPUTED status while solving equation");
      case INFINITE:
      case NONE:
      default:
        break;
    }
  }
}
