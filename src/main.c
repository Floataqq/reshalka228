#include <stdio.h>
#include <math.h>

#define EPSILON 10e-7

int is_zero(double x) {
  return fabs(x) < EPSILON;
}

typedef enum {
  NOT_COMPUTED = 0,
  NONE,
  SINGLE,
  DOUBLE,
  INFINITE
} SolutionStatus;

typedef struct {
  /// x^2 coefficient
  int a;
  /// x coefficient
  int b;
  /// constant coefficient
  int c;
  
  SolutionStatus tag;
  double solutions[2];
} Equation;

int read_equation(Equation *eq, int argc, char **argv) {
  double a, b, c;
  int result;

  if (argc == 4) {
    char input[1024];
    sprintf(input, "%s %s %s", argv[1], argv[2], argv[3]);
    result = sscanf(input, "%lf %lf %lf", &a, &b, &c);
  } else {
    printf("Enter equation coefficients a, b, and c in order:\n");
    result = scanf("%lf %lf %lf", &a, &b, &c);
  }
  
  if (result == EOF || result == 0) {
    printf("Error: could not read the equation data from stdin or arguments\n");
    return 0;
  }

  if (is_zero(a)) {
    printf("Error: not a quardatic equation!\n  (although i can add a linear equation solver if needed)\n");
    return 0;
  }

  eq->a = a;
  eq->b = b;
  eq->c = c;
  eq->tag = NOT_COMPUTED;
  return 1;
}

void compute_solutions(Equation *eq) {
  double a = eq->a, b = eq->b, c = eq->c;
  double d = b*b - 4 * a*c;
  
  if (is_zero(a)) {
    eq->tag = SINGLE;
    eq->solutions[0] = -b / (2 * a);
  } else if (d > 0) {
    double d_sqrt = sqrt(d);
    eq->tag = DOUBLE;
    eq->solutions[0] = (-b + d_sqrt) / (2 * a);
    eq->solutions[1] = (-b - d_sqrt) / (2 * a);
  } else {
    eq->tag = NONE;
  }
}

void print_solutions(Equation eq) {
  switch (eq.tag) {
    case NOT_COMPUTED:
      printf("Error: The equation solutions were not yet computed!\n");
      break;
    case NONE:
      printf("The equation has no solutions!\n");
      break;
    case SINGLE:
      printf("The eqaution has a single solution:\n");
      printf(" - x = %lf\n", eq.solutions[0]);
      break;
    case DOUBLE:
      printf("The equation has two solutions:\n");
      printf("- x1 = %lf\n", eq.solutions[0]);
      printf("- x2 = %lf\n", eq.solutions[1]);
      break;
    case INFINITE:
      printf("The eqaution has an infinite number of solutions!\n");
  }
}

int main(int argc, char **argv) {
  Equation equation;  
  if (!read_equation(&equation, argc, argv))
    return 1;

  compute_solutions(&equation);
  
  print_solutions(equation);

  return 0;
}

