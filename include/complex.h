/**
 * @file
 * @brief Data structure for a complex number
 */

#ifndef LIB_COMPLEX
#define LIB_COMPLEX

/// A complex number consisting of two doubles
typedef struct {
  /// Real component
  double real;
  /// Imaginary component
  double imag;
} complex_t;

/**
 * Compute the magnitude of a given number
 *
 * @param a the number
 * @returns the number's magnitude
 */
double cmplx_mag (const complex_t a);

/**
 * Compute the argument of a given number
 *
 * @param a the number
 * @returns the number's argument
 */
double cmplx_arg (const complex_t a);

/**
 * Add two complex number together
 *
 * @param a first number
 * @param b second number
 *
 * @returns #a + #b
 */
complex_t cmplx_add (const complex_t a, const complex_t b);

/**
 * Substract a complex number from another one
 *
 * @param a first number
 * @param b second number
 *
 * @returns #a - #b
 */
complex_t cmplx_sub (const complex_t a, const complex_t b);

/**
 * Multiply two complex number together
 *
 * @param a first number
 * @param b second number
 *
 * @returns #a * #b
 */
complex_t cmplx_mul (const complex_t a, const complex_t b);

/**
 * Divide a complex number by another
 *
 * @param a first number
 * @param b second number
 *
 * @returns #a / #b
 */
complex_t cmplx_div (const complex_t a, const complex_t b);

/**
 * Raise a complex number to the power of another one.
 * **!!! Only works if b's imaginary part is zero!**
 *
 * @param a first number
 * @param b second number
 *
 * @returns #a ** #b
 */
complex_t cmplx_pow (const complex_t a, const int b);

/**
 * Get a square root of some complex number. The other one is the negation
 * of this value.
 *
 * @param a the number
 *
 * @returns sqrt(#a)
 */
complex_t cmplx_sqrt (const complex_t a);

/**
 * Compute the k'th cube root of a complex number.
 *
 * @param a the number
 * @param k the index of the root (between 0 and 2)
 *
 * @returns cube root of #a
 */
complex_t cmplx_cbrt (const complex_t a, int k);

/**
 * Negate a complex number
 *
 * @param a the number
 *
 * @returns -#a
 */
complex_t cmplx_negate (const complex_t a);

/**
 * Replaces any `-0` in the components of a complex number with just `0`
 *
 * @param a the number to normalize
 *
 * @returns #a with all negative zeros replaces
 */
complex_t cmplx_normalize_zero (const complex_t a);

/**
 * Compare two complex numbers for equality
 *
 * @returns whether #a equals #b
 */
bool cmplx_eq (const complex_t a, const complex_t b);

/**
 * Checks if both components of a complex number are 0
 * @param a the number to check
 *
 * @returns #a == 0 + 0i
 */
bool cmplx_is_zero (const complex_t a);

/**
 * Print a complex number to stdout
 *
 * @param x the number to print
 */
void print_complex (const complex_t x);

/**
 * The constant equal to `i`
 */
extern const complex_t CMPLX_I;

#endif // LIB_COMPLEX
