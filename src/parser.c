#include "parser.h"

/*
 * double_literal = #%lg;
 * num_literal = double_literal | double_literal 'i';
 *
 *
 * poly_var_literal = [a-z];
 * var_literal = [A-Z][a-z0-9]+;
 *
 * term    = factor ( ('+' | '-') factor )*;
 * factor  = unary ( ('*' | '/') unary )*;
 * unary   = '-' unary | power;
 * power   = primary ('^' primary);
 * primary = num_literal | var_literal | poly_var_literal;
 *
 */

int parse_poly(const char *str, Polynomial *poly) {
  
}

