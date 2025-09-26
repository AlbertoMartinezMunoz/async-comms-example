#ifndef IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_
#define IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_

#include <commands/shutdown-receiver.hpp>
#include <event-listeners/event-listener-socket.hpp>
#include <event-listeners/event-listener.hpp>

class socket_event_listener : public event_listener, public shutdown_receiver {
  public:
    socket_event_listener(const char *path, event_listener_socket *socket);
    ~socket_event_listener();
    int listen(event_listener_handler *handler) override;
    int stop() override;
    int shutdown() override;

  private:
    const char *socket_path;
    event_listener_socket *socket;
    bool is_listening;
    int wakeuppfd[2];
};

#endif // IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_SOCKET_EVENT_LISTENER_H_
