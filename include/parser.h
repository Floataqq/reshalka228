/**
 * @file
 * @brief A parser for polynomials
 */

#ifndef LIB_PARSER
#define LIB_PARSER


#include "complex.h"

/// Available operators
typedef enum {
  /// Addition
  OP_ADD  = '+',
  /// Subtraction
  OP_SUB  = '-',
  /// Multiplication
  OP_MUL  = '*',
  /// Division
  OP_DIV  = '/',
  /// Power
  OP_POW  = '^',
  /// Unary negation
  OP_NEG  = '~',
  /// Evaluation of a polynomial at a point
  OP_CALL = '!', // won't be displayed unless something went wrong
} Operator;

/// AST node types
typedef enum {
  /// An operator node, like addition, negation or call
  OPERATOR,
  /// A number constant node
  NUMBER,
  /// A polynomial variable (like `x`, or `y`)
  POLY_VAR,
  /// An identifier - currently a single uppercase letter
  IDENTIFIER
} NodeType;

/// An AST node
typedef struct Expr {
  /// It's node type
  NodeType type;
  union {
    /// Operator node data
    struct { struct Expr *left; Operator op; struct Expr *right; };
    /// Number node data
    struct { complex_t val;  };
    /// Plynomial variable node data
    struct { char poly_name; };
    /// Identifier node data
    struct { char var_name;  };
  };
} Expr;

/**
 * Parse an expression from a string. Allocates nodes by itself.
 *
 * @param source The source string
 * @param output Where to write the resulting value
 *
 * @returns zero if parsing failed, otherwise a non-zero value
 */
int  parse_expr (const char *source, Expr *output);

/**
 * Free all allocated child nodes of an #Expr. Assumes \p ast is also allocated on the heap.
 *
 * @param ast The tree to destory
 */
void destroy_expr (Expr *ast);

/**
 * Recursively print an #Expr tree to stdout
 *
 * @param ast The tree to print out
 */
void print_expr (const Expr *ast);

/// An enum for available commands
typedef enum {
  /// Assigns a value to a variable
  CMD_LET,
  /// Solves a polynomial
  CMD_SOLVE,
  /// Shows a cute kitty
  CMD_POLTORASHKA,
  /// Shows a funny video
  CMD_PORNO,
  /// Evaluates an expression and prints the reslut
  CMD_EXPR
} Command;

/// A struct for storing parsed commands
typedef struct {
  /// Comman type
  Command cmd;

  /// The expression referenced in the command. let, expr and solve only
  Expr *expr;

  /// The variable referenced in the command. let only
  char var;
} Statement;

/**
 * Parse a statement from a string
 *
 * @param source The string to parse from
 * @param output Where to write the output
 *
 * @returns a zero if parsing failed, otherwise a non-zero code
 */
int  parse_stmt (const char *source, Statement *output);

/**
 * Free a #Statement. Assumes that \p stmt itself was allocated on the heap
 *
 * @param stmt The statement to destroy
 */
void destory_stmt (Statement *stmt);

/**
 * Print a command to stdout
 *
 * @param stmt The statement to print
 */
void print_stmt (const Statement *stmt);


#endif // LIB_PARSER
