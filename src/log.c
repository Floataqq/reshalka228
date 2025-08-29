/**
 * @file
 * @brief Logging facilities
 */

#include <stdio.h>
#include <stdarg.h>

#include "log.h"

FL_LogFormat _fl_log_format = _fl_get_log_format();

FL_LogFormat _fl_get_log_format (void) {
  if (isatty(fileno(stdout)))
    return FL_COLOR;
  else
    return FL_TXT;
}

void fl_set_log_format (FL_LogFormat format) {
  _fl_log_format = format;
}

int _fl_write_log_color (FL_LogLevel level, _FL_LogContext ctx,
                         const char *fmt, va_list args);
int _fl_write_log_txt (FL_LogLevel level, _FL_LogContext ctx,
                       const char *fmt, va_list args);

int _fl_write_log (FL_LogLevel level, _FL_LogContext ctx, const char *fmt, ...) {
  if (!_fl_do_logs)
    return 0;

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

int _fl_write_log_txt (FL_LogLevel level, _FL_LogContext ctx,
                       const char *fmt, va_list args) {
  switch(level) {
   case FL_DEBUG:
      printf("[debug][%s:%d] ", ctx.func, ctx.line);
      break;
    case FL_INFO:
      printf("[info][%s:%d] ", ctx.func, ctx.line);
      break;
    case FL_WARN:
      printf("[warn][%s:%d] ", ctx.func, ctx.line);
      break;
    case FL_ERROR:
      printf("[error][%s:%d] ", ctx.func, ctx.line);
      break;
    default:
      printf("[unknown(level = %d)][%s:%d] ", level, ctx.func, ctx.line);
      break;
  }

  return vprintf(fmt, args);
}

int _fl_write_log_color (FL_LogLevel level, _FL_LogContext ctx,
                         const char *fmt, va_list args) {
  switch(level) {
    case FL_DEBUG:
      printf(COLOR_WHITE);
      break;
    case FL_INFO:
      // printf(COLOR_GREEN);
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

bool _fl_do_logs = true;

void fl_logs_on (void) {
  _fl_do_logs = true;
}

void fl_logs_off (void) {
  _fl_do_logs = false;
}


