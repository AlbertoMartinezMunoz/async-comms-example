#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_LISTENER_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_LISTENER_TRANSPORT_LAYER_H_

#include <communications-layer/communications-observer.hpp>
#include <communications-layer/socket-transport-layer.hpp>

class local_tcp_socket_listener_transport_layer : public socket_transport_layer {
  public:
    local_tcp_socket_listener_transport_layer(const char *path);
    ~local_tcp_socket_listener_transport_layer();
    int listen(command_handler *cmd_handler);
    int stop();
    int shutdown() override;

  private:
    const char *socket_path;
    bool is_listening;
    int wakeuppfd[2];
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_LISTENER_TRANSPORT_LAYER_H_
