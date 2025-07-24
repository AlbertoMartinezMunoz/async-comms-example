#include <argp.h>
#include <arguments-parser/arguments-parser.hpp>

const char *argp_program_version = "command-manager 1.0";
const char *argp_program_bug_address = "<traquinedes@yahoo.es>";
static char doc[] =
    "command-manager send receives messages using a unix socket";
static char args_doc[] = "local-socket-path remote-socket-path";

struct arguments {
  char *args[2];
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)(state->input);

  switch (key) {
  case ARGP_KEY_ARG:
    if (state->arg_num >= 2)
      /* Too many arguments. */
      argp_usage(state);
    arguments->args[state->arg_num] = arg;
    break;

  case ARGP_KEY_END:
    if (state->arg_num < 2)
      /* Not enough arguments. */
      argp_usage(state);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {0, parse_opt, args_doc, doc, 0, 0, 0};

void arguments_parser::parse(int argc, char **argv) {
  struct arguments arguments;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  local_path = arguments.args[0];
  remote_path = arguments.args[1];
}

const char *arguments_parser::get_local_path(void) {
  return local_path.c_str();
}

const char *arguments_parser::get_remote_path(void) {
  return remote_path.c_str();
}
