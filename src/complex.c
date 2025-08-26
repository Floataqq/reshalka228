#include "equation.h"
#include "complex.h"
#include <math.h>

const complex_t CMPLX_I = { .real = 0, .imag = 1 };

double cmplx_arg(const complex_t a) {
  return atan(a.imag / a.real);
}

double cmplx_mag(const complex_t a) {
  return sqrt(a.imag * a.imag + a.real * a.real);
}

complex_t cmplx_add(const complex_t a, const complex_t b) {
  return { a.real + b.real, a.imag + b.imag };
}

complex_t cmplx_sub(const complex_t a, const complex_t b) {
  return { a.real - b.real, a.imag - b.imag };
}

complex_t cmplx_mul(const complex_t a, const complex_t b) {
 return {
   a.real * b.real - a.imag * b.imag,
   a.real * b.imag + a.imag * b.real,
 };
}

complex_t cmplx_div(const complex_t a, const complex_t b) {
  double b_mag = cmplx_mag(b) * cmplx_mag(b);
  return {
    (a.real * b.real + a.imag * b.imag) / b_mag,
    (a.imag * b.real - a.real * b.imag) / b_mag
  };
}

complex_t cmplx_sqrt(const complex_t a) {
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

complex_t cmplx_normalize_zero(const complex_t a) {
  return {normalize_zero(a.real), normalize_zero(a.imag) };
}

bool cmplx_eq(const complex_t a, const complex_t b) {
  return is_equal(a.real, b.real) && is_equal(a.imag, b.imag);
}

