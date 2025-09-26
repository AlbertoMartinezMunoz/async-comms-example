#include <event-listeners/socket-event-listener.hpp>

socket_event_listener::socket_event_listener() {}
int socket_event_listener::listen(event_listener_handler *handler) {
    (void)handler;
    return 0;
}

int socket_event_listener::stop() { return 0; }
