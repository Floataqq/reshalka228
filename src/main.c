#include <stdio.h>
#include <string.h>
#include "lib.h"

int main(int argc, const char *argv[]) {
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
        printf("Warning: could not read equation at line %d!\n", line_no);
        continue;
      }

      compute_solutions (&equation);
      print_solutions   ( equation);
    }
  }

  return 0;
}
