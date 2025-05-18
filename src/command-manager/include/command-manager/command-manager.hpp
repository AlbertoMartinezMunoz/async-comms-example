#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_

#include <communications-layer/communications-layer.hpp>
#include <command-manager/command-observer.hpp>

class command_manager
{
public:
    command_manager(communications_layer_interface *application_layer, communications_layer_interface *transport_layer);

    ~command_manager()
    {
    }

    int send_fast_cmd();

    int send_slow_cmd(char *response_buffer, size_t response_buffer_size);

    int incoming_cmd();

    void subscribe_fast_cmd(fast_command_observer *observer);

    void subscribe_slow_cmd(slow_command_observer *observer);

    static constexpr char ack[4] = "ACK";
    static constexpr char nack[5] = "NACK";

private:
    communications_layer_interface *application_layer;
    communications_layer_interface *transport_layer;
    fast_command_observer *fast_cmd_observer;
    slow_command_observer *slow_cmd_observer;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
