#include <command-manager/command-ids.hpp>
#include <command-manager/command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *communications_layer, socket_transport_layer *socket,
                                 const char *send_cmd_socket_path)
    : communications_layer(communications_layer), socket(socket), send_cmd_socket_path(send_cmd_socket_path) {}

int command_manager::send_simple_cmd(const char *cmd, size_t cmd_size) {
    char response[32];

    printf("command_manager::send_simple_cmd send '%s' [%zu]\r\n", cmd, cmd_size);
    int ret = socket->connect();
    if (ret == -1) {
        perror("fast_command::execute: connect");
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
    socket->disconnect();
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
    int ret = socket->connect();
    if (ret == -1) {
        perror("fast_command::execute: connect");
        return -2;
    }

    ret = communications_layer->send(command_ids::slow_cmd_id, sizeof(command_ids::slow_cmd_id));
    if (ret < (int)sizeof(command_ids::slow_cmd_id)) {
        printf("command_manager::send_slow_cmd send error '%d'\r\n", ret);
        return -1;
    }

    printf("command_manager::send_slow_cmd wait ack\r\n");
    ret = communications_layer->recv(response, sizeof(response));
    printf("command_manager::send_slow_cmd ack received [%d]\r\n", ret);
    socket->disconnect();
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
