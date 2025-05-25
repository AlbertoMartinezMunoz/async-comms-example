#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_

#include <communications-layer/communications-layer.hpp>
#include <communications-layer/communications-observer.hpp>
#include <command-manager/command-observer.hpp>

class command_manager: public communications_layer_observer
{
public:
    command_manager(communications_layer_interface *application_layer, communications_layer_interface *transport_layer);

    ~command_manager()
    {
    }

    int send_fast_cmd();

    int send_slow_cmd(char *response_buffer, size_t response_buffer_size);

    int send_shutdown_cmd();

    int incoming_message() const override;

    void subscribe_fast_cmd(command_observer *observer);

    void subscribe_slow_cmd(command_observer *observer);

    void subscribe_shutdown_cmd(command_observer *observer);

    static constexpr char ack[4] = "ACK";
    static constexpr char nack[5] = "NACK";
    static constexpr char fast_cmd[] = "FAST COMMAND";
    static constexpr char slow_cmd[] = "SLOW COMMAND";
    static constexpr char shutdown_cmd[] = "SHUTDOWN COMMAND";

private:
    communications_layer_interface *application_layer;
    communications_layer_interface *transport_layer;
    command_observer *fast_cmd_observer;
    command_observer *slow_cmd_observer;
    command_observer *shutdown_cmd_observer;

    int send_simple_cmd(const char *cmd, size_t cmd_size);
};

#endif  // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
