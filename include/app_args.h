/**
 * @file
 * @brief A simple little bit of code that parses arguments from #ParsedArg s
          to the app's #Args
 */

#ifndef FLOATAQQ_LIB_APP_ARGS
#define FLOATAQQ_LIB_APP_ARGS


#include <stdio.h>

/// A structure for holding the equation solver's command line arguments.
typedef struct {
  /// Float calculations precision, specified in decimal digits.
  unsigned int precision;
  /// A file handle to output to. Default is stdout.
  FILE *file;
  /// A string that specifies the equation from command line args. If
  /// there isn't an equation in the arguments, this will be NULL.
  char *equation;
} Args;

/*
 * Get the #Args struct from command line, while nicely handling errors.
 */
Args get_args(int argc, char *arg[]);

/*
 * A destructor for #Args. Must call before exiting application.
 */
void destroy_args(Args args);


#endif // FLOATAQQ_LIB_APP_ARGS
