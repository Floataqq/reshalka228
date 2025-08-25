#ifndef FLOATAQQ_LIB_APP_ARGS
#define FLOATAQQ_LIB_APP_ARGS


#include <stdio.h>

typedef struct {
  unsigned int precision;
  FILE *file;
  char *equation;
} Args;

Args get_args(int argc, char *arg[]);
void destroy_args(Args args);


#endif // FLOATAQQ_LIB_APP_ARGS
