#include <stdio.h>
#include <string.h>

#include "app_args.h"
#include "equation.h"
#include "log.h"

???

int main(int argc, char *argv[]) {
  Args args = get_args(argc, argv);

  Equation equation = {};

  if (argc != 1) /* input from argv */ {
    if (read_equation_from_argv(&equation, argc, argv))
      return 1;

    compute_solutions (&equation);
    print_solutions   ( equation);
  } else /* input from stdin */ {
    int line_no = 0;
    char line[MAX_LENGTH] = {};

    printf("Enter equation coefficients one by one on different lines:\n");

    while (1) {
      if(!fgets(line, MAX_LENGTH, stdin))
        break;

      const size_t read_cnt = strlen(line);

      if (read_cnt == 0) // empty string
        break;

      if (read_cnt == 1 && line[0] == '\n')
        continue;

      line_no++;
      if (read_equation_from_line(line, &equation)) {
        LOG_WARN("Warning: could not read equation at line %d!", line_no);
        continue;
      }

      compute_solutions (&equation);
      print_solutions   ( equation);
    }
  }

  return 0;
}

void print_solutions(const Equation eq) {
  char eq_str[MAX_LENGTH] = {};
  sprintf(eq_str, "%lg*x^2 + %lg*x + %lg = 0", eq.a, eq.b, eq.c);

  switch (eq.tag) {
    case NOT_COMPUTED:
      LOG_ERROR("Error: %s solutions were not yet computed!", eq_str);
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
      UNREACHABLE("Encountered invalid eq.tag: %d\n", eq.tag);
      break;
  }
}


