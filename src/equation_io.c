/**
 * @file
 * @brief Definitions of functions that help read #Equation data or print it out
 */

#include <stdio.h>

#include "lib.h"

int read_equation_from_line(const char *const line, Equation *eq) {
  double a = 0, b = 0, c = 0;
  int result = 0;

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

int read_equation_from_argv(Equation *const eq, 
                            const int argc, const char *argv[]) {
  int result = 0;
  
  if (argc != 4) 
  {
    printf("Error: invalid format - not enough arguments!\n");
    return 1;
  }

  char input[MAX_LENGTH] = {};
  sprintf(input, "%s %s %s", argv[1], argv[2], argv[3]);
  
  result = read_equation_from_line(input, eq);
  if (result)
    printf("Error: invalid format in args!\n");
  return result;
}

void print_solutions(const Equation eq) {
  char eq_str[MAX_LENGTH] = {};
  sprintf(eq_str, "%lg*x^2 + %lg*x + %lg = 0", eq.a, eq.b, eq.c);

  switch (eq.tag) 
  {
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
      break;
    default:
      UNREACHABLE();
      break;
  }
}

