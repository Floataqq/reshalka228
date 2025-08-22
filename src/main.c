#include <stdio.h>
#include <string.h>
#include <math.h>

#define EPSILON 10e-7
#define MAX_LENGTH 256

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
  double a;
  /// x coefficient
  double b;
  /// constant coefficient
  double c;
  
  SolutionStatus tag;
  double solutions[2];
} Equation;

int read_equation_from_line(char *line, Equation *eq) {
  double a, b, c;
  int result;

  result = sscanf(line, "%lf %lf %lf", &a, &b, &c);
  // scanf reads three things
  if (result != 3)
    return result;

  eq->a = a;
  eq->b = b;
  eq->c = c;
  eq->tag = NOT_COMPUTED;
  return 0;
}

int read_equation_from_argv(Equation *eq, int argc, char *argv[]) {
  int result;
  
  if (argc == 1)
    return 1;
  else if (argc != 4) {
    printf("Error: invalid format in args!\n");
    return 1;
  }

  char input[MAX_LENGTH];
  sprintf(input, "%s %s %s", argv[1], argv[2], argv[3]);
  
  result = read_equation_from_line(input, eq);
  return result;
}

void compute_solutions(Equation *eq) {
  double a = eq->a, b = eq->b, c = eq->c;
  double d = b*b - 4 * a*c;
  
  // linear eqautions
  if (is_zero(a)) {
    int is_zero_b = is_zero(b);
    int is_zero_c = is_zero(c);
    if (!is_zero_b && is_zero_c) {
      eq->tag = SINGLE;
      eq->solutions[0] = 0;
    } else if (!is_zero_c && is_zero_b) {
      eq->tag = NONE;
    } else if (is_zero_b && is_zero_c) {
      eq->tag = INFINITE;
    } else {
      eq->tag = SINGLE;
      eq->solutions[0] = -c / b;
    }
  } 
  // quadratic equations
  else if (is_zero(d)) {
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
  char eq_str[MAX_LENGTH];
  sprintf(eq_str, "%lg*x^2 + %lg*x + %lg = 0", eq.a, eq.b, eq.c);

  switch (eq.tag) {
    case NOT_COMPUTED:
      printf("Error: %s solutions were not yet computed!\n", eq_str);
      break;
    case NONE:
      printf("%s has no solutions!\n", eq_str);
      break;
    case SINGLE:
      printf("%s has a single solution:\n", eq_str);
      printf(" - x = %lg\n", eq.solutions[0]);
      break;
    case DOUBLE:
      printf("%s has two solutions:\n", eq_str);
      printf("- x1 = %lg\n", eq.solutions[0]);
      printf("- x2 = %lg\n", eq.solutions[1]);
      break;
    case INFINITE:
      printf("%s has an infinite number of solutions!\n", eq_str);
  }
}

int main(int argc, char *argv[]) {
  Equation equation;

  if (read_equation_from_argv(&equation, argc, argv)) {
    int line_no = 0;
    char line[MAX_LENGTH];

    printf("Enter equation coefficients one by one on different lines:\n");
        

    while (1) {
      if(!fgets(line, MAX_LENGTH, stdin))
        break;
      int read_cnt = strlen(line);
      if (read_cnt <= 1) // just \n or EOF
        break;

      line_no++;
      if (read_equation_from_line(line, &equation)) {
        printf("Warning: could not read equation at line %d!\n", line_no);
        continue;
      }
      
      compute_solutions(&equation);
      print_solutions(equation);
    }
  } else {
    compute_solutions(&equation);
    print_solutions(equation);
  }
  return 0;
}

