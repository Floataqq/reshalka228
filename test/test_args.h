#include "test.h"

#include "arg_parse.h"
#include <cstddef>

const ArgSpecItem args[] = {
  { .long_flag="long",     .arg_type=FLAG, .short_flag='l', .value=NO_VALUE },
  { .long_flag="option",   .arg_type=FLAG, .short_flag='o', .value=OPTIONAL_VALUE },
};
const ArgSpec spec = {
  .len = sizeof(args) / sizeof(args[0]),
  .data = args,
};

void dump_res(ParsedArg *data, size_t len);
void dump_res(ParsedArg *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("- %s - %p\n", data[i].long_flag, data[i].value.str_val);
  }
}

#define P(...) __VA_ARGS__

#define TEST_ARGS(name, argc, _argv, e_status, _e_len, _e_res)           \
  TEST(name) {                                                           \
    ParsedArg res[256] = {};                                             \
    size_t res_len = 0;                                                  \
    const char *argv[] = _argv;                                          \
    ParseStatus status = parse_args(argc, argv, spec, res, &res_len);    \
                                                                         \
    const ParsedArg e_res[] = _e_res;                                    \
    const size_t e_len = _e_len;                                         \
                                                                         \
    ASSERT_EQ(status, e_status);                                         \
                                                                         \
    if(e_status != PARSE_OK)                                             \
      return;                                                            \
                                                                         \
    ASSERT_EQ(res_len, e_len);                                           \
                                                                         \
    for (size_t i = 0; i < res_len; i++) {                               \
      ParsedArg curr   =   res[i];                                       \
      ParsedArg e_curr = e_res[i];                                       \
                                                                         \
                                                                         \
      ASSERT_BOOL(!strcmp(curr.long_flag, e_curr.long_flag));            \
      if ((long)e_curr.value.str_val > 1) {                              \
        ASSERT_BOOL(curr.value.str_val);                                 \
        ASSERT_BOOL(!strcmp(curr.value.str_val, e_curr.value.str_val));  \
      } else {                                                           \
        ASSERT_EQ(curr.value.bool_val, e_curr.value.bool_val);           \
      }                                                                  \
    }                                                                    \
  }                                                                      

const ParsedValue str_val(const char *str);

const ParsedValue bool_true = { .bool_val = true };
const ParsedValue str_val(const char *str) {
  return { .str_val = str };
}

TEST_ARGS(flag_long, 2, P({"", "--long"}), PARSE_OK, 1, P({
    {"long", bool_true}
  }));

TEST_ARGS(flag_short, 2, P({"", "-l"}), PARSE_OK, 1, P({
    {"long", bool_true}
  }));

TEST_ARGS(optional_flag_no_value, 2, P({"", "-o"}), PARSE_OK, 1, P({
    {"option", str_val(NULL)}
  }));

TEST_ARGS(optional_flag_with_value, 3, P({"", "-o", "value"}), PARSE_OK, 1, P({
    {"option", str_val("value")}
  }));

TEST_ARGS(flag_minus_value, 3, P({"", "-o", "-meow"}), INVALID_FLAG, 0, {});

TEST_ARGS(flag_minus_minus, 4, P({"", "-o", "--", "-meow"}), PARSE_OK, 1, P({
    {"option", str_val("-meow")}
  }));





