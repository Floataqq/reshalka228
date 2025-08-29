/**
 * @file
 * @brief Functions for finding #Polynomial roots
 */

#ifndef LIB_ARITH
#define LIB_ARITH


#include "polynomial.h"

/**
 *  Solve a given #Polynomial and produce a #Solutions object
 *
 *  @param p The polynomial to solve
 *  @returns A struct with \p p 's roots
 */
int solve_polynomial (Polynomial p, Solutions *sols);


#endif // LIB_ARITH
