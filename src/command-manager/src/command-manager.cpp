#include <command-manager/command-manager.hpp>
#include <cstring>

command_manager::command_manager(communications_layer_interface *comms) : communications(comms) {}

int command_manager::send_fast_cmd()
{
    char command[] = "FAST COMMAND";
    char response[16];

    int ret = communications->send_command(command, sizeof(command), response, sizeof(response));
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

    int ret = communications->send_command(command, sizeof(command), response, sizeof(response));
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

int command_manager::subscribe_fast_cmd(__attribute__((unused)) fast_command_observer *observer) { return 0; }

int command_manager::subscribe_slow_cmd(__attribute__((unused)) slow_command_observer *observer) { return 0; }
