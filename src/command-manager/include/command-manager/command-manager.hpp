#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_

#include <commands/commands.hpp>
#include <communications-layer/communications-layer.hpp>
#include <communications-layer/communications-observer.hpp>
#include <communications-layer/socket-transport-layer.hpp>

class command_manager {
  public:
    command_manager(communications_layer_interface *communications_layer, socket_transport_layer *socket,
                    const char *send_cmd_socket_path);

    ~command_manager() {}

    int send_fast_cmd();

    int send_slow_cmd(char *response_buffer, size_t response_buffer_size);

    int send_shutdown_cmd();

  private:
    communications_layer_interface *communications_layer;
    socket_transport_layer *socket;
    const char *send_cmd_socket_path;

    int send_simple_cmd(const char *cmd, size_t cmd_size);
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_MANAGER_H_
