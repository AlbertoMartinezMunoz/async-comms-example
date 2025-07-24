#ifndef IOT_MICRO_FIRMWARE_SRC_ARGUMENTS_PARSER_INCLUDE_ARGUMENTS_PARSER_ARGUMENTS_PARSER_H_
#define IOT_MICRO_FIRMWARE_SRC_ARGUMENTS_PARSER_INCLUDE_ARGUMENTS_PARSER_ARGUMENTS_PARSER_H_

#include <string>

class arguments_parser {
public:
  arguments_parser() : local_path(""), remote_path("") {}

  void parse(int argc, char **argv);

  const char *get_local_path(void);

  const char *get_remote_path(void);

private:
  std::string local_path;
  std::string remote_path;
};

#endif // IOT_MICRO_FIRMWARE_SRC_ARGUMENTS_PARSER_INCLUDE_ARGUMENTS_PARSER_ARGUMENTS_PARSER_H_
