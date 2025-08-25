#ifndef FLOATAQQ_LIB_ARG_PARSE
#define FLOATAQQ_LIB_ARG_PARSE

#include <stdlib.h>

typedef enum {
  NO_VALUE,
  OPTIONAL_VALUE,
  REQUIRED_VALUE,
} ValueSpecifier;

typedef enum {
  POSITIONAL,
  FLAG,
} ArgType;

extern const size_t MAX_ERROR;

typedef struct {
  const char *long_flag; // both argument id and long flag
  ArgType arg_type;

  char short_flag;
  const char *help;

  ValueSpecifier value;
  int (*validator)(const char *data, char *msg);
} ArgSpecItem;

typedef struct {
  int len;
  const ArgSpecItem *data;

  const char *synopsis;
  const char *footer;
} ArgSpec;

typedef union {
  char *str_val;
  bool bool_val;
} ParsedValue;

typedef struct {
  const char *long_flag;
  ParsedValue value;
} ParsedArg;

typedef enum {
  OK = 0,
  EXTRA_ARGUMENT,
  VALIDATION_ERROR,
  INVALID_FLAG,
  MISSING_ARGUMENT,
  MISSING_VALUE,
  INVALID_VALUE,
} ParseStatus;

ParseStatus parse_args(int argc, char *argv[], ArgSpec spec,
               ParsedArg *output, size_t *output_len);


#endif // FLOATAQQ_LIB_ARG_PARSE
