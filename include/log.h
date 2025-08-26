/**
 * @file
 * @brief A simple library that provides logging macros
 */

#ifndef FLOATAQQ_LOG_LIB
#define FLOATAQQ_LOG_LIB

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

#include <stdarg.h>

// ------- PROTOTYPES -------

/// An enum describing the output format of logs
typedef enum {
  /// Colored #FL_TXT logs, used by default in terminals
  FL_COLOR,
  /// Text log format: `[<log_level>][<current_function>:<line>] <log_message>`
  FL_TXT,
} FL_LogFormat;

FL_LogFormat  _fl_get_log_format(void);
void           fl_set_log_format(FL_LogFormat format);

extern FL_LogFormat _fl_log_format;
extern bool         _fl_do_logs;

/**
 * Turn logs back on.
 * \relates fl_logs_off
 */
void fl_logs_on (void);

/**
 * Suppress all logs temporarily.
 * \relates fl_logs_on
 */
void fl_logs_off(void);

/// An enum describing log levels
typedef enum {
  /// Debug data, by default white in the terminal (and not printed with NDEBUG)
  FL_DEBUG,
  /// Normal info, by default printed in green
  FL_INFO,
  /// Warnings, printed in yellow
  FL_WARN,
  /// Usually fatal errors, printed in red
  FL_ERROR
} FL_LogLevel;

/// A struct holding some meta information for logs, like the
/// function and line number.
typedef struct {
  /// Current log format
  FL_LogFormat fmt;
  /// Current function
  const char *func;
  /// Current line
  const int   line;
} _FL_LogContext;

/**
 * If called inside a function, returns a #_FL_LogContext relevant to
 * that function.
 */
#define GET_LOG_CONTEXT() (    \
    (_FL_LogContext) {         \
      .fmt = _fl_log_format,   \
      .func = __func__,        \
      .line = __LINE__         \
    })

/**
 * A helper function that actually writes the log using #_fl_log_format.
 *
 * @param level The log level to use.
 * @param ctx   The log context to get metadata from.
 *
 * Other parameters work just like #printf
 *
 * @returns #printf status code
 */
int _fl_write_log(FL_LogLevel level, _FL_LogContext ctx, const char *fmt, ...);

/**
 * Log with \p level log level. Other arguments work like #printf.
 */
#define LOG(level, fmt, ...) _fl_write_log(level, GET_LOG_CONTEXT(), (fmt) __VA_OPT__(,) __VA_ARGS__)

#ifdef NDEBUG
  /**
   * Log with #FL_DEBUG log level. Arguments work like #printf.
   * Doesn't do anything if `NDEBUG` is defined.
   */
  #define LOG_DEBUG(fmt, ...) {}
#else
  /**
   * Log with #FL_DEBUG log level. Arguments work like #printf.
   * Doesn't do anything if `NDEBUG` is defined.
   */
  #define LOG_DEBUG(fmt, ...)  LOG(FL_DEBUG, (fmt) __VA_OPT__(,) __VA_ARGS__)
#endif

/**
 * Log with #FL_INFO log level. Arguments work like in #printf
 */
#define LOG_INFO(fmt, ...)   LOG(FL_INFO,  (fmt) __VA_OPT__(,) __VA_ARGS__)

/**
 * Log with #FL_WARN log level. Arguments work like in #printf
 */
#define LOG_WARN(fmt, ...)   LOG(FL_WARN,  (fmt) __VA_OPT__(,) __VA_ARGS__)

/**
 * Log with #FL_ERROR log level. Arguments work like in #printf
 */
#define LOG_ERROR(fmt, ...)  LOG(FL_ERROR, (fmt) __VA_OPT__(,) __VA_ARGS__)

/// ANSI escape code for white text
#define COLOR_WHITE  "\033[37m"
/// ANSI escape code for green text
#define COLOR_GREEN  "\033[32m"
/// ANSI escape code for yellow text
#define COLOR_YELLOW "\033[33m"
/// ANSI escape code for red text
#define COLOR_RED    "\033[31m"
/// ANSI escape code for resetting styles
#define COLOR_RESET  "\033[0m"

#endif // FLOATAQQ_LOG_LIB

