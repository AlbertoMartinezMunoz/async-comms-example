#ifndef IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_
#define IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_

#include <event-listeners/event-listener.hpp>

class socket_event_listener : public event_listener {
  public:
    socket_event_listener();
    int listen(event_listener_handler *handler) override;
    int stop() override;
};

#endif // IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_
