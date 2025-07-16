#include <remote-command-manager/remote-command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *communications_layer)
    : communications_layer(communications_layer), fast_cmd_observer(nullptr), slow_cmd_observer(nullptr), shutdown_cmd_observer(nullptr) {}

int command_manager::send_simple_cmd(const char *cmd, size_t cmd_size)
{
    char response[32];

    printf("command_manager::send_simple_cmd send '%s' [%zu]\r\n", cmd, cmd_size);
    int ret = communications_layer->send(cmd, cmd_size);
    if (ret < (int)cmd_size)
    {
        printf("command_manager::send_simple_cmd send error '%d'\r\n", ret);
        return -1;
    }

    printf("command_manager::send_simple_cmd wait ack\r\n");
    ret = communications_layer->recv(response, sizeof(response));
    printf("command_manager::send_simple_cmd ack received[%d]\r\n", ret);
    if (ret < 0)
        return ret;
    else if (strncmp(nack, response, strlen(nack)) == 0)
        return 1;
    else if (strncmp(ack, response, strlen(ack)) == 0)
        return 0;
    else
        return -1;
}

int command_manager::send_fast_cmd()
{
    return send_simple_cmd(fast_cmd, sizeof(fast_cmd));
}

int command_manager::send_shutdown_cmd()
{
    return send_simple_cmd(shutdown_cmd, sizeof(shutdown_cmd));
}

int command_manager::send_slow_cmd(char *response_buffer, size_t response_buffer_size)
{
    char response[32];

    printf("command_manager::send_slow_cmd send '%s' [%zu]\r\n", slow_cmd, sizeof(slow_cmd));
    int ret = communications_layer->send(slow_cmd, sizeof(slow_cmd));
    if (ret < (int)sizeof(slow_cmd))
    {
        printf("command_manager::send_slow_cmd send error '%d'\r\n", ret);
        return -1;
    }

    printf("command_manager::send_slow_cmd wait ack\r\n");
    ret = communications_layer->recv(response, sizeof(response));
    printf("command_manager::send_slow_cmd ack received [%d]\r\n", ret);
    if (ret < 0)
    {
        printf("command_manager::send_slow_cmd ack received '%s' [%d]\r\n", response, ret);
        return ret;
    }
    else if (strncmp(nack, response, strlen(nack)) == 0)
        return 1;
    else if (ret > (int)response_buffer_size)
        return -1;
    else if (strncmp(ack, response, strlen(ack)) == 0)
    {
        response[ret] = '\0';
        strcpy(response_buffer, response + 4);
        return 0;
    }
    else
    {
        printf("send_slow_cmd: unknown error\r\n");
        return -1;
    }
}

int command_manager::incoming_message() const
{
    char command[32];

    int ret = communications_layer->recv(command, sizeof(command));
    if (ret < 0)
    {
        printf("command_manager::incoming_message recv error'%d'\r\n", ret);
        return ret;
    }

    ret = -1;
    command[sizeof(command) - 1] = '\0';
    printf("command_manager::incoming_message '%s'\r\n", command);
    if (strcmp(fast_cmd, command) == 0)
        ret = fast_cmd_observer->process_command();
    else if (strcmp(slow_cmd, command) == 0)
        ret = slow_cmd_observer->process_command();
    else if (strcmp(shutdown_cmd, command) == 0)
        ret = shutdown_cmd_observer->process_command();

    printf("command_manager:incoming_message processed '%s' => %d\r\n", command, ret);
    if (ret == 0)
        communications_layer->send(ack, sizeof(ack));
    else
        communications_layer->send(nack, sizeof(nack));

    return 0;
}

void command_manager::subscribe_fast_cmd(command_observer *observer) { fast_cmd_observer = observer; }

void command_manager::subscribe_slow_cmd(command_observer *observer) { slow_cmd_observer = observer; }

void command_manager::subscribe_shutdown_cmd(command_observer *observer) { shutdown_cmd_observer = observer; }
