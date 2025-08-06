#include <command-manager/command-local-handler.hpp>

#include <stdio.h>
#include <string.h>

command_local_handler::command_local_handler(communications_layer_interface *cli)
    : fast_cmd(nullptr), slow_cmd(nullptr), shutdown_cmd(nullptr), cli(cli) {}

int command_local_handler::handle() {
    char command[32] = "";

    int ret = cli->recv(command, sizeof(command));
    if (ret < 0) {
        printf("command_manager::incoming_message recv error'%d'\r\n", ret);
        return ret;
    }

    printf("command_manager::incoming_message '%s'\r\n", command);
    if (strcmp(command::local_fast_cmd_id, command) == 0)
        fast_cmd->execute();
    else if (strcmp(command::local_slow_cmd_id, command) == 0)
        slow_cmd->execute();
    else if (strcmp(command::local_shutdown_cmd_id, command) == 0)
        shutdown_cmd->execute();

    return 0;
}

void command_local_handler::subscribe_fast_cmd(command *cmd) { fast_cmd = cmd; }

void command_local_handler::subscribe_slow_cmd(command *cmd) { slow_cmd = cmd; }

void command_local_handler::subscribe_shutdown_cmd(command *cmd) { shutdown_cmd = cmd; }
