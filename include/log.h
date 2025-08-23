/**
 * @file
 * @brief A simple header-only library that provides logging macros
 */

#ifndef FLOATAQQ_LOG_LIB
#define FLOATAQQ_LOG_LIB

#include <cstdarg>
#ifdef __unix__
  #define FL_UNIX
#elif defined(_WIN32) || defined(WIN32)
  #define FL_WIN32
#endif

// both files export isatty()
#if   defined(FL_UNIX)
  #include <unistd.h>
#elif defined(FL_WIN32)
  #include <io.h>
#endif

#include <stdio.h>
#include <stdarg.h>

// ------- PROTOTYPES -------

typedef enum {
  FL_COLOR,
  FL_TXT,
} FL_LogFormat;

FL_LogFormat  _fl_get_log_format(void);
void           fl_set_log_format(FL_LogFormat format);

FL_LogFormat _fl_log_format = _fl_get_log_format();


typedef enum {
  FL_DEBUG,
  FL_INFO,
  FL_WARN,
  FL_ERROR
} FL_LogLevel;

typedef struct {
  FL_LogFormat fmt;
  const char * func;
} _FL_LogContext;

#define GET_LOG_CONTEXT() (    \
    (_FL_LogContext) {         \
      .fmt = _fl_log_format,   \
      .func = __func__         \
    })

int _fl_write_log(FL_LogLevel level, _FL_LogContext ctx, const char *fmt, ...);

#define LOG(level, fmt, ...) _fl_write_log(level, GET_LOG_CONTEXT(), fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_DEBUG(fmt, ...)  LOG(FL_DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(fmt, ...)   LOG(FL_INFO,  fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(fmt, ...)   LOG(FL_WARN,  fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(fmt, ...)  LOG(FL_ERROR, fmt __VA_OPT__(,) __VA_ARGS__)

#define COLOR_WHITE  "\033[37m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"

// ------- DEFINITIONS -------

FL_LogFormat _fl_get_log_format(void) {
  if (isatty(fileno(stdout)))
    return FL_COLOR;
  else
    return FL_TXT;
}

void fl_set_log_format(FL_LogFormat format) {
  _fl_log_format = format;
}

int _fl_write_log_txt   (FL_LogLevel level, _FL_LogContext ctx, const char *fmt, va_list args);
int _fl_write_log_color (FL_LogLevel level, _FL_LogContext ctx, const char *fmt, va_list args);

int _fl_write_log(FL_LogLevel level, _FL_LogContext ctx, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  
  int result = 0;

  switch (_fl_log_format) {
  case FL_TXT:
    result = _fl_write_log_txt   (level, ctx, fmt, args);
    break;
  case FL_COLOR:
    result = _fl_write_log_color (level, ctx, fmt, args);
    break;
  default:
    result = printf("Uknown log format value: %d\n", _fl_log_format);
    break;
  }

  putchar('\n');

  va_end(args);

  return result;
}

int _fl_write_log_txt(FL_LogLevel level, _FL_LogContext ctx, 
                      const char *fmt, va_list args) {
  switch(level) {
  case FL_DEBUG:
    printf("[debug][%s] ", ctx.func);
    break;
  case FL_INFO:
    printf("[info][%s] ", ctx.func);
    break;
  case FL_WARN:
    printf("[warn][%s] ", ctx.func);
    break;
  case FL_ERROR:
    printf("[error][%s] ", ctx.func);
    break;
  default:
    printf("[unknown(level = %d)][%s] ", level, ctx.func);
    break;
  }

  return vprintf(fmt, args);
}

int _fl_write_log_color(FL_LogLevel level, _FL_LogContext ctx, 
                      const char *fmt, va_list args) {
  switch(level) {
  case FL_DEBUG:
    printf(COLOR_WHITE);
    break;
  case FL_INFO:
    printf(COLOR_GREEN);
    break;
  case FL_WARN:
    printf(COLOR_YELLOW);
    break;
  case FL_ERROR:
    printf(COLOR_RED);
    break;
  default:
    break;
  }

  int result = _fl_write_log_txt(level, ctx, fmt, args);
  
  printf(COLOR_RESET);

  return result;
}

#endif // FLOATAQQ_LOG_LIB

