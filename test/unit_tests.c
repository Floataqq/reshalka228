#include <cstdio>
#include <math.h>
#include <stdlib.h>

#include "test.h"
#include "equation.h"

#define SOLVE_EQUATION_TEST_0(name, a, b, c, expected) \
  TEST(name) {                                         \
    Equation eq = (Equation){ a, b, c };               \
    compute_solutions(&eq);                            \
                                                       \
    ASSERT_EQ(eq.tag, expected);                       \
  }

#define SOLVE_EQUATION_TEST_1(name, a, b, c, r1) \
  TEST(name) {                                   \
    Equation eq = (Equation){ a, b, c };         \
    compute_solutions(&eq);                      \
                                                 \
    ASSERT_EQ(eq.tag, SINGLE);                   \
    ASSERT_BOOL(is_equal(eq.solutions[0], r1));  \
  }

#define SOLVE_EQUATION_TEST_2(name, a, b, c, r1, r2) \
  TEST(name) {                                       \
    Equation eq = (Equation){ a, b, c };             \
    compute_solutions(&eq);                          \
                                                     \
    ASSERT_EQ(eq.tag, DOUBLE);                       \
    ASSERT_BOOL(is_equal(eq.solutions[0], r1));      \
    ASSERT_BOOL(is_equal(eq.solutions[1], r2));      \
  }

SOLVE_EQUATION_TEST_0(solve_equation_three_zeros, 0,  0,  0, INFINITE)
SOLVE_EQUATION_TEST_0(solve_equation_zero_a_b,    0,  0,  1, NONE)
SOLVE_EQUATION_TEST_1(solve_equation_zero_a_c,    0,  5,  0, 0)
SOLVE_EQUATION_TEST_1(solve_equation_zero_b_c,    4,  0,  0, 0)
SOLVE_EQUATION_TEST_1(solve_equation_zero_a,      0,  2, -1, 0.5)
SOLVE_EQUATION_TEST_2(solve_equation_zero_b,      1,  0, -1, 1, -1)
SOLVE_EQUATION_TEST_2(solve_equation_zero_c,      1, -1,  0, 1,  0)

double rand_double();
double eval_equation(Equation eq, double x);

TEST(solve_equation_stress_test_10000) {
  for (int i = 0; i < 10000; i++) {
    Equation eq = { .a = rand_double(), .b = rand_double(), .c = -rand_double()};
    compute_solutions(&eq);
    
    double root_1 = NAN, root_2 = NAN;
    switch(eq.tag) {
      case DOUBLE:
        root_2 = eval_equation(eq, eq.solutions[1]);
        ASSERT_BOOL_MSG(
          is_zero(root_2), 
          "The equation %lg*x^2 + %lg*x + %lg has value %lg in it's computed root %lg",
          eq.a, eq.b, eq.c, root_2, eq.solutions[1]);
        [[fallthrough]];
      case SINGLE:
        root_1 = eval_equation(eq, eq.solutions[0]);
        ASSERT_BOOL_MSG(
            is_zero(root_1), 
            "The equation %lg*x^2 + %lg*x + %lg has value %lg in it's computed root %lg",
            eq.a, eq.b, eq.c, root_1, eq.solutions[0]);
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

int main() {
  fl_run_tests();
}

double rand_double() {
  return (double)rand() / (double)(RAND_MAX);
}

double eval_equation(Equation eq, double x) {
  return eq.a * x * x + eq.b * x + eq.c;
}

