#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_

#include <command-manager/command-handler.hpp>
#include <communications-layer/communications-layer.hpp>

class socket_transport_layer : public communications_layer {
  public:
    socket_transport_layer(const char *socket_path);

    ~socket_transport_layer();

    int connect() override;

    int disconnect() override;

    int listen_connections(const char *socket_path, command_handler *cmd_handler);

    void stop_listening();

    ssize_t send(const char *buffer, size_t buffer_size) override;

    ssize_t recv(char *buffer, size_t buffer_size) override;

    int shutdown() override;

  private:
    char *socket_path;
    int sending_socket;
    int listening_socket;
    bool is_listening;

    int wakeuppfd[2];
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
