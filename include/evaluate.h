/**
 * @file
 * @brief A file that contains functions used to evaluate algebraic expressions from the parser.
 */

#ifndef LIB_EVALUATE
#define LIB_EVALUATE

#define MAX_VARS 128

#include "polynomial.h"
#include "complex.h"
#include "parser.h"

/// Internal value type - either a complex number, or a #Polynomial
typedef enum {
  /// Complex number
  TP_NUMBER,
  /// Polynomial
  TP_POLYNOMIAL
} ValueType;

/// Internal representation of a value
typedef struct {
  /// Type of the value
  ValueType type;
  union {
    /// Polynomial value, if type == #TP_POLYNOMIAL
    Polynomial poly;
    /// Numeric value, if type == #TP_NUMBER
    complex_t  num;
  };
} Value;

/// Internal representation of a stored variable
typedef struct {
  /// Whether this cell is used
  bool used;
  /// Value of the variable
  Value val;
} VarDescription;

/// Runtime environment, that contains stuff like variable values
typedef struct {
  /// Var dictionary
  VarDescription vars[MAX_VARS];
} Env;

/// #EVAL_OK or any sort of evaluation error
typedef enum {
  /// Everything is all right
  EVAL_OK = 0,
  /// Encountered a polynomial with a larger-than 4 degree
  TOO_LARGE_DEGREE,
  /// Encountered division by zero
  ZERO_DIVISION,
  /// Encountered usage of unbound variable
  NO_VARIABLE,
  /// Tried to operate on two polynomials with different variables. Kept
  /// for the sake of potentially introducing multivariate polynomials later
  DIFFERENT_POLY_VAR,
  /// Operation on invalid types, like dividing two polynomials
  TYPE_ERROR,
  /// Invalid power operation. Currently, you can only raise a rational number to a
  /// rational power, a comple number to an integer power, or a polynomial to an integer power
  COMPLEX_POWER,
  /// Some unknown error happened. This is only returned in cases that are considered unreachable
  WTF_ERROR,
} EvalStatus;

/**
 * Evaluate an #Expr into a #Value
 *
 * @param env    Where to take variable values from
 * @param expr   An expression to evaluate
 * @param output Where to write the resulting value
 *
 * @returns An #EvalStatus representing any error that may have occured
 */
EvalStatus eval_expr (Env *env, Expr *expr, Value *output);

/**
 * Get a variable value from and #Env
 *
 * @param env      Variable storage
 * @param var_name Name of the variable to get
 * @param output   Where to write the value
 *
 * @returns #NO_VARIABLE if it failed, or #EVAL_OK if succeeded
 */
EvalStatus env_get_value (Env *env, char var_name, Value *output);

/**
 * Set a variable to some value in an #Env
 *
 * @param env      Variable storage
 * @param var_name Name of the variable to set
 * @param val      Value of the variable
 */
void env_set_value (Env *env, char var_name, Value val);

/**
 * Print a value to stdout
 *
 * @param val #Value to print
 */
void print_value (Value val);

#endif // LIB_EVALUATE

