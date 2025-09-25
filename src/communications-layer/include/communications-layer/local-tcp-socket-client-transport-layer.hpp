#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_CLIENT_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_CLIENT_TRANSPORT_LAYER_H_

#include <communications-layer/socket-transport-layer.hpp>

class local_tcp_socket_client_transport_layer : public socket_transport_layer {
  public:
    local_tcp_socket_client_transport_layer(const char *path) : socket_transport_layer(""), socket_path(path) {}
    int connect();
    int disconnect();

  private:
    const char *socket_path;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_LOCAL_TCP_SOCKET_CLIENT_TRANSPORT_LAYER_H_
