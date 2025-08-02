#include <command-manager/command-remote-handler.hpp>

#include <stdio.h>
#include <string.h>

command_remote_handler::command_remote_handler(communications_layer_interface *communications_layer)
    : communications_layer(communications_layer), fast_cmd(nullptr), slow_cmd(nullptr), shutdown_cmd(nullptr) {}

int command_remote_handler::handle() {
    char command[32];

    int ret = communications_layer->recv(command, sizeof(command));
    if (ret < 0) {
        printf("command_manager::incoming_message recv error'%d'\r\n", ret);
        return ret;
    }

    ret = -1;
    command[sizeof(command) - 1] = '\0';
    printf("command_manager::incoming_message '%s'\r\n", command);
    if (strcmp(command::remote_fast_cmd_id, command) == 0)
        ret = fast_cmd->execute();
    else if (strcmp(command::remote_slow_cmd_id, command) == 0)
        ret = slow_cmd->execute();
    else if (strcmp(command::remote_shutdown_cmd_id, command) == 0)
        ret = shutdown_cmd->execute();

    printf("command_manager:incoming_message processed '%s' => %d\r\n", command, ret);
    if (ret == 0)
        communications_layer->send(command::ack, sizeof(command::ack));
    else
        communications_layer->send(command::nack, sizeof(command::nack));

    return 0;
}

void command_remote_handler::subscribe_fast_cmd(command *cmd) { fast_cmd = cmd; }

void command_remote_handler::subscribe_slow_cmd(command *cmd) { slow_cmd = cmd; }

void command_remote_handler::subscribe_shutdown_cmd(command *cmd) { shutdown_cmd = cmd; }
