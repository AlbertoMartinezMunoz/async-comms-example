#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPLEMENTATIONS_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPLEMENTATIONS_H_
#include <commands/commands.hpp>
#include <commands/local-command-receiver.hpp>
#include <commands/shutdown-receiver.hpp>
#include <communications-layer/communications-layer.hpp>

class local_fast_command : public command {
  public:
    local_fast_command(communications_layer *comms);
    int execute() const override;

  private:
    communications_layer *comms;
};

class local_slow_command : public command {
  public:
    local_slow_command(communications_layer *comms);
    int execute() const override;

  private:
    communications_layer *comms;
};

class local_shutdown_command : public command {
  public:
    local_shutdown_command(communications_layer *comms, shutdown_receiver *listener, shutdown_receiver *cli);
    int execute() const override;

  private:
    communications_layer *comms;
    shutdown_receiver *listener;
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

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_IMPLEMENTATIONS_H_
