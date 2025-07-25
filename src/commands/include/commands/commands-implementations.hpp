#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPEMETATIONS_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPEMETATIONS_H_
#include <commands/commands.hpp>
#include <commands/shutdown-receiver.hpp>

class local_fast_command : public command {
public:
  int execute() const override;
};

class local_slow_command : public command {
public:
  int execute() const override;
};

class local_shutdown_command : public command {
public:
  local_shutdown_command(shutdown_receiver *comms, shutdown_receiver *cli);
  int execute() const override;

private:
  shutdown_receiver *comms;
  shutdown_receiver *cli;
};

class remote_fast_command : public command {
public:
  int execute() const override;
};

class remote_slow_command : public command {
public:
  int execute() const override;
};

class remote_shutdown_command : public command {
public:
  remote_shutdown_command(shutdown_receiver *comms, shutdown_receiver *cli);
  int execute() const override;

private:
  shutdown_receiver *comms;
  shutdown_receiver *cli;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPEMETATIONS_H_
