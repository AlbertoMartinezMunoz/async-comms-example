#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_COMMANDS_INCLUDE_INTERACTIVE_COMMANDS_INTERACTIVE_COMMANDS_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_COMMANDS_INCLUDE_INTERACTIVE_COMMANDS_INTERACTIVE_COMMANDS_H_

#include <command-manager/command-manager.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <interactive-console/interactive-console-command.hpp>
#include <interactive-console/interactive-console.hpp>

class shutdown_command : public interactive_console_command {
public:
  shutdown_command(command_manager *cmd_mngr, socket_transport_layer *socket,
                   interactive_console *cli);
  void execute() const override;

private:
  command_manager *cmd_mngr;
  socket_transport_layer *socket;
  interactive_console *cli;
};

class fast_command : public interactive_console_command {
public:
  fast_command(command_manager *cmd_mngr);
  void execute() const override;

private:
  command_manager *cmd_mngr;
};

class slow_command : public interactive_console_command {
public:
  slow_command(command_manager *cmd_mngr);
  void execute() const override;

private:
  command_manager *cmd_mngr;
};

#endif // IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_COMMANDS_INCLUDE_INTERACTIVE_COMMANDS_INTERACTIVE_COMMANDS_H_
