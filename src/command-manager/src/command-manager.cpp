#include <command-manager/command-manager.hpp>

command_manager::command_manager(communications_layer_interface *comms) : communications(comms) {}

int command_manager::send_fast_cmd()
{
    char command[] = "FAST COMMAND";
    char response[16];
    return communications->send_command(command, sizeof(command), response, sizeof(response));
}

int command_manager::send_slow_cmd()
{
    char command[] = "SLOW COMMAND";
    char response[16];
    return communications->send_command(command, sizeof(command), response, sizeof(response));
}

int command_manager::subscribe_fast_cmd(__attribute__((unused)) fast_command_observer *observer) { return 0; }

int command_manager::subscribe_slow_cmd(__attribute__((unused)) slow_command_observer *observer) { return 0; }