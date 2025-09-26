#include <event-listeners/socket-event-listener.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

socket_event_listener::socket_event_listener(const char *path, event_listener_socket *socket)
    : socket_path(path), socket(socket) {
    is_listening = false;

    if (pipe(wakeuppfd) == -1)
        throw std::runtime_error("socket_event_listener: Error creating wake-up pipe");

    int flags = fcntl(wakeuppfd[0], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("socket_event_listener: Error getting wake-up[0] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[0], F_SETFL, flags) == -1)
        throw std::runtime_error("socket_event_listener: Error setting wake-up[0] pipe non blocking flag");

    flags = fcntl(wakeuppfd[1], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("socket_event_listener: Error getting wake-up[1] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[1], F_SETFL, flags) == -1)
        throw std::runtime_error("socket_event_listener: Error setting wake-up[1] pipe non blocking flag");
}

socket_event_listener::~socket_event_listener() {
    close(wakeuppfd[0]);
    close(wakeuppfd[1]);
}

int socket_event_listener::listen(event_listener_handler *handler) {
    fd_set readfds;
    int result = 0;
    is_listening = true;
    int listening_socket = socket->init();
    if (listening_socket == -1) {
        perror("socket_event_listener::listen: listening socket");
        return listening_socket;
    }

    while (is_listening) {
        FD_ZERO(&readfds);
        FD_SET(listening_socket, &readfds);
        FD_SET(wakeuppfd[0], &readfds);
        result = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if (result < 0 && errno != EINTR) {
            perror("socket_event_listener::listen: select");
            break;
        }
        if (result < 0)
            continue;

        if (FD_ISSET(listening_socket, &readfds)) {
            result = socket->connect();
            if (result == -1) {
                perror("local_tcp_socket_listener_transport_layer::accept");
                break;
            }
            printf("socket_event_listener::listen: event\r\n");
            handler->handle();
            socket->close();
        }
    }

    printf("socket_event_listener::listen: stopped\r\n");
    socket->cleanup();

    return result;
}

int socket_event_listener::stop() {
    printf("socket_event_listener::stop\r\n");
    is_listening = false;
    if (write(wakeuppfd[1], "x", 1) == -1) {
        perror("socket_event_listener::stop: wakeup write");
        return -1;
    }
    return 0;
}

int socket_event_listener::shutdown() { return stop(); }
