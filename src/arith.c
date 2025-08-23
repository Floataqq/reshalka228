/**
 * @file
 * @brief Arithmetic-related function definitions
 */

#include <math.h>

#include "equation.h"

int is_zero(const double x) {
  return fabs(x) < EPSILON;
}

int is_equal(const double x, const double y) {
  return fabs(x - y) < EPSILON;
}

double normalize_zero(const double x) {
  return is_zero(x) ? 0 : x;
}

/* a == 0 case */
void solve_linear_equation(Equation *const eq) {
  const double b = eq->b, c = eq->c;
  const int is_zero_b = is_zero(b);
  const int is_zero_c = is_zero(c);

  if (!is_zero_b && is_zero_c) {
    eq->tag = SINGLE;
    eq->solutions[0] = 0;
  } else if (!is_zero_c && is_zero_b) {
    eq->tag = NONE;
  } else if (is_zero_b && is_zero_c) {
    eq->tag = INFINITE;
  } else {
    eq->tag = SINGLE;
    eq->solutions[0] = normalize_zero(-c / b);
  }
}

/* a != 0 case */
void solve_quadratic_equation(Equation *const eq) {
  const double a = eq->a, b = eq->b, c = eq->c;
  const double d = b*b - 4 * a*c;

  if (is_zero(d)) {
    eq->tag = SINGLE;
    eq->solutions[0] = normalize_zero(-b / (2 * a));
  } else if (d > 0) {
    double d_sqrt = sqrt(d);
    eq->tag = DOUBLE;
    eq->solutions[0] = normalize_zero((-b + d_sqrt) / (2 * a));
    eq->solutions[1] = normalize_zero((-b - d_sqrt) / (2 * a));
  } else {
    eq->tag = NONE;
  }
}

void compute_solutions(Equation *const eq) {
  if (is_zero(eq->a))
    solve_linear_equation(eq);
  else
    solve_quadratic_equation(eq);
}


