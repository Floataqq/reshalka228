/**
 * @file
 * @brief Arithmetic-related function definitions
 */

#include <cmath>
#include <complex>
#include <math.h>

#include "equation.h"
#include "complex.h"
#include "polynomial.h"
#include "arith.h"
#include "log.h"

int is_zero (const double x) {
  return fabs(x) < EPSILON;
}

int is_equal (const double x, const double y) {
  return fabs(x - y) < EPSILON;
}

double normalize_zero (const double x) {
  return is_zero(x) ? 0 : x;
}

int solve_degree_2 (Polynomial p, Solutions *sols);

int solve_degree_3 (Polynomial p, Solutions *sols);
int solve_depressed_qubic (Polynomial poly, complex_t p, complex_t q, Solutions *sols);
complex_t cardano_unsubstitute (Polynomial poly, const complex_t y);

int solve_degree_4 (Polynomial p, Solutions *sols);

int solve_polynomial (Polynomial p, Solutions *sols) {
  int deg = polynomial_deg(p);
  if (deg == -1) {
    // 0 = 0
    sols->count = INFINITE_SOLUTIONS;
    return 1;
  } else if (deg == 0) {
    // e = 0
    sols->count = 0;
    return 1;
  } else if (deg == 1) {
    // dx + e = 0
    // x = -e / d
    sols->count = 1;
    sols->x1 = cmplx_normalize_zero(
      cmplx_div(cmplx_negate(p.e), p.d)
    );
    return 1;
  } else if (deg == 2) {
    return solve_degree_2(p, sols);
  } else if (deg == 3) {
    return solve_degree_3(p, sols);
  }

  return 0;
}

int solve_degree_2 (Polynomial p, Solutions *sols) {
  // cx^2 + dx + e = 0

  // d = d^2 - 4ce
  complex_t d = cmplx_sub(
    cmplx_mul(p.d, p.d),
    cmplx_mul({4}, cmplx_mul(p.c, p.e))
  );
  complex_t d_sqrt = cmplx_sqrt(d);

  LOG_DEBUG(
    "d = %lg + %lgi, d_sqrt = %lg + %lgi",
    d.real, d.imag, d_sqrt.real, d_sqrt.imag
  );
  
  // x1 = (-d - sqrt(discr)) / (2 * a)
  complex_t x1 = cmplx_normalize_zero(cmplx_div(
    cmplx_sub(cmplx_negate(p.d), d_sqrt),
    cmplx_mul(p.c, {2})
  ));
  // x2 = (-d + sqrt(discr)) / (2 * a)
  complex_t x2 = cmplx_normalize_zero(cmplx_div(
    cmplx_add(cmplx_negate(p.d), d_sqrt),
    cmplx_mul(p.c, {2})
  ));
  
  if (cmplx_eq(x1, x2)) {
    sols->count = 1;
    sols->x1 = x1;
  } else {
    sols->count = 2;
    sols->x1 = x1;
    sols->x2 = x2;
  }

  return 1;
}

int solve_degree_3 (Polynomial p, Solutions *sols) {
  // bx^3 + cx^2 + dx + e = 0
  // ax^3 + bx^2 + cx + d = 0
  
  // 3 * a * c
  complex_t three_a_c = cmplx_mul({3}, cmplx_mul(p.b, p.d));
  // b * b
  complex_t b_squared = cmplx_pow(p.c, 2);
  // 3 * a * a
  complex_t three_a_a = cmplx_mul({3}, cmplx_pow(p.b, 2));

  complex_t depressed_p = cmplx_div(
    cmplx_sub(three_a_c, b_squared),
    three_a_a
  );
  
  // 2 * b * b * b
  complex_t two_b_cubed    = cmplx_mul({2}, cmplx_pow(p.c, 3));
  // 9 * a * b * c
  complex_t nine_a_b_c     = cmplx_mul({9}, cmplx_mul(p.b, cmplx_mul(p.c, p.d)));
  // 27 * a * a * d
  complex_t ts_a_squared_d = cmplx_mul({27}, cmplx_mul(p.e, cmplx_pow(p.b, 2)));
  // 27 * a * a * a
  complex_t tw_a_cubed     = cmplx_mul({27}, cmplx_pow(p.b, 3));

  complex_t depressed_q = cmplx_div(
    cmplx_add(cmplx_sub(two_b_cubed, nine_a_b_c), ts_a_squared_d),
    tw_a_cubed
  );

  LOG_DEBUG("calculated depressed cubic: p = %lg + %lgi, q = %lg + %lgi",
      depressed_p.real, depressed_p.imag, depressed_q.real, depressed_q.imag);

  return solve_depressed_qubic(p, depressed_p, depressed_q, sols);
}

int solve_depressed_qubic (Polynomial poly, complex_t p, complex_t q, Solutions *sols) {
  if (is_zero(cmplx_mag(p))) {
    sols->count = 1;
    // y = cbrt(-q)
    complex_t y = cmplx_cbrt(cmplx_negate(q), 0);
    
    // x1 = y - b/(3*a)
    sols->x1 = cardano_unsubstitute(poly, y);
    return 1;
  }
  
  complex_t half_q_squared = cmplx_pow(cmplx_div(q, {2}), 2);
  complex_t third_p_cubed  = cmplx_pow(cmplx_div(p, {3}), 3);
  
  // disc = (q/2)^2 + (p/3)^3
  complex_t disc = cmplx_add(half_q_squared, third_p_cubed);
  LOG_DEBUG("discriminant: %lg + %lgi", disc.real, disc.imag);
  
  if (is_zero(cmplx_mag(disc))) {
    LOG_DEBUG("two real roots!");

    sols->count = 2;
    
    complex_t y2 = cmplx_cbrt(
      cmplx_div(q, {2}),
      1
    );
    complex_t y1 = cmplx_mul({-2}, y2);
    
    sols->x1 = cardano_unsubstitute(poly, y1);
    sols->x2 = cardano_unsubstitute(poly, y2);

    return 1;
  }

  return 0;
}

complex_t cardano_unsubstitute (Polynomial poly, const complex_t y) {
  // y - b / (3 * a)
  return cmplx_sub(y, cmplx_div(poly.c, cmplx_mul({3}, poly.b)));
}

/* a == 0 case */
void solve_linear_equation (Equation *const eq) {
  const double b = eq->b, c = eq->c;
  const int is_zero_b = is_zero(b);
  const int is_zero_c = is_zero(c);

  if (!is_zero_b && is_zero_c) {
    eq->tag = SINGLE;
    eq->solutions[0] = {0};
  } else if (!is_zero_c && is_zero_b) {
    eq->tag = NONE;
  } else if (is_zero_b && is_zero_c) {
    eq->tag = INFINITE;
  } else {
    eq->tag = SINGLE;
    eq->solutions[0] = {normalize_zero(-c / b)};
  }
}

/* a != 0 case */
void solve_quadratic_equation (Equation *const eq) {
  const double a = eq->a, b = eq->b, c = eq->c;
  const double d = b*b - 4 * a*c;

  if (is_zero(d)) {
    eq->tag = SINGLE;
    eq->solutions[0] = {normalize_zero(-b / (2 * a))};
  } else {
    complex_t d_sqrt = cmplx_sqrt({d});
    
    eq->tag = DOUBLE;
    eq->solutions[0] = cmplx_normalize_zero(cmplx_div(
      cmplx_sub({-b}, d_sqrt),
      cmplx_mul({a}, {2})
    ));
    eq->solutions[1] = cmplx_normalize_zero(cmplx_div(
      cmplx_add({-b}, d_sqrt),
      cmplx_mul({a}, {2})
    ));
  }
}

void compute_solutions (Equation *const eq) {
  if (is_zero(eq->a))
    solve_linear_equation(eq);
  else
    solve_quadratic_equation(eq);
}

