/**
 * @file
 * @brief Argument parsing for the equation solver
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "arg_parse.h"
#include "app_args.h"
#include "log.h"

int file_validator    (const char *file,     char *error);
int prec_validator    (const char *prec,     char *error);
int equation_validator(const char *equation, char *error);

const ArgSpecItem arg_data[] = {
  {
    .long_flag = "file",
    .arg_type = FLAG,
    .short_flag = 'f',
    .help = "The file to read from, if interpreting from file. Default: stdin",
    .value = REQUIRED_VALUE,
    .validator = file_validator,
  },
  {
    .long_flag = "precision",
    .arg_type = FLAG,
    .short_flag = 'p',
    .help = "Calculations precision in decimal digits. Default: 7",
    .value = REQUIRED_VALUE,
    .validator = prec_validator,
  },
  {
    .long_flag = "equation",
    .arg_type = POSITIONAL,
    .help = "Equation string. If it's present, solve the equation. Otherwise, "
            "read from stdin (see -f) and solve.",
    .value = OPTIONAL_VALUE,
    .validator = equation_validator
  },
};

const ArgSpec spec = {
  .len = sizeof(arg_data) / sizeof(arg_data[0]),
  .data = arg_data,
  .synopsis = "Solve quadratic eqautions with ease!",
};

Args get_args(const int argc, const char *argv[]) {
  ParsedArg *output = (ParsedArg *) malloc(sizeof(ParsedArg) * 256);
  size_t output_len = 0;

  ParseStatus res = parse_args(argc, argv, spec, output, &output_len);
  
  if (res != PARSE_OK) {
    free(output);
    exit(1);
  }

  Args args = {
    .precision = 6,
    .file = stdin,
    .equation = NULL,
  };

  for (size_t i = 0; i < output_len; i++) {
    ParsedArg current_arg = output[i];

    if (!strcmp(current_arg.long_flag, "file")) {
      args.file = fopen(current_arg.value.str_val, "r");
    } else if (!strcmp(current_arg.long_flag, "precision")) {
      args.precision = 0;
      int _ = scanf("%u", &args.precision);
    }
  }

  free(output);
  return args;
}

void destroy_args(Args args) {
  fclose(args.file);
  free(args.equation);
}

int file_validator(const char *file, char *error) {
  FILE *res = {};
  res = fopen(file, "r");

  const char *file_error =
    "Could not open file for reading, check if it exists and "
    "if you have permission!";

  if (!res)
    strncpy(error, file_error, MAX_ERROR);
  return !!res;
}

int prec_validator(const char *prec, char *error) {
  unsigned int num = 0;
  int result = sscanf(prec, "%u", &num);

  const char *prec_error = "Invalid format for unsigned integer!";

  if (!result)
    strncpy(error, prec_error, MAX_ERROR);
  return result;
}

int equation_validator(const char *equation, char *error) {
  // TODO

  const char *null_error = "Somehow got a NULL on `equation`! "
                           "This shouldn't happen.";

  if (!equation)
    strncpy(error, null_error, MAX_ERROR);
  return true;
}


