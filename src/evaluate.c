/**
 * @file
 * @brief A file that contains functions used to evaluate algebraic expressions from the parser.
 */ 

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include "evaluate.h"
#include "polynomial.h"
#include "complex.h"
#include "parser.h"
#include "equation.h"
#include "log.h"

EvalStatus handle_op_neg  (Env *env, Expr *target,            Value *output);
EvalStatus handle_op_add  (Env *env, Expr *left, Expr *right, Value *output);
EvalStatus handle_op_sub  (Env *env, Expr *left, Expr *right, Value *output);
EvalStatus handle_op_mul  (Env *env, Expr *left, Expr *right, Value *output);
EvalStatus handle_op_div  (Env *env, Expr *left, Expr *right, Value *output);
EvalStatus handle_op_pow  (Env *env, Expr *left, Expr *right, Value *output);
EvalStatus handle_op_call (Env *env, Expr *left, Expr *right, Value *output);

/**
 * Handles some operation on two values, based on functions passed to it.
 *
 * @param num_num    A function that handles the operation on two numbers
 * @param num_poly   A function that handles the operation on a number and a polynomial
 * @param poly_num   A function that handles the operation on a polynomial and a number
 * @param poly_poly  A function that handles the operation on two polynomials
 *
 * @param env        Execution context to take variables from
 * @param left       Left operand
 * @param right      Right operand
 *
 * @param output     A place to write the resulting value to
 */
EvalStatus handle_op_generic (
    EvalStatus (*num_num)   (complex_t,  complex_t,  Value *),
    EvalStatus (*num_poly)  (complex_t,  Polynomial, Value *),
    EvalStatus (*poly_num)  (Polynomial, complex_t,  Value *),
    EvalStatus (*poly_poly) (Polynomial, Polynomial, Value *),
    Env *env, Expr *left, Expr *right, Value *output);

EvalStatus add_num_num   (complex_t a,  complex_t b,  Value *output);
EvalStatus add_num_poly  (complex_t n,  Polynomial p, Value *output);
EvalStatus add_poly_num  (Polynomial p, complex_t n,  Value *output);
EvalStatus add_poly_poly (Polynomial a, Polynomial b, Value *output);

EvalStatus mul_num_num   (complex_t a,  complex_t b,  Value *output);
EvalStatus mul_num_poly  (complex_t n,  Polynomial p, Value *output);
EvalStatus mul_poly_num  (Polynomial p, complex_t n,  Value *output);
EvalStatus mul_poly_poly (Polynomial a, Polynomial b, Value *output);

EvalStatus div_num_num   (complex_t a,  complex_t b,  Value *output);
EvalStatus div_poly_num  (Polynomial p, complex_t n,  Value *output);

EvalStatus pow_num_num   (complex_t a,  complex_t b,  Value *output);
EvalStatus pow_poly_num  (Polynomial p, complex_t n,  Value *output);

EvalStatus call_poly_num (Polynomial p, complex_t n,  Value *output);

Value mk_number (complex_t  n);
Value mk_poly   (Polynomial p);

EvalStatus handle_polynomial_error (PolynomialError err);

EvalStatus eval_expr (Env *env, Expr *expr, Value *output) {
  switch (expr->type) {
    case POLY_VAR:
      LOG_DEBUG("Evaluating POLY_VAR...");
      output->type = TP_POLYNOMIAL;
      // 1 * poly_name^1
      output->poly = { expr->poly_name, {.d = {1, 0}}};
      return EVAL_OK;
    case NUMBER:
      LOG_DEBUG("Evaluating NUMBER (%lg + %lgi)...", expr->val.real, expr->val.imag);
      output->type = TP_NUMBER;
      output->num = expr->val;
      return EVAL_OK;
    case IDENTIFIER:
      LOG_DEBUG("Evaluating IDENTIFIER...");
      return env_get_value(env, expr->var_name, output);
    case OPERATOR:
      LOG_DEBUG("Evaluating OPERATOR...");
      switch (expr->op) {
        case OP_ADD:
          return handle_op_add(env, expr->left, expr->right, output);
        case OP_SUB:
          return handle_op_sub(env, expr->left, expr->right, output);
        case OP_NEG:
          return handle_op_neg(env, expr->left, output);
        case OP_MUL:
          return handle_op_mul(env, expr->left, expr->right, output);
        case OP_DIV:
          return handle_op_div(env, expr->left, expr->right, output);
        case OP_POW:
          return handle_op_pow(env, expr->left, expr->right, output);
        case OP_CALL:
          return handle_op_call(env, expr->left, expr->right, output);
        default:
          return WTF_ERROR;
      }
    default:
      return WTF_ERROR;
  }
}

EvalStatus env_get_value (Env *env, char var_name, Value *output) {
  VarDescription var = env->vars[(int) var_name];
  if (var.used) {
    *output = var.val;
    return EVAL_OK;
  } else {
    return NO_VARIABLE;
  }
}

void env_set_value (Env *env, char var_name, Value value) {
  VarDescription var = { .used = true, .val = value };
  env->vars[(int) var_name] = var;
}

void print_value (Value val) {
  switch (val.type) {
    case TP_NUMBER:
      print_complex(val.num);
      break;
    case TP_POLYNOMIAL:
      print_polynomial(val.poly);
      break;
    default:
      assert(false);
  }
  putchar('\n');
}

EvalStatus handle_op_neg (Env *env, Expr *target, Value *output) {
  Value target_val;

  EvalStatus res = eval_expr(env, target, &target_val);
  if (res)
    return res;

  if (target_val.type == TP_NUMBER)
    *output = mk_number(cmplx_negate(target_val.num));
  else {
    Polynomial negated = {};

    PolynomialError err = polynomial_negate(target_val.poly, &negated);
    if (err)
      return handle_polynomial_error(err);

    *output = mk_poly(negated);
  }

  return EVAL_OK;
}

EvalStatus handle_op_add (Env *env, Expr *left, Expr *right, Value *output) {
  return handle_op_generic(
    add_num_num, add_num_poly, add_poly_num, add_poly_poly,
    env, left, right, output
  );
}

EvalStatus handle_op_sub (Env *env, Expr *left, Expr *right, Value *output) {
  // a - b = a + (-b), at least in this language
  Expr neg_right = {
    .type = OPERATOR,
    .left = right,
    .op = OP_NEG,
  };
  return handle_op_add(env, left, &neg_right, output);
}

EvalStatus handle_op_mul (Env *env, Expr *left, Expr *right, Value *output) {
  return handle_op_generic(
      mul_num_num, mul_num_poly, mul_poly_num, mul_poly_poly,
      env, left, right, output
  );
}

EvalStatus handle_op_div (Env *env, Expr *left, Expr *right, Value *output) {
  return handle_op_generic(
      div_num_num, NULL, div_poly_num, NULL,
      env, left, right, output
  );
}

EvalStatus handle_op_pow  (Env *env, Expr *left, Expr *right, Value *output) {
  return handle_op_generic(
    pow_num_num, NULL, pow_poly_num, NULL,
    env, left, right, output
  );
}

EvalStatus handle_op_call (Env *env, Expr *left, Expr *right, Value *output) {
  return handle_op_generic(
    NULL, NULL, call_poly_num, NULL,
    env, left, right, output
  );
}

EvalStatus handle_op_generic (
    EvalStatus (*num_num)   (complex_t,  complex_t,  Value *),
    EvalStatus (*num_poly)  (complex_t,  Polynomial, Value *),
    EvalStatus (*poly_num)  (Polynomial, complex_t,  Value *),
    EvalStatus (*poly_poly) (Polynomial, Polynomial, Value *),
    Env *env, Expr *left, Expr *right, Value *output) {
  Value a, b;
  EvalStatus res;

  res = eval_expr(env, left, &a);
  if (res)
    return res;

  res = eval_expr(env, right, &b);
  if (res)
    return res;
  
  if (a.type == TP_NUMBER && b.type == TP_NUMBER) {
    if (num_num)
      res = num_num(a.num, b.num, output);
    else
      return TYPE_ERROR;
  } else if (a.type == TP_NUMBER && b.type == TP_POLYNOMIAL) {
    if (num_poly)
      res = num_poly(a.num, b.poly, output);
    else
      return TYPE_ERROR;
  } else if (a.type == TP_POLYNOMIAL && b.type == TP_NUMBER) {
    if (poly_num)
      res = poly_num(a.poly, b.num, output);
    else
      return TYPE_ERROR;
  }
  else if (a.type == TP_POLYNOMIAL && b.type == TP_POLYNOMIAL) {
    if (poly_poly)
      res = poly_poly(a.poly, b.poly, output);
    else
      return TYPE_ERROR;
  } else
    assert(false);

  return res;
}

Value mk_number (complex_t  n) {
  return {
    TP_NUMBER,
    { .num = n },
  };
}

Value mk_poly   (Polynomial p) {
  return {
    TP_POLYNOMIAL,
    { .poly = p },
  };
}

EvalStatus handle_polynomial_error (PolynomialError err) {
  switch (err) {
    case POLY_OK:
      return EVAL_OK;
    case POLY_TOO_LARGE:
      return TOO_LARGE_DEGREE;
    case POLY_DIFFERENT_VAR:
      return DIFFERENT_POLY_VAR;
    default:
      return WTF_ERROR;
  }
}

EvalStatus add_num_num (complex_t a, complex_t b, Value *output) {
  *output = mk_number(cmplx_add(a, b));
  return EVAL_OK;
}

EvalStatus add_num_poly (complex_t n, Polynomial p, Value *output) {
  // add to last coefficient
  p.e = cmplx_add(p.e, n);
  *output = mk_poly(p);
  return EVAL_OK;
}

EvalStatus add_poly_num (Polynomial p, complex_t n, Value *output) {
  return add_num_poly(n, p, output);
}

EvalStatus add_poly_poly (Polynomial a, Polynomial b, Value *output) {
  if (a.var != b.var) {
    LOG_DEBUG("Encountered polynomials with different variables - %c and %c", a.var, b.var);
    return DIFFERENT_POLY_VAR;
  }

  Polynomial res = {};
  PolynomialError err = polynomial_add(a, b, &res);
  EvalStatus res_status = handle_polynomial_error(err);

  if (res_status)
    return res_status;

  *output = mk_poly(res);
  return EVAL_OK;
}

EvalStatus mul_num_num (complex_t a, complex_t b, Value *output) {
  *output = mk_number(cmplx_mul(a, b));
  return EVAL_OK;
}

EvalStatus mul_num_poly (complex_t n, Polynomial p, Value *output) {
  for (int i = 0; i < POLY_COEFF_LEN; i++)
    p.coeffs[i] = cmplx_mul(p.coeffs[i], n);

  *output = mk_poly(p);
  return EVAL_OK;
}

EvalStatus mul_poly_num (Polynomial p, complex_t n, Value *output) {
  return mul_num_poly(n, p, output);
}

EvalStatus mul_poly_poly (Polynomial a, Polynomial b, Value *output) {
  if (a.var != b.var) {
    LOG_DEBUG("Encountered polynomials with different variables - %c and %c", a.var, b.var);
    return DIFFERENT_POLY_VAR;
  }

  Polynomial res = {};
  PolynomialError err = polynomial_mul(a, b, &res);
  EvalStatus res_status = handle_polynomial_error(err);

  if (res_status)
    return res_status;

  *output = mk_poly(res);
  return EVAL_OK;
}

EvalStatus div_num_num (complex_t a,  complex_t b,  Value *output) {
  if (cmplx_is_zero(b))
    return ZERO_DIVISION;

  *output = mk_number(cmplx_div(a, b));
  return EVAL_OK;
}

EvalStatus div_poly_num (Polynomial p, complex_t n,  Value *output) {
  if (cmplx_is_zero(n))
    return ZERO_DIVISION;
  
  for (int i = 0; i < POLY_COEFF_LEN; i++)
    p.coeffs[i] = cmplx_div(p.coeffs[i], n);

  *output = mk_poly(p);
  return EVAL_OK;
}

EvalStatus pow_num_num (complex_t a,  complex_t b,  Value *output) {
  if (!is_zero(b.imag)) {
    return COMPLEX_POWER;
  }
  
  if (!is_zero(a.imag) && !is_zero(fmod(b.real, 1.0))) {
    return COMPLEX_POWER;
  }
  
  if (!is_zero(a.imag)) {
    *output = mk_number({pow(a.real, b.real)});
    return EVAL_OK;
  }
  
  complex_t res = {1};
  for (int i = 0; i < a.real; i++)
    res = cmplx_mul(res, a);

  *output = mk_number(res);
  return EVAL_OK;
}

EvalStatus pow_poly_num (Polynomial p, complex_t n,  Value *output) {
  if (!is_zero(n.imag))
    return COMPLEX_POWER;

  if (is_zero(n.real)) {
    *output = mk_number({0});
    return EVAL_OK;
  }

  if (!is_zero(fmod(n.real, 1.0)))
    return COMPLEX_POWER;

  Polynomial res = { p.var, {.e = {1}} };

  for (int i = 0; i < n.real; i++) {
    PolynomialError status = polynomial_mul(res, p, &res);
    if (status)
      return handle_polynomial_error(status);
  }

  *output = mk_poly(res);
  return EVAL_OK;
}

EvalStatus call_poly_num (Polynomial p, complex_t n,  Value *output) {
  *output = mk_number(polynomial_eval(p, n));
  return EVAL_OK;
}
