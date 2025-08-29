/**
 * @file
 * @brief algebraic expression parser
 */

#include <complex.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "parser.h"
#include "log.h"

/*
 * V double_literal = #%lg;
 * V num_literal = double_literal | double_literal 'i';
 *
 *
 * V poly_var_literal = [a-z];
 *  identifier = [A-Z];
 *
 * V term    = factor ( ('+' | '-') factor )*;
 * V factor  = unary ( ('*' | '/') unary )*;
 * V unary   = '-'* call;
 *   call    = power ("(" expression ")")*;
 * V power   = primary ('^' primary)*;
 * V primary = num_literal | var_literal | poly_var_literal | "(" expression ")";
 *
 * V expression = term;
 *
 *  command = let_cmd | solve_cmd | poltorashka_cmd | porno_cmd | expression;
 *
 *  let_cmd = "let" identifier "=" expression;
 *  solve_cmd = "solve" expression;
 *  poltorashka_cmd = "poltoraska";
 *  porno_cmd = "porno";
 */

int expression  (const char *str, int *current_index, Expr *output);
int term        (const char *str, int *current_index, Expr *output);
int factor      (const char *str, int *current_index, Expr *output);
int unary       (const char *str, int *current_index, Expr *output);
int call        (const char *str, int *current_index, Expr *output);
int power       (const char *str, int *current_index, Expr *output);
int primary     (const char *str, int *current_index, Expr *output);
int num_literal (const char *str, int *current_index, Expr *output);
int poly_var    (const char *str, int *current_index, Expr *output);
int identifier  (const char *str, int *current_index, Expr *output);

int add_or_sub_consumer (const char *str, int *current_index, Operator *output);
int div_or_mul_consumer (const char *str, int *current_index, Operator *output);
int pow_consumer        (const char *str, int *current_index, Operator *output);

int consume (const char *str, int *current_index, const char c);

int binary_level (
    int (*lower)(const char *, int *, Expr *),
    int (*op_consumer)(const char *, int *, Operator *),
    const char *str, int *current_index, Expr *output);

int a_or_b_consumer (char a, Operator op_a, char b, Operator op_b,
                     const char *str, int *current_index, Operator *output);

int a_consumer (char a, Operator op,
                const char *str, int *current_index, Operator *output);

void skip_spaces (const char *from, char *to);

int parse_expr (const char *source, Expr *output) {
  char *compressed_source = (char *) calloc(strlen(source) + 5, sizeof(char));
  skip_spaces(source, compressed_source);

  int current_index = 0;
  
  LOG_DEBUG("Compressed source: <%s>", compressed_source);
  int result = expression(compressed_source, &current_index, output);
  free(compressed_source);
  return result;
}

void destroy_expr (Expr *tree) {
  LOG_DEBUG("Freed %p", tree);
  if (!tree)
    return;

  switch (tree->type) {
    case OPERATOR:
      destroy_expr(tree->left );
      destroy_expr(tree->right);
      break;
    case NUMBER:
    case POLY_VAR:
    case IDENTIFIER:
    default:
      break;
  }
  free(tree);
}

void print_expr (const Expr *ast) {
  // LOG_DEBUG("entered with pointer %p, t = %d, %lg + %lgi", ast, ast->type, ast->val.real, ast->val.imag);
  if (!ast) {
    printf("(nil)");
    return;
  }

  switch (ast->type) {
    case OPERATOR:
      // LOG_DEBUG("Operator!");
      putchar('(');
      if (ast->op == OP_NEG) {
        putchar(ast->op);
        print_expr(ast->left);
      } else if (ast->op == OP_CALL) { 
        print_expr(ast->left);
        putchar('(');
        print_expr(ast->right);
        putchar(')');
      } else {
        print_expr(ast->left);
        putchar(' ');
        putchar(ast->op);
        putchar(' ');
        print_expr(ast->right);
      }
      putchar(')');
      break;
    case NUMBER:
      // LOG_DEBUG("Number!");
      print_complex(ast->val);
      break;
    case POLY_VAR:
      // LOG_DEBUG("Poly var!");
      putchar(ast->poly_name);
      break;
    case IDENTIFIER:
      // LOG_DEBUG("Identifier!");
      putchar(ast->var_name);
      break;
    default:
      break;
  }
}

int expression (const char *str, int *current_index, Expr *output) {
  int res = term(str, current_index, output);
  if (!res)
    LOG_DEBUG("Failed! Left <%s>", str + *current_index);
  else
    LOG_DEBUG("Parsed! Left <%s>", str + *current_index);

  return res;
}

int add_or_sub_consumer (const char *str, int *current_index, Operator *output) {
  return a_or_b_consumer(
      '+', OP_ADD, '-', OP_SUB,
      str, current_index, output
    );
}

int term (const char *str, int *current_index, Expr *output) {
  int res = binary_level(factor, add_or_sub_consumer, str, current_index, output);

  if (!res)
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
  else
    LOG_DEBUG("Parsed! Left <%s>", str + *current_index);
  
  return res;
}

int div_or_mul_consumer (const char *str, int *current_index, Operator *output) {
  return a_or_b_consumer(
      '*', OP_MUL, '/', OP_DIV,
      str, current_index, output
    );
}

int factor (const char *str, int *current_index, Expr *output) {
  int res = binary_level(unary, div_or_mul_consumer, str, current_index, output);

  if (!res)
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
  else
    LOG_DEBUG("Parsed! Left <%s>", str + *current_index);
  
  return res;
}


int unary (const char *str, int *current_index, Expr *output) {
  int flip = 0;
  while (consume(str, current_index, '-'))
    flip = !flip;
  
  Expr *left = (Expr *) calloc(1, sizeof(Expr));
  if (!call(str, current_index, left)) {
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
    free(left);
    return 0;
  }
  
  if (flip) {
    output->type = OPERATOR;
    output->left = left;
    output->op   = OP_NEG;
  } else {
    *output = *left;
    free(left);
  }

  LOG_DEBUG("Parsed! Left <%s>", str + *current_index);
  return 1;
}

int call (const char *str, int *current_index, Expr *output) {
  Expr *lhs = (Expr *) calloc(1, sizeof(Expr));

  if (!power(str, current_index, lhs)) {
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
    free(lhs);
    return 0;
  }
  
  Expr *rhs = NULL;
  int call_chain = 0;

  LOG_DEBUG("Trying to parse call chain! Left <%s>", str + *current_index);
  
  while ( consume    (str, current_index, '(')
       && (++call_chain)
       && (rhs = (Expr *) calloc(1, sizeof(Expr)))
       && expression (str, current_index, rhs)
       && consume    (str, current_index, ')')) {
    call_chain++;
    
    Expr *assembled  = (Expr *) calloc(1, sizeof(Expr));
    assembled->type  = OPERATOR;
    assembled->op    = OP_CALL;
    assembled->left  = lhs;
    assembled->right = rhs;

    lhs = assembled;
  }

  if (call_chain % 2) // odd - allocated rhs but didn't get to the cycle body
    free(rhs);

  LOG_DEBUG("Parsed! Left <%s>", str + *current_index);

  *output = *lhs;
  LOG_DEBUG("Freed %p", lhs);
  free(lhs);
  return 1;
}

int pow_consumer (const char *str, int *current_index, Operator *output) {
  return a_consumer('^', OP_POW, str, current_index, output);
}

int power (const char *str, int *current_index, Expr *output) {
  int res = binary_level(primary, pow_consumer, str, current_index, output);

  if (!res)
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
  else
    LOG_DEBUG("Parsed! Left <%s>", str + *current_index);

  return res;
}

int primary (const char *str, int *current_index, Expr *output) {
  int res = 0;
  res = num_literal(str, current_index, output);
  if (res)
    return res;
  res = poly_var(str, current_index, output);
  if (res)
    return res;
  res = identifier(str, current_index, output);
  if (res)
    return res;
  
  LOG_DEBUG("Trying to parse (<expr>)! Left: <%s>", str + *current_index);

  if (consume    (str, current_index, '(')  && 
      expression (str, current_index, output)  && 
      consume    (str, current_index, ')'))
    return 1;

  return 0;
}

int num_literal (const char *str, int *current_index, Expr *output) {
  int pos = 0;
  double value = 0;

  int res = sscanf(str + *current_index, "%lg%n", &value, &pos);

  if (!res) {
    LOG_DEBUG("Failed to parse! Left <%s>", str + *current_index);
    return 0;
  } else {
    *current_index += pos;
    output->type = NUMBER;
    if (str[*current_index] == 'i') {
      (*current_index)++;
      output->val = {0, value};
    } else
      output->val = {value};
    
    LOG_DEBUG("Parsed %lg + %lgi! Left <%s>", output->val.real, output->val.imag, str + *current_index);
    return 1;
  }
}

int poly_var (const char *str, int *current_index, Expr *output) {
  if (str[*current_index] >= 'a' && str[*current_index] <= 'z') {
    char res = str[*current_index];
    (*current_index)++;

    output->type = POLY_VAR;
    output->poly_name = res;
    LOG_DEBUG("Parsed %c! Left <%s>", res, str + *current_index);
    return 1;
  } else {
    LOG_DEBUG("Failed! Left <%s>", str + *current_index);
    return 0;
  }
}

int identifier (const char *str, int *current_index, Expr *output) {
  char res = str[*current_index];
  if ('A' <= res && res <= 'Z') {
    (*current_index)++;
    output->type = IDENTIFIER;
    output->var_name = res;
    LOG_DEBUG("Parsed %c! Left <%s>", res, str + *current_index);
    return 1;
  }

  LOG_DEBUG("Failed %c! Left <%s>", res, str + *current_index);
  return 0;
}

void skip_spaces (const char *in, char *out) {
  while (*in) {
    if (!isspace(*in))
      *out++ = *in;
    in++;
  }
}

int consume (const char *str, int *current_index, char c) {
  LOG_DEBUG("Expecting '%c'! Left <%s>", c, str + *current_index);

  if (str[*current_index] == c) {
    (*current_index)++;
    return 1;
  } else {
    return 0;
  }
}

int a_consumer (
    char a, Operator op,
    const char *str, int *current_index, Operator *output) {

  if (str[*current_index] == a) {
    (*current_index)++;
    *output = op;
    return 1;
  }

  return 0;
}

int a_or_b_consumer (
    char a, Operator op_a, char b, Operator op_b,
    const char *str, int *current_index, Operator *output) {

  return a_consumer(a, op_a, str, current_index, output) ||
         a_consumer(b, op_b, str, current_index, output);
}

int binary_level (
    int (*lower)(const char *, int *, Expr *),
    int (*op_consumer)(const char *, int *, Operator *),
    const char *str, int *current_index, Expr *output) {
  
  Expr *lhs = (Expr *) calloc(1, sizeof(Expr));
  if (lower && !lower(str, current_index, lhs)) {
    free(lhs);
    return 0;
  }
  
  Operator op = {};
  while (op_consumer(str, current_index, &op)) {
    Expr *rhs = (Expr *) calloc(1, sizeof(Expr));
    if (lower && !lower(str, current_index, rhs)) {
      free(lhs);
      return 0;
    }
    

    Expr *assembled  = (Expr *) calloc(1, sizeof(Expr));
    assembled->type  = OPERATOR;
    assembled->left  = lhs;
    assembled->right = rhs;
    assembled->op    = op;

    lhs = assembled;
  }

  *output = *lhs;
  LOG_DEBUG("Freed %p", lhs);
  free(lhs);

  return 1;
}

int parse_stmt (const char *source, Statement *output) {
  int current_index = 0;
  char var_name[2];

  if (sscanf(source, " let %1[A-Z] =%n", var_name, &current_index) == 1) {
    Expr *expr = (Expr *) calloc(1, sizeof(Expr));
    if (!parse_expr(source + current_index, expr)) {
      free(expr);
      LOG_DEBUG("Failed to parse expression!");
      return 0;
    }

    output->cmd = CMD_LET;
    output->var = var_name[0];
    output->expr = expr;
    return 1;
  }

  LOG_DEBUG("Failed to parse let cmd! Left <%s>", source + current_index);
  
  char cmd[1024];
  sscanf(source, " %s %n", cmd, &current_index);
  
  if (!strcmp(cmd, "porno")) {
    output->cmd = CMD_PORNO;
    return 1;
  }

  if (!strcmp(cmd, "poltorashka")) {
    output->cmd = CMD_POLTORASHKA;
    return 1;
  }

  if (!strcmp(cmd, "solve")) {
    Expr *expr = (Expr *) calloc(1, sizeof(Expr));
    if (!parse_expr(source + current_index, expr)) {
      free(expr);
      LOG_DEBUG("Failed to parse expression!");
      return 0;
    }

    output->cmd = CMD_SOLVE;
    output->expr = expr;
    return 1;
  }

  Expr *expr = (Expr *) calloc(1, sizeof(Expr));
  if (!parse_expr(source, expr)) {
    free(expr);
    LOG_DEBUG("Failed to parse expression!");
    return 0;
  }

  output->cmd = CMD_EXPR;
  output->expr = expr;
  return 1;
}

void destory_stmt (Statement *stmt) {
  destroy_expr(stmt->expr);
  free(stmt);
}

void print_stmt (const Statement *stmt) {
  switch (stmt->cmd) {
    case CMD_POLTORASHKA:
      printf("poltorashka");
      break;
    case CMD_PORNO:
      printf("porno");
      break;
    case CMD_LET:
      printf("left %c = ", stmt->var);
      print_expr(stmt->expr);
      break;
    case CMD_SOLVE:
      printf("solve ");
      print_expr(stmt->expr);
      break;
    case CMD_EXPR:
      print_expr(stmt->expr);
      break;
    default:
      break;
  }
  putchar('\n');
}


