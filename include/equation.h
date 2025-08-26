#ifndef EQUATION_SOLVER_LIB
#define EQUATION_SOLVER_LIB

#include <stdlib.h>

#define EPSILON 10e-7
#define MAX_LENGTH 256

/**
 * Just like `assert(false)`, but doesn't print any sort of error message.
 * Does nothing if NDEBUG is defined.
 */
[[noreturn]] void abort_with_ndebug(void);

/**
 * Quite similar to rust's `unreachable!()` macro.
 *
 * Prints an error message like this:
 * ```
 * example.c:42
 *   Unreachable code hit in example_failing_function
 * ```
 *
 * And aborts execution unless NDEBUG is set.
 */
#define UNREACHABLE(...) {                            \
  printf("%s:%d\n", __FILE__, __LINE__);              \
  printf("  Unreachable code hit in %s\n", __func__); \
  printf("" __VA_ARGS__);                             \
  abort_with_ndebug();                                \
}

typedef enum {
  /// The solutions have not been computed yet
  NOT_COMPUTED = 0,
  /// The equation has no solutions
  NONE,
  /// The equation only has a single solution
  SINGLE,
  /// The equation has two solutions
  DOUBLE,
  /// The equation has infinitely many solutions
  INFINITE
} SolutionStatus;

// TODO: обощить до полинома
// TODO: добавить стандартныое значение тэгу
//       (конструктор \ дефолтная инициализация)
// TODO: union с четырехпольной структурой
// TODO: список велосипедов: логи, парсинг аргументов
// TODO: ассерты везде накидать
/// A structure describing a quadratic equation and it's solutions
typedef struct {
  /// x^2 coefficient
  double a;
  /// x coefficient
  double b;
  /// constant coefficient
  double c;

  /// A tag describing whether this equation's solutions
  /// have been computed and how many are there.
  ///
  /// @brief Status of soluti on computation
  SolutionStatus tag;
  /// Array of solutions. A quadratic equation can have at most 2
  /// solutions, so there are just two elements. If there are less
  /// solutions, last elements of the array are unused.
  ///
  /// @brief Array with solutions
  double solutions[2];
} Equation;

// ------- src/equation_io.c -------

/**
 * Reads an #Equation from a string \p line in the format: \<a\> \<b\> \<c\>.
 * Some valid examples:
 *  - "1.0 2 3"
 *  - "1 -2 3"
 *
 *  @param line The string from which to read the equation
 *  @param eq   The #Equation to read into
 *  @returns    A zero if the equation was parsed successfully, otherwise
 *              a non-zero code.
 */
int read_equation_from_line(char *line, Equation *eq);

/**
 * Reads an #Equation from a \p argv using \p argc in the format: \<a\> \<b\> \<c\>.
 * Each value should be in it's own argument.
 * Some valid examples:
 *  - "1.0 2 3"
 *  - "1 -2 3"
 *
 *  @param eq The #Equation to read into
 *  @param argc Number of arguments
 *  @param argv Arguments array
 *  @returns A zero if the equation was parsed successfully, otherwise a non-zero code.
 */
int read_equation_from_argv(Equation *eq, int argc, char *argv[]);

/**
 * Nicely prints solutions of an #Equation. Requires that \ref Equation.tag
 * is not #NOT_COMPUTED.
 *
 * @param eq The #Equation to get solutions from
 */
void print_solutions(const Equation eq);

// ------- src/arith.c -------

/**
 * Returns true if the \p x number is within #EPSILON of zero.
 *
 * @param x The number to check.
 * @returns A zero if \p x is within #EPSILON of 0. Otherwise a non-zero code.
 */
int is_zero(const double x);

/**
 * Returns true if \p x is within #EPSILON of \p y.
 *
 * @param x Number to check
 * @param y Number to check
 * @returns Whether the numbers are compute_solutions
 */
int is_equal(const double x, const double y);

/**
 * Sets `-0` to 0. Otherwise returns the same number
 *
 * @param x The number to modify
 * @returns Zero if the number is `-0`, otherwise the same number
 */
double normalize_zero(const double x);

/**
 * Solves an #Equation assuming that it's \ref Equation.a coefficient equals 0.
 * Puts the number of roots into \ref Equation.tag and the roots
 * in \ref Equation.solutions.
 *
 * @param eq Reference to the equation to solve
 */
void solve_linear_equation(Equation *const eq);

/**
 * Solves an #Equation assuming that it's \ref Equation.a coefficient does
 * not equal 0. Puts the number of roots into \ref Equation.tag and the roots
 * in \ref Equation.solutions
 *
 * @param eq Reference to the equation to solve
 */
void solve_quadratic_equation(Equation *const eq);

/**
 * Solves an #Equation.
 * Puts the number of roots into \ref Equation.tag and the roots
 * in \ref Equation.solutions.
 *
 * @param eq Reference to the equation to solve
 */
void compute_solutions(Equation *const eq);


#endif // EQUATION_SOLVER_LIB

