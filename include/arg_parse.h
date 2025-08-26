/**
 * @file
 * @brief A fairly simple library for parsing command-line arguments
 */

#ifndef FLOATAQQ_LIB_ARG_PARSE
#define FLOATAQQ_LIB_ARG_PARSE

#include <stdlib.h>

/// Specifies whether this flag has a value and if it's optional.
typedef enum {
  /// Flag has no value, it's value in #ParsedArg is a `true` if the flag was
  /// present, otherwise `false`.
  NO_VALUE,
  /// Optional value, it's value in #ParsedArg is a possibly-null string
  OPTIONAL_VALUE,
  /// Required value, it's value in #ParsedArg is a non-null string
  REQUIRED_VALUE,
} ValueSpecifier;

/// Specifies the argument type - positional args or flags.
typedef enum {
  /// Positional argument
  POSITIONAL,
  /// Switch, potentially with a value
  FLAG,
} ArgType;

/// Maximum length of validator error.
extern const size_t MAX_ERROR;

/// A description of a command-line argument.
typedef struct {
  /// If the argument is a flag, it will get parsed in `--long_flag` format.
  /// It is also an internal argument ID, and is required.
  const char *long_flag;

  /// Argument type: positional or flag.
  ArgType arg_type;

  /// Optional shot flag, e.g. for 'file', it can be 'f', so that
  /// '--file' can be also abbreviated to '-f'.
  char short_flag;

  /// Optional message that will be shown for this argument in the help menu.
  const char *help;

  /// No value / Optional value / Required value
  ValueSpecifier value;

  /// Optional validation function. It should return a zero to fail, or anything
  /// else to succeed. If it fails, it can write an error message to the
  /// \p msg buffer. The buffer is at least #MAX_ERROR long.
  int (*validator)(const char *data, char *msg);
} ArgSpecItem;

/// A description of all command-line arguments of a program
typedef struct {
  /// How many `ArgSpecItem`s do we have in our description
  int len;
  /// Pointer to the `ArgSpecItem`s
  const ArgSpecItem *data;

  /// An optional short explanation of how the program works. Printed in
  /// the help menu
  const char *synopsis;
  /// An optional footer to be printed in the end of the help menu.
  const char *footer;
} ArgSpec;

/**
 * A parsed value - either a bool for on/off flags, or a string for
 * flags with values
 */
typedef union {
  /// A string value for arguments with values
  const char *str_val;
  /// A bool for on/off flags
  bool bool_val;
} ParsedValue;

/// A parsed argument with it's value.
typedef struct {
  /// The argument's #ArgSpecItem.long_flag
  const char *long_flag;
  /// The argument's parsed value
  ParsedValue value;
} ParsedArg;

/// Parsing result of an argument
typedef enum {
  /// Everything was parsed correctly
  PARSE_OK = 0,
  /// There is an extraneuos positional argument
  EXTRA_ARGUMENT,
  /// Encountered an unknown flag or shorthand
  INVALID_FLAG,
  /// A mandatory positional argument was not provided
  MISSING_ARGUMENT,
  /// The flag required an argument, but it wasn't provided
  MISSING_VALUE,
  /// A validator returned a non-zero value
  INVALID_VALUE,
} ParseStatus;

/**
 * Parse command-line arguments to a vector of #ParsedArg. Log errors nicely and
 * Handle -h / --help.
 *
 * @param argc       The program's argc
 * @param argv       The program's argv
 * @param spec       A description of how to parse arguments
 * @param output     An array of #ParsedArg to write to
 * @param output_len Pointer to the length of \p output. Will be modified
 *
 * @returns A #ParseStatus - either #PARSE_OK or some error.
 */
ParseStatus parse_args(const int argc, const char *argv[], ArgSpec spec,
                       ParsedArg *output, size_t *output_len);


#endif // FLOATAQQ_LIB_ARG_PARSE
