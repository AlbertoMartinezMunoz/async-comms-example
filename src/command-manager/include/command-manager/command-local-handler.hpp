#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_LOCAL_HANDLER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_LOCAL_HANDLER_H_

#include <command-manager/command-handler.hpp>

#include <commands/commands.hpp>
#include <communications-layer/communications-layer.hpp>

class command_local_handler : public command_handler {
  public:
    command_local_handler(communications_layer_interface *cli);

    virtual ~command_local_handler() {}

    int handle() override;

    void subscribe_fast_cmd(command *cmd);

    void subscribe_slow_cmd(command *cmd);

    void subscribe_shutdown_cmd(command *cmd);

  private:
    command *fast_cmd;
    command *slow_cmd;
    command *shutdown_cmd;

    communications_layer_interface *cli;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_LOCAL_HANDLER_H_
