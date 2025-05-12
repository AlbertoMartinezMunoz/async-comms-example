#include <command-manager/command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *comms)
    : communications(comms), fast_cmd_observer(nullptr), slow_cmd_observer(nullptr) {}

int command_manager::send_fast_cmd()
{
    char command[] = "FAST COMMAND";
    char response[16];

    int ret = communications->send(command, sizeof(command));
    if (ret != sizeof(command))
        return -1;

    ret = communications->recv(response, sizeof(response));
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
    char response[16];

    int ret = communications->send(command, sizeof(command));
    if (ret != sizeof(command))
        return -1;

    ret = communications->recv(response, sizeof(response));
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
        return -1;
}

int command_manager::incoming_cmd()
{
    char command[16];

    int ret = communications->recv(command, sizeof(command));
    if (ret < 0)
        return ret;
    command[sizeof(command) - 1] = '\0';
    if (strcmp("FAST COMMAND", command) == 0)
        ret = fast_cmd_observer->process_command();
    else if (strcmp("SLOW COMMAND", command) == 0)
        ret = slow_cmd_observer->process_command();

    if (ret == 0)
        communications->send(ack, sizeof(ack));
    else
        communications->send(nack, sizeof(nack));

    return 0;
}

void command_manager::subscribe_fast_cmd(fast_command_observer *observer) { fast_cmd_observer = observer; }

void command_manager::subscribe_slow_cmd(slow_command_observer *observer) { slow_cmd_observer = observer; }
