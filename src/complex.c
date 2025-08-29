/**
 * @file
 * @brief A data structure for complex numbers
 */

#include <cmath>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "equation.h"
#include "complex.h"

const complex_t CMPLX_I = { .real = 0, .imag = 1 };

double cmplx_arg (const complex_t a) {
  return atan2(a.imag, a.real);
}

double cmplx_mag (const complex_t a) {
  return sqrt(a.imag * a.imag + a.real * a.real);
}

complex_t cmplx_add (const complex_t a, const complex_t b) {
  return { a.real + b.real, a.imag + b.imag };
}

complex_t cmplx_sub (const complex_t a, const complex_t b) {
  return { a.real - b.real, a.imag - b.imag };
}

complex_t cmplx_mul (const complex_t a, const complex_t b) {
 return {
   a.real * b.real - a.imag * b.imag,
   a.real * b.imag + a.imag * b.real,
 };
}

complex_t cmplx_div (const complex_t a, const complex_t b) {
  double b_mag = cmplx_mag(b) * cmplx_mag(b);
  assert(!is_zero(b_mag));

  return {
    (a.real * b.real + a.imag * b.imag) / b_mag,
    (a.imag * b.real - a.real * b.imag) / b_mag
  };
}

complex_t cmplx_pow (const complex_t a, const int b) {
  assert(b >= 0);
  complex_t res = {1};

  for (int i = 0; i < b; i++)
    res = cmplx_mul(res, a);
  return res;
}

complex_t cmplx_sqrt (const complex_t a) {
  if (is_zero(a.imag)) {
    if (a.real > 0)
      return {sqrt(a.real)};
    else
      return {0, sqrt(-a.real)};
  } else {
    double mag   = cmplx_mag(a);
    double left  = sqrt((mag + a.real) / 2);
    double right = sqrt((mag - a.imag) / 2);
    return {left, a.imag / fabs(a.imag) * right };
  }
}

complex_t cmplx_cbrt (const complex_t a, int k) {
  assert(0 <= k && k <= 2);

  double length = cmplx_mag(a);
  double angle  = cmplx_arg(a);

  double length_cbrt = cbrt(length);

  angle = (angle + 2 * M_PI * k) / 3.0;
  return {length_cbrt * cos(angle), length_cbrt * sin(angle) };
}

complex_t cmplx_negate (const complex_t a) {
  return { -a.real, -a.imag };
}

complex_t cmplx_normalize_zero (const complex_t a) {
  return {normalize_zero(a.real), normalize_zero(a.imag) };
}

bool cmplx_eq (const complex_t a, const complex_t b) {
  return is_equal(a.real, b.real) && is_equal(a.imag, b.imag);
}

bool cmplx_is_zero (const complex_t a) {
  return is_zero(a.real) && is_zero(a.imag);
}

void print_complex (const complex_t x) {
  if (!is_zero(x.real) && !is_zero(x.imag)) {
    printf("(%lg ", x.real);

    if (x.imag > 0) {
      putchar('+');
      printf(" %lgi)", x.imag);
    } else {
      putchar('-');
      printf(" %lgi)", fabs(x.imag));
    }
  } else if (!is_zero(x.real))
    printf("%lg", x.real);
  else if (!is_zero(x.imag))
    printf("%lgi", x.imag);
  else
    printf("0");
}

