/**
 * @file
 * @brief Definitions of functions that help read #Equation data or print it out
 */

#include <stdio.h>
#include <math.h>

#include "equation.h"
#include "log.h"

int read_equation_from_line(char *line, Equation *eq) {
  double a = NAN, b = NAN, c = NAN;
  int result = sscanf(line, "%lf %lf %lf", &a, &b, &c);
  // scanf reads three things
  if (result != 3)
    return 1;

  eq->a = a;
  eq->b = b;
  eq->c = c;
  eq->tag = NOT_COMPUTED;
  return 0;
}

int read_equation_from_argv(Equation *eq,
                            int argc, char *argv[]) {
  int result = 0;

  if (argc != 4) {
    LOG_ERROR("Invalid format - not enough arguments!");
    return 1;
  }

  char input[MAX_LENGTH] = {};
  sprintf(input, "%s %s %s", argv[1], argv[2], argv[3]);

  result = read_equation_from_line(input, eq);
  if (result)
    LOG_ERROR("Invalid format in args!");
  return result;
}


