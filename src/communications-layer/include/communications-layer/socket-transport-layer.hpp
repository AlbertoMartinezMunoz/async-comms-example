#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_

#include <command-manager/command-handler.hpp>
#include <communications-layer/communications-layer.hpp>

class socket_transport_layer : public communications_layer {
  public:
    ssize_t send(const char *buffer, size_t buffer_size) override;

    ssize_t recv(char *buffer, size_t buffer_size) override;

  protected:
    int socket;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_TRANSPORT_LAYER_H_
