#include <command-manager/command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *application_layer, communications_layer_interface *transport_layer)
    : application_layer(application_layer), transport_layer(transport_layer), fast_cmd_observer(nullptr), slow_cmd_observer(nullptr) {}

int command_manager::send_fast_cmd()
{
    char command[] = "FAST COMMAND";
    char response[16];

    int ret = application_layer->send(command, sizeof(command));
    if (ret != sizeof(command))
        return -1;

    ret = transport_layer->recv(response, sizeof(response));
    if (ret < 0)
        return ret;
    else if (strncmp(nack, response, strlen(nack)) == 0)
        return 1;
    else if (strncmp(ack, response, strlen(ack)) == 0)
        return 0;
    else
        return -1;
}

int command_manager::send_slow_cmd(char *response_buffer, size_t response_buffer_size)
{
    char command[] = "SLOW COMMAND";
    char response[32];

    int ret = application_layer->send(command, sizeof(command));
    if (ret < 0)
        return -1;

    ret = transport_layer->recv(response, sizeof(response));
    if (ret < 0)
        return ret;
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

int command_manager::send_shutdown_cmd()
{
    char response[16];

    int ret = application_layer->send(shutdown_cmd, sizeof(shutdown_cmd));
    if (ret != sizeof(shutdown_cmd))
        return -1;

    ret = transport_layer->recv(response, sizeof(response));
    if (ret < 0)
        return ret;
    else if (strncmp(nack, response, strlen(nack)) == 0)
        return 1;
    else if (strncmp(ack, response, strlen(ack)) == 0)
        return 0;
    else
        return -1;
}

int command_manager::incoming_message() const
{
    char command[32];

    int ret = transport_layer->recv(command, sizeof(command));
    if (ret < 0)
        return ret;

    ret = -1;
    command[sizeof(command) - 1] = '\0';
    if (strcmp("FAST COMMAND", command) == 0)
        ret = fast_cmd_observer->process_command();
    else if (strcmp("SLOW COMMAND", command) == 0)
        ret = slow_cmd_observer->process_command();
    else if (strcmp(shutdown_cmd, command) == 0)
        ret = shutdown_cmd_observer->process_command();

    if (ret == 0)
        application_layer->send(ack, sizeof(ack));
    else
        application_layer->send(nack, sizeof(nack));

    return 0;
}

void command_manager::subscribe_fast_cmd(command_observer *observer) { fast_cmd_observer = observer; }

void command_manager::subscribe_slow_cmd(command_observer *observer) { slow_cmd_observer = observer; }

void command_manager::subscribe_shutdown_cmd(command_observer *observer) { shutdown_cmd_observer = observer; }
