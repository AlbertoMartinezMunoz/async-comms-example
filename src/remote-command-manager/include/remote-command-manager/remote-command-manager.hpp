#ifndef IOT_MICRO_FIRMWARE_SRC_REMOTE_COMMAND_MANAGER_INCLUDE_REMOTE_COMMAND_MANAGER_REMOTE_COMMAND_MANAGER_H_
#define IOT_MICRO_FIRMWARE_SRC_REMOTE_COMMAND_MANAGER_INCLUDE_REMOTE_COMMAND_MANAGER_REMOTE_COMMAND_MANAGER_H_

#include <communications-layer/communications-layer.hpp>
#include <communications-layer/communications-observer.hpp>
#include <remote-command-manager/remote-command-observer.hpp>

class remote_command_manager : public communications_layer_observer
{
public:
    remote_command_manager(communications_layer_interface *communications_layer);

    ~remote_command_manager()
    {
    }

    int send_fast_cmd();

    int send_slow_cmd(char *response_buffer, size_t response_buffer_size);

    int send_shutdown_cmd();

    int incoming_message() const override;

    void subscribe_fast_cmd(remote_command_observer *observer);

    void subscribe_slow_cmd(remote_command_observer *observer);

    void subscribe_shutdown_cmd(remote_command_observer *observer);

    static constexpr char ack[4] = "ACK";
    static constexpr char nack[5] = "NACK";
    static constexpr char fast_cmd[] = "FAST COMMAND";
    static constexpr char slow_cmd[] = "SLOW COMMAND";
    static constexpr char shutdown_cmd[] = "SHUTDOWN COMMAND";

private:
    communications_layer_interface *communications_layer;

    remote_command_observer *fast_cmd_observer;
    remote_command_observer *slow_cmd_observer;
    remote_command_observer *shutdown_cmd_observer;

    int send_simple_cmd(const char *cmd, size_t cmd_size);
};

#endif // IOT_MICRO_FIRMWARE_SRC_REMOTE_COMMAND_MANAGER_INCLUDE_REMOTE_COMMAND_MANAGER_REMOTE_COMMAND_MANAGER_H_
