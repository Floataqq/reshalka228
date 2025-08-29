/**
 * @file
 * @brief A library with a polynomial data structure
 */

#ifndef LIB_POLYNOMIAL
#define LIB_POLYNOMIAL


#include "complex.h"

/// Length of the coeffs array of the #Polynomial
#define POLY_COEFF_LEN 5
/// The maximum supported (mathematical) degree of a #Polynomial. Equal to #POLY_COEFF_LEN - 1
#define POLY_MAX_DEG   4

/// Data structure for a polynomial
typedef struct {
  /// The variable name, e.g. x or y
  char var;

  union {
    /// Address polynomial coefficients from a to e. #Polynomial.a is the highest
    /// coeff, while #Polynomial.e is the lowest
    struct { complex_t e, d, c, b, a; };

    /// Address polynomial coefficients by index. 0 is the lowest coeff,
    /// #POLY_COEFF_LEN is the highest
    complex_t coeffs[5];
  };

} Polynomial;

/// A constant that means that a polynomial has an infinite number of solutions
extern const int INFINITE_SOLUTIONS;

/// A struct for holding the solutions of a polynomial
typedef struct {
  /// Number of solutions from 0 to 4, or #INFINITE_SOLUTIONS
  int count;

  union {
    /// Address solutions by x[1-4]
    struct { complex_t x1, x2, x3, x4; };
    /// Address solutions by index
    complex_t x[4];
  };
} Solutions;

/**
 * Output a provided #Soltuions to stdout nicely.
 *
 * @param sols #Solutions to output
 */
void print_solutions (Solutions sols);

/// Errors that can arise when doing #Polynomial arithmetic
typedef enum {
  /// Everything was computed correctly
  POLY_OK,
  /// Encountered a polynomial with a degree larger than #POLY_MAX_DEG
  POLY_TOO_LARGE,
  /// Tried to operate on polynomials with different var names.
  POLY_DIFFERENT_VAR,
} PolynomialError;

/**
 * Get the mathematical degree of \p a. If the polynomial has **all**
 * of it's coefficients zeroed out, return `-1`.
 */
int polynomial_deg(Polynomial a);

/**
 * Compute a negated polynomial
 *
 * @param a      Polynomial to negate
 * @param output Where to write the output
 *
 * @returns Whether an error was encountered
 */
PolynomialError polynomial_negate (Polynomial a,               Polynomial *output);

/**
 * Compute sum of two polynomials
 *
 * @param a      First operand
 * @param b      Second operand
 * @param output Where to write the output
 *
 * @returns Whether an error was encountered
 */
PolynomialError polynomial_add    (Polynomial a, Polynomial b, Polynomial *output);

/**
 * Compute difference of two polynomials
 *
 * @param a      First operand
 * @param b      Second operand
 * @param output Where to write the output
 *
 * @returns Whether an error was encountered
 */
PolynomialError polynomial_sub    (Polynomial a, Polynomial b, Polynomial *output);

/**
 * Compute product of two polynomials
 *
 * @param a      First operand
 * @param b      Second operand
 * @param output Where to write the output
 *
 * @returns Whether an error was encountered
 */
PolynomialError polynomial_mul    (Polynomial a, Polynomial b, Polynomial *output);

/**
 * Evaluate a polynomial at a point
 *
 * @param p The polnomial to evaluate
 * @param v The point to evaluate \p p at
 *
 * @returns The value of p in point v
 */
complex_t polynomial_eval (Polynomial p, complex_t v);

/**
 * Print a polynomial to stdout
 *
 * @param p The polynomial to describe
 */
void print_polynomial (Polynomial p);

#endif // LIB_POLYNOMIAL



