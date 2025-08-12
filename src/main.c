#include <stdio.h>
#include <math.h>

typedef struct {
  /// x^2 coefficient
  int a;
  /// x coefficient
  int b;
  /// constant coefficient
  int c;
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

  eq->a = a;
  eq->b = b;
  eq->c = c;
  return 1;
}

void print_solutions(Equation eq) {
  double a = eq.a, b = eq.b, c = eq.c;
  double d = b*b - 4 * a*c;
  if (d < 0) {
    printf("Negative discriminant, no solutions!\n");
    return;
  } else if (fabs(d) > 10e-7) {
    printf("Zero discriminant, one solution!\n");
    double x = -b / (2 * a);
    printf(" - x = %lf\n", x);
    return;
  } else {
    printf("Positive discriminant, two solutions!\n");
    double x1 = (-b + sqrt(d)) / (2 * a);
    double x2 = (-b - sqrt(d)) / (2 * a);
    printf(" - x1 = %lf\n"
           " - x2 = %lf\n", x1, x2);
    return;
  }
}

int main(int argc, char **argv) {
  Equation equation;  
  if (!read_equation(&equation, argc, argv))
    return 1;
  
  print_solutions(equation);

  return 0;
}

