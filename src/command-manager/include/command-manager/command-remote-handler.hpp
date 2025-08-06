#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_REMOTE_HANDLER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_REMOTE_HANDLER_H_

#include <command-manager/command-handler.hpp>

#include <commands/commands.hpp>
#include <communications-layer/communications-layer.hpp>

class command_remote_handler : public command_handler {
  public:
    command_remote_handler(communications_layer_interface *communications_layer);

    virtual ~command_remote_handler() {}

    int handle() override;

    void subscribe_fast_cmd(command *cmd);

    void subscribe_slow_cmd(command *cmd);

    void subscribe_shutdown_cmd(command *cmd);

  private:
    communications_layer_interface *communications_layer;

    command *fast_cmd;
    command *slow_cmd;
    command *shutdown_cmd;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_REMOTE_HANDLER_H_
