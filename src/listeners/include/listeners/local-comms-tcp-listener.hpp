#ifndef IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LOCAL_COMMS_TCP_LISTENER_H_
#define IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LOCAL_COMMS_TCP_LISTENER_H_

#include <communications-layer/socket-transport-layer.hpp>
#include <listener.hpp>

class local_comms_tcp_listener : public listener, public socket_transport_layer {
  public:
    local_comms_tcp_listener();
    int listen(listener_event_observer *connection_observer) override;
    int stop() override;
    ssize_t send(const char *buffer, size_t buffer_size) override;
    ssize_t recv(char *buffer, size_t buffer_size) override;
}

#endif // IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LOCAL_COMMS_TCP_LISTENER_H_
