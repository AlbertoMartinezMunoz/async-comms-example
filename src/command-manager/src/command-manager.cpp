#include <command-manager/command-ids.hpp>
#include <command-manager/command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *communications_layer, socket_transport_layer *socket,
                                 const char *send_cmd_socket_path)
    : communications_layer(communications_layer), socket(socket), send_cmd_socket_path(send_cmd_socket_path),
      fast_cmd(nullptr), slow_cmd(nullptr), shutdown_cmd(nullptr) {}

int command_manager::send_simple_cmd(const char *cmd, size_t cmd_size) {
    char response[32];

    printf("command_manager::send_simple_cmd send '%s' [%zu]\r\n", cmd, cmd_size);
    int ret = socket->connect_socket(send_cmd_socket_path);
    if (ret == -1) {
        perror("fast_command::execute: connect_socket");
        return -2;
    }

    ret = communications_layer->send(cmd, cmd_size);
    if (ret < (int)cmd_size) {
        printf("command_manager::send_simple_cmd send error '%d'\r\n", ret);
        return -1;
    }

    printf("command_manager::send_simple_cmd wait ack\r\n");
    response[0] = '\0';
    ret = communications_layer->recv(response, sizeof(response));
    printf("command_manager::send_simple_cmd ack received[%d]\r\n", ret);
    socket->disconnect_socket();
    if (ret < 0)
        return ret;
    else if (strncmp(command_ids::nack, response, strlen(command_ids::nack)) == 0)
        return 1;
    else if (strncmp(command_ids::ack, response, strlen(command_ids::ack)) == 0)
        return 0;
    else
        return -1;
}

int command_manager::send_fast_cmd() {
    return send_simple_cmd(command_ids::fast_cmd_id, sizeof(command_ids::fast_cmd_id));
}

int command_manager::send_shutdown_cmd() {
    return send_simple_cmd(command_ids::shutdown_cmd_id, sizeof(command_ids::shutdown_cmd_id));
}

int command_manager::send_slow_cmd(char *response_buffer, size_t response_buffer_size) {
    char response[32];

    printf("command_manager::send_slow_cmd send '%s' [%zu]\r\n", command_ids::slow_cmd_id,
           sizeof(command_ids::slow_cmd_id));
    int ret = socket->connect_socket(send_cmd_socket_path);
    if (ret == -1) {
        perror("fast_command::execute: connect_socket");
        return -2;
    }

    ret = communications_layer->send(command_ids::slow_cmd_id, sizeof(command_ids::slow_cmd_id));
    if (ret < (int)sizeof(slow_cmd)) {
        printf("command_manager::send_slow_cmd send error '%d'\r\n", ret);
        return -1;
    }

    printf("command_manager::send_slow_cmd wait ack\r\n");
    ret = communications_layer->recv(response, sizeof(response));
    printf("command_manager::send_slow_cmd ack received [%d]\r\n", ret);
    socket->disconnect_socket();
    if (ret < 0) {
        printf("command_manager::send_slow_cmd ack received '%s' [%d]\r\n", response, ret);
        return ret;
    } else if (strncmp(command_ids::nack, response, strlen(command_ids::nack)) == 0)
        return 1;
    else if (ret > (int)response_buffer_size)
        return -1;
    else if (strncmp(command_ids::ack, response, strlen(command_ids::ack)) == 0) {
        response[ret] = '\0';
        strcpy(response_buffer, response + 4);
        return 0;
    } else {
        printf("send_slow_cmd: unknown error\r\n");
        return -1;
    }
}

int command_manager::incoming_message() const {
    char command[32];

    int ret = communications_layer->recv(command, sizeof(command));
    if (ret < 0) {
        printf("command_manager::incoming_message recv error'%d'\r\n", ret);
        return ret;
    }

    ret = -1;
    command[sizeof(command) - 1] = '\0';
    printf("command_manager::incoming_message '%s'\r\n", command);
    if (strcmp(command_ids::fast_cmd_id, command) == 0)
        ret = fast_cmd->execute();
    else if (strcmp(command_ids::slow_cmd_id, command) == 0)
        ret = slow_cmd->execute();
    else if (strcmp(command_ids::shutdown_cmd_id, command) == 0)
        ret = shutdown_cmd->execute();

    printf("command_manager:incoming_message processed '%s' => %d\r\n", command, ret);
    if (ret == 0)
        communications_layer->send(command_ids::ack, sizeof(command_ids::ack));
    else
        communications_layer->send(command_ids::nack, sizeof(command_ids::nack));

    return 0;
}

void command_manager::subscribe_fast_cmd(command *cmd) { fast_cmd = cmd; }

void command_manager::subscribe_slow_cmd(command *cmd) { slow_cmd = cmd; }

void command_manager::subscribe_shutdown_cmd(command *cmd) { shutdown_cmd = cmd; }
