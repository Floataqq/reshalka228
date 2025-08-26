/**
 * @file
 * @brief General argument parsing library
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "arg_parse.h"
#include "log.h"

const size_t MAX_ERROR = 1024;

ParsedValue value_str (char *val);
ParsedValue value_bool(bool  val);

const ArgSpecItem *get_spec_by_long_flag(char  *flag,  ArgSpec spec);
const ArgSpecItem *get_spec_by_shorthand(char   flag,  ArgSpec spec);
const ArgSpecItem *get_ith_posiional_arg(size_t index, ArgSpec spec);

void add_parsed_arg(ParsedArg *out, size_t *len, ParsedArg item);

ParseStatus try_parse_value(int argc, char *argv[], const ArgSpecItem *spec, int *current_arg, ParsedValue *item);

ParseStatus parse_long_flag(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    ParsedArg *output, size_t *output_len);

ParseStatus parse_shorthand_chain(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    ParsedArg *output, size_t *output_len);

ParseStatus parse_positional_argument(
    ArgSpec spec, char *arg, size_t positional_index, int *current_arg,
    ParsedArg *output, size_t *output_len);

ParseStatus try_parse_trailing_value(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    char flag, ParsedArg *output, size_t *output_len);

ParseStatus run_validator(const ArgSpecItem *spec, const char *arg);

[[noreturn]] void print_help_and_exit(ArgSpec spec, char *progname);
void print_usage(ArgSpec spec, char *progname);

ParseStatus parse_args(int argc, char *argv[], ArgSpec spec,
                       ParsedArg *output, size_t *output_len) {
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
      print_help_and_exit(spec, argv[0]);
  }

  int i = 1;
  size_t positional_index = 0;

  while (i < argc) {
    char *argument = argv[i];
    size_t argument_len = strlen(argument);

    if (argument_len > 1 && argument[0] == '-' && argument[1] == '-') { // long flag
      LOG_DEBUG("Long flag `%s`", argv[i]);
      ParseStatus res = parse_long_flag(spec, argc, argv, &i, output, output_len);

      if (res != PARSE_OK)
        return res;
    } else if (argument_len && argument[0] == '-') {                    // sequence of short flags
      LOG_DEBUG("Short flags `%s`", argv[i]);
      ParseStatus res = parse_shorthand_chain(spec, argc, argv, &i, output, output_len);

      if (res != PARSE_OK)
        return res;
    } else {                                                            // positional argument
      LOG_DEBUG("Positional argument `%s`", argv[i]);
      ParseStatus res = parse_positional_argument(spec, argv[i], positional_index, &i, output, output_len);

      if (res != PARSE_OK)
        return res;

      positional_index++;
    }
  }

  // check if some positional arguments are missing
  const ArgSpecItem *pos_spec = get_ith_posiional_arg(positional_index, spec);
  if (pos_spec) {
    LOG_ERROR("Missing positional argument %s!", pos_spec->long_flag);
    return MISSING_VALUE;
  }

  return PARSE_OK;
}

ParseStatus parse_long_flag(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    ParsedArg *output, size_t *output_len) {

  char *flag = argv[*current_arg];
  (*current_arg)++;

  // skip the --
  const ArgSpecItem *arg_spec = get_spec_by_long_flag(flag + 2, spec);
  if (!arg_spec) {
    LOG_ERROR("Unknown long flag: `%s`", flag);
    return INVALID_FLAG;
  }

  if (arg_spec->value == NO_VALUE) {
    // no value - dont't parse and return
    add_parsed_arg(output, output_len, { arg_spec->long_flag, value_str(NULL) });
    return PARSE_OK;
  } else {
    // otherwise try to parse value
    ParsedValue value;
    ParseStatus res = try_parse_value(argc, argv, arg_spec, current_arg, &value);

    if (res == PARSE_OK) {
      add_parsed_arg(output, output_len, { arg_spec->long_flag, value });
      return PARSE_OK;
    } else if (res == MISSING_VALUE) {
      // if the value is required, fail. otherwise return NULL
      if (arg_spec->value == OPTIONAL_VALUE) {
        add_parsed_arg(output, output_len, { arg_spec->long_flag, value_str(NULL) });
        return PARSE_OK;
      } else {
        LOG_ERROR("Long flag %s requires a value!", flag);
        return MISSING_VALUE;
      }
    } else {
      return res;
    }
  }
  return PARSE_OK;
}

ParseStatus parse_shorthand_chain(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    ParsedArg *output, size_t *output_len) {

  char  *chain = argv[*current_arg];
  size_t chain_len = strlen(chain);

  (*current_arg)++;

  // flags don't have value except for last one
  for (size_t i = 1; i < chain_len - 1; i++) {
    const ArgSpecItem *curr_spec = get_spec_by_shorthand(chain[i], spec);
    if (!curr_spec) {
      LOG_ERROR("Invalid shorthand `-%c`!", chain[i]);
      return INVALID_FLAG;
    }

    switch (curr_spec->value) {
    case NO_VALUE:
      add_parsed_arg(output, output_len, { curr_spec->long_flag, value_bool(true) });
      break;
    case OPTIONAL_VALUE:
      add_parsed_arg(output, output_len, { curr_spec->long_flag, value_str(NULL) });
      break;
    case REQUIRED_VALUE:
      LOG_ERROR("Shorthand -%c (--%s) requires a value!", chain[i], curr_spec->long_flag);
      return MISSING_VALUE;
    default:
      break;
    }
  }

  return try_parse_trailing_value(spec, argc, argv, current_arg, chain[chain_len - 1], output, output_len);
}

ParseStatus parse_positional_argument(
    ArgSpec spec, char *arg, size_t positional_index, int *current_arg,
    ParsedArg *output, size_t *output_len) {
  (*current_arg)++;
  const ArgSpecItem *curr_spec = get_ith_posiional_arg(positional_index, spec);
  if (!curr_spec) {
    LOG_ERROR("Extra positional argument: `%s`", arg);
    return EXTRA_ARGUMENT;
  }

  ParseStatus res = run_validator(curr_spec, arg);
  if (res != PARSE_OK)
    return res;

  add_parsed_arg(output, output_len, { curr_spec->long_flag, value_str(arg) });
  return PARSE_OK;
}


ParseStatus try_parse_trailing_value(
    ArgSpec spec, int argc, char *argv[], int *current_arg,
    char flag, ParsedArg *output, size_t *output_len) {

  const ArgSpecItem *curr_spec = get_spec_by_shorthand(flag, spec);
  if (!curr_spec) {
    LOG_ERROR("Invalid shorthand `-%c`!", flag);
    return INVALID_FLAG;
  }

  if(curr_spec->value == NO_VALUE) {
    add_parsed_arg(output, output_len, { curr_spec->long_flag, value_bool(true) });
    return PARSE_OK;
  } else {
    ParsedValue value = {};
    ParseStatus res = try_parse_value(argc, argv, curr_spec, current_arg, &value);

    if (res == PARSE_OK) {
      add_parsed_arg(output, output_len, { curr_spec->long_flag, value });
      return PARSE_OK;
    } else if (res == MISSING_VALUE) {
      if (curr_spec->value == OPTIONAL_VALUE) {
        add_parsed_arg(output, output_len, { curr_spec->long_flag, value_str(NULL) });
        return PARSE_OK;
      } else {
        LOG_ERROR("Missing value for shorthand -%c (--%s)!", flag, curr_spec->long_flag);
        return MISSING_VALUE;
      }
    } else {
      return res;
    }
  }
}

ParseStatus try_parse_value(int argc, char *argv[], const ArgSpecItem *spec, int *current_arg, ParsedValue *item) {
  if (!strcmp(argv[*current_arg], "--")) {
  // if the next arg is --, eat the next value anyhow
    if (*current_arg + 1 == argc) {
      LOG_ERROR("A `--` can't be the last argument. If you want to pass in a literal `--`, do it in quotes.");
      return INVALID_VALUE;
    } else {
      (*current_arg)++;
      *item = value_str(argv[*current_arg]);
      (*current_arg)++;
    }
  } else if (argv[*current_arg][0] == '-') {
  // the next argument here will be treated as a flag
    return MISSING_VALUE;
  } else {
  // just eat the value
    *item = value_str(argv[*current_arg]);
    (*current_arg)++;
  }

  ParseStatus res = run_validator(spec, item->str_val);
  if (res != PARSE_OK)
    return res;

  return PARSE_OK;
}

const ArgSpecItem *get_spec_by_long_flag(char *flag, ArgSpec spec) {
  for (int i = 0; i < spec.len; i++) {
    if (!strcmp(spec.data[i].long_flag, flag))
      return &spec.data[i];
  }

  return NULL;
}

const ArgSpecItem *get_spec_by_shorthand(char flag, ArgSpec spec) {
  for (int i = 0; i < spec.len; i++) {
    if (spec.data[i].short_flag == flag)
      return &spec.data[i];
  }

  return NULL;
}

const ArgSpecItem *get_ith_posiional_arg(size_t index, ArgSpec spec) {
  size_t pos_count = 0;
  for (int i = 0; i < spec.len; i++) {
    if (spec.data[i].arg_type == POSITIONAL)
      pos_count++;

    // indexes start from 0
    if (pos_count == index + 1)
      return &spec.data[i];
  }

  return NULL;
}

ParseStatus run_validator(const ArgSpecItem *spec, const char *arg) {
  char error[MAX_ERROR] = {};
  if (spec->validator && !spec->validator(arg, error)) {
    if (strlen(error))
      LOG_ERROR("Invalid argument `%s` (%s) - %s!", arg, spec->long_flag, error);
    else
      LOG_ERROR("Invalid argument `%s` (%s)!", arg, spec->long_flag);
    return INVALID_VALUE;
  }

  return PARSE_OK;
}

void add_parsed_arg(ParsedArg *out, size_t *len, ParsedArg item) {
  out[(*len)++] = item;
}

ParsedValue value_str(char *val) {
  return ParsedValue { .str_val =  val };
}

ParsedValue value_bool(bool val) {
  return ParsedValue { .bool_val = val };
}

[[noreturn]] void print_help_and_exit(ArgSpec spec, char *progname) {
  print_usage(spec, progname);

  if (spec.synopsis)
    printf("%s\n", spec.synopsis);
  putchar('\n');

  // flags first
  printf("Options:\n");
  for (int i = 0; i < spec.len; i++) {
    ArgSpecItem curr_arg = spec.data[i];
    if (curr_arg.arg_type != FLAG)
      continue;

    printf("  ");
    if (curr_arg.short_flag)
      printf("-%c, ", curr_arg.short_flag);
    else
      printf("    ");

    printf("--%-20s", curr_arg.long_flag);

    if (curr_arg.help)
      printf("%s", curr_arg.help);

    putchar('\n');
  }

  // positionals later
  printf("Arguments:\n");
  for (int i = 0; i < spec.len; i++) {
    ArgSpecItem curr_arg = spec.data[i];
    if (curr_arg.arg_type != POSITIONAL)
      continue;

    printf("  %s", curr_arg.long_flag);
    if (curr_arg.help)
      printf(" %s", curr_arg.help);
    putchar('\n');
  }

  printf("\n\n");
  if (spec.footer)
    printf("%s", spec.footer);

  exit(0);
}

void print_usage(ArgSpec spec, char *progname) {
  printf("Usage: %s [options]... ", progname);

  size_t i = 0;
  const ArgSpecItem *curr_spec;
  while ((curr_spec = get_ith_posiional_arg(i, spec))) {
    i++;
    printf("[%s] ", curr_spec->long_flag);
    putchar('\n');
  }
}


