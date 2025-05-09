#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_

#include <communications-layer/communications-layer.hpp>
#include <command-manager/command-observer.hpp>

class command_manager
{
public:
    command_manager(communications_layer_interface *comms);

    ~command_manager()
    {
    }

    int send_fast_cmd();

    int send_slow_cmd();

    int subscribe_fast_cmd(fast_command_observer *observer);

    int subscribe_slow_cmd(slow_command_observer *observer);

    // protected??? create in an interface? this method will be provided to the sending data method (AKA) transport layer.
    int incoming_command(const void *command, size_t command_size, void *response, size_t reponse_size);

private:
    communications_layer_interface *communications;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
