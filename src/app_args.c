/**
 * @file
 * @brief Argument parsing for the equation solver
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "arg_parse.h"
#include "app_args.h"

int file_validator (const char *file,     char *error);

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
    .long_flag = "equation",
    .arg_type = POSITIONAL,
    .help = "A polynomial literal to solve. If not provided, open a shell",
    .value = OPTIONAL_VALUE,
  }
};

const ArgSpec spec = {
  .len = sizeof(arg_data) / sizeof(arg_data[0]),
  .data = arg_data,
  .synopsis = "Solve quadratic eqautions with ease!",
};

Args get_args (const int argc, const char *argv[]) {
  ParsedArg *output = (ParsedArg *) calloc(256, sizeof(ParsedArg));
  size_t output_len = 0;

  ParseStatus res = parse_args(argc, argv, spec, output, &output_len);

  if (res != PARSE_OK) {
    free(output);
    exit(1);
  }

  Args args = {
    .file = stdin,
    .equation = NULL,
  };

  for (size_t i = 0; i < output_len; i++) {
    ParsedArg current_arg = output[i];

    if (!strcmp(current_arg.long_flag, "file")) {
      args.file = fopen(current_arg.value.str_val, "r");
    } else if (!strcmp(current_arg.long_flag, "equation")) {
      args.equation = current_arg.value.str_val;
    }
  }

  free(output);
  return args;
}

void destroy_args (Args args) {
  fclose(args.file);
}

int file_validator (const char *file, char *error) {
  FILE *res = {};
  res = fopen(file, "r");

  const char *file_error =
    "Could not open file for reading, check if it exists and "
    "if you have permission!";

  if (!res)
    strncpy(error, file_error, MAX_ERROR);
  return !!res;
}


