#include <listeners/local-comms-tcp-listener.hpp>

local_comms_tcp_listener::local_comms_tcp_listener() {}

int local_comms_tcp_listener::listen(listener_event_observer *connection_observer) { return 0; }

int local_comms_tcp_listener::stop() { return 0; }

ssize_t local_comms_tcp_listener::send(const char *buffer, size_t buffer_size) { return 0; }

ssize_t local_comms_tcp_listener::recv(char *buffer, size_t buffer_size) { return 0; }
