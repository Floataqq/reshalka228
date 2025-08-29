/**
 * @file
 * @brief Polynomial operations
 */

#include <assert.h>
#include <stdio.h>

#include "polynomial.h"
#include "complex.h"
#include "log.h"

const int INFINITE_SOLUTIONS = -1;

int polynomial_deg (Polynomial p) {
  int res = -1;
  for (int i = 0; i < POLY_COEFF_LEN; i++)
    if (!cmplx_is_zero(p.coeffs[i]))
      res = i;

  return res;
}

PolynomialError polynomial_negate (Polynomial a, Polynomial *output) {
  *output = a;
  for (int i = 0; i < POLY_COEFF_LEN; i++)
    output->coeffs[i] = cmplx_negate(output->coeffs[i]);

  return POLY_OK;
}

PolynomialError polynomial_add (Polynomial a, Polynomial b, Polynomial *output) {
  if (a.var != b.var) {
    LOG_DEBUG("Encountered polynomials with different variables - %c and %c", a.var, b.var);
    return POLY_DIFFERENT_VAR;
  }

  Polynomial res = { .var = a.var };

  for (int i = 0; i < POLY_COEFF_LEN; i++)
    res.coeffs[i] = cmplx_add(a.coeffs[i], b.coeffs[i]);

  *output = res;
  return POLY_OK;
}

PolynomialError polynomial_sub (Polynomial a, Polynomial b, Polynomial *output) {
  if (a.var != b.var) {
    LOG_DEBUG("Encountered polynomials with different variables - %c and %c", a.var, b.var);
    return POLY_DIFFERENT_VAR;
  }

  Polynomial res = { .var = a.var };

  for (int i = 0; i < POLY_COEFF_LEN; i++)
    res.coeffs[i] = cmplx_sub(a.coeffs[i], b.coeffs[i]);

  *output = res;
  return POLY_OK;
}

PolynomialError polynomial_mul (Polynomial a, Polynomial b, Polynomial *output) {
  if (a.var != b.var) {
    LOG_DEBUG("Encountered polynomials with different variables - %c and %c", a.var, b.var);
    return POLY_DIFFERENT_VAR;
  }

  if (polynomial_deg(a) + polynomial_deg(b) > POLY_MAX_DEG)
    return POLY_TOO_LARGE;
  
  Polynomial res = { .var = a.var };

  for (int i = 0; i < POLY_COEFF_LEN; i++) {
    for (int j = 0; j < POLY_COEFF_LEN; j++) {
      if (i + j > POLY_MAX_DEG)
        continue;

      res.coeffs[i + j] = cmplx_add(
        cmplx_mul(a.coeffs[i], b.coeffs[j]), 
        res.coeffs[i + j]
      );
    }
  }

  *output = res;
  return POLY_OK;
}

complex_t polynomial_eval (Polynomial p, complex_t v) {
  complex_t res = {};

  for (int i = 0; i < POLY_COEFF_LEN; i++) {
    // res += c[i] * v ^ i
    res = cmplx_add(
      res,
      cmplx_mul(p.coeffs[i], cmplx_pow(v, i))
    );
  }

  return res;
}

void print_polynomial (Polynomial p) {
  int nonzero_cnt = 0;
  for (int i = 0; i < POLY_COEFF_LEN; i++)
    if (!cmplx_is_zero(p.coeffs[i]))
      nonzero_cnt++;

  for (int i = POLY_COEFF_LEN - 1; i >= 0; i--) {
    if (!cmplx_is_zero(p.coeffs[i])) {
      if (!i || !cmplx_eq(p.coeffs[i], {1})) { // always print a nonzero last coeff
        print_complex(p.coeffs[i]);
        if (i)
          printf("*");
      }
      
      if (i != 0) {
        printf("%c", p.var);
        if (i != 1)
          printf("^%d", i);
      }

      nonzero_cnt--;
      if (nonzero_cnt)
        printf(" + ");
    }
  }
}

void print_solutions (Solutions sols) {
  if (sols.count == INFINITE_SOLUTIONS) {
    printf("Infinite solutions\n");
    return;
  } 

  if (sols.count == 0) {
    printf("No solutions!\n");
    return;
  }
  
  printf("%d solutions!\n", sols.count);
  for (int i = 0; i < sols.count; i++) {
    printf("  - ");
    print_complex(sols.x[i]);
    putchar('\n');
  }
}

