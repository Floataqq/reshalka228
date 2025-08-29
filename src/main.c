#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "app_args.h"
#include "evaluate.h"
#include "parser.h"
#include "arith.h"
#include "log.h"
#include "polynomial.h"

#define POLTORASHKA_URL "https://ded32.synology.me/~mipt-photo/photo/#!Search/album_323032323031303120d09fd0bed0bbd182d0bed180d0b0d188d0bad0b02f323032313034313320d09fd0bed0bbd182d0bed180d0b0d188d0bad0b0"
#define PORNO_URL "https://vk.com/video63300907_456239570"

void shell (Args args);
EvalStatus eval_and_handle_errors (Env *env, Statement *command, Value *value);
int execute_command (Env *env, const char *source);
void open_url (const char *url);
const int MAX_SOURCE_LEN = 1024;


int main (int argc, const char *argv[]) {
  Args args = get_args(argc, argv);

  if (args.equation) {
    char solve_cmd[MAX_SOURCE_LEN + strlen("solve ")] = {};
    strcpy(solve_cmd, "solve ");
    strcat(solve_cmd, args.equation);

    execute_command({}, solve_cmd);
  } else
    shell(args);

  return 0;
}

void shell (Args args) {
  Env env = {};

  while (true) {
    if (fileno(args.file) == STDIN_FILENO)
      printf("> ");

    char source[MAX_SOURCE_LEN];

    char *line = fgets(source, MAX_SOURCE_LEN, args.file);
    if (!line)
      return;

    source[strcspn(source, "\n")] = '\0';
    if (strlen(source) == 0)
      continue;

    execute_command(&env, source);
  }
}

int execute_command (Env *env, const char *source) {
  Statement *command = (Statement *) calloc(1, sizeof(Statement));
  int res = parse_stmt(source, command);

  if (!res) {
    LOG_ERROR("Could not parse command!");
    return 0;
  }

  int status = EVAL_OK;
  Value val = {};
  Solutions sols = {};

  switch (command->cmd) {
    case CMD_LET:
      status = eval_and_handle_errors(env, command, &val);
      if (status)
        break;

      env_set_value(env, command->var, val);
      break;
    case CMD_SOLVE:
      status = eval_and_handle_errors(env, command, &val);
      if (status)
        break;

      if (val.type != TP_POLYNOMIAL) {
        LOG_ERROR("Expected a polynomial as an argument to solve, but got a number!");
        break;
      }

      status = solve_polynomial(val.poly, &sols);
      if (!status) {
        LOG_ERROR("Could not solve this polynomial! Deg-4 and some deg-3 polys are not yet supported :(");
        break;
      }

      printf("-> ");
      print_polynomial(val.poly);
      putchar('\n');

      printf("-> ");
      print_solutions(sols);
      break;
    case CMD_POLTORASHKA:
      open_url(POLTORASHKA_URL);
      break;
    case CMD_PORNO:
      open_url(PORNO_URL);
      break;
    case CMD_EXPR:
      status = eval_and_handle_errors(env, command, &val);
      if (status)
        break;

      printf("-> ");
      print_value(val);
      break;
    default:
      LOG_ERROR("Unknown command");
  }

  destory_stmt(command);
  return status;
}

EvalStatus eval_and_handle_errors (Env *env, Statement *command, Value *val) {
  EvalStatus status = eval_expr(env, command->expr, val);
  switch (status) {
    case EVAL_OK:
      break;
    case TOO_LARGE_DEGREE:
      LOG_ERROR("Encountered a polynomial of degree larger than 4, which is currently not supported");
      break;
    case ZERO_DIVISION:
      LOG_ERROR("Encountered division by zero!");
      break;
    case NO_VARIABLE:
      LOG_ERROR("An unknown variable was referenced!");
      break;
    case DIFFERENT_POLY_VAR:
      LOG_ERROR("Multivariable polynomials are not yet supported!");
      break;
    case TYPE_ERROR:
      LOG_ERROR("Encountered type error!");
      break;
    case COMPLEX_POWER:
      LOG_ERROR("Invalid power operation! Currently, you can only raise a rational number to a"
                "rational power, a complex number to an integer power, or a polynomial to an integer power");
      break;
    case WTF_ERROR:
    default:
      LOG_ERROR("My brain exploded! Something went really wrong...");
      break;
  }

  return status;
}

void open_url (const char *url) {
  char command[1024];
  sprintf(command, "firefox --new-tab %s", url);
  int status = system(command);

  if (status)
    printf("Check your browser!\n");
  else {
    LOG_ERROR("Failed to open a browser tab :(");
    LOG_ERROR("You can try to visit %s yourself though", url);
  }
}
