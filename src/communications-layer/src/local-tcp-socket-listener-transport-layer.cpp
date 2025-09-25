#include <communications-layer/local-tcp-socket-listener-transport-layer.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

local_tcp_socket_listener_transport_layer::local_tcp_socket_listener_transport_layer(const char *path)
    : socket_transport_layer(), socket_path(path) {
    is_listening = false;

    if (pipe(wakeuppfd) == -1)
        throw std::runtime_error("Error creating wake-up pipe");

    int flags = fcntl(wakeuppfd[0], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("Error getting wake-up[0] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[0], F_SETFL, flags) == -1)
        throw std::runtime_error("Error setting wake-up[0] pipe non blocking flag");

    flags = fcntl(wakeuppfd[1], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("Error getting wake-up[1] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[1], F_SETFL, flags) == -1)
        throw std::runtime_error("Error setting wake-up[1] pipe non blocking flag");
}

int local_tcp_socket_listener_transport_layer::listen(command_handler *cmd_handler) {
    struct ::sockaddr_un name;

    int listening_socket = ::socket(AF_UNIX, ::SOCK_SEQPACKET, 0);
    if (listening_socket == -1) {
        perror("listening_socket");
        return (-1);
    }

    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, socket_path, sizeof(name.sun_path) - 1);
    int ret = ::bind(listening_socket, (const struct ::sockaddr *)&name, sizeof(name));
    if (ret == -1) {
        perror("local_tcp_socket_listener_transport_layer::bind");
        close(listening_socket);
        unlink(socket_path);
        return (-1);
    }

    ret = ::listen(listening_socket, 20);
    if (ret == -1) {
        perror("local_tcp_socket_listener_transport_layer::listen");
        close(listening_socket);
        unlink(socket_path);
        return (-1);
    }

    fd_set readfds;
    int r;
    is_listening = true;
    while (is_listening) {
        FD_ZERO(&readfds);
        FD_SET(listening_socket, &readfds);
        FD_SET(wakeuppfd[0], &readfds);
        r = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        if (r < 0 && errno != EINTR) {
            perror("local_tcp_socket_listener_transport_layer::listen: select");
            break;
        }
        if (r < 0)
            continue;

        if (FD_ISSET(listening_socket, &readfds)) {
            socket = ::accept(listening_socket, NULL, NULL);
            if (socket == -1) {
                perror("local_tcp_socket_listener_transport_layer::accept");
                break;
            }
            printf("local_tcp_socket_listener_transport_layer::listen_connections: incoming "
                   "connection\r\n");
            cmd_handler->handle();
            close(socket);
        }
    }

    printf("local_tcp_socket_listener_transport_layer::listen_connections: stopped\r\n");

    close(listening_socket);
    unlink(socket_path);

    return 0;
}

int local_tcp_socket_listener_transport_layer::stop() {
    printf("local_tcp_socket_listener_transport_layer::stop\r\n");
    is_listening = false;
    if (write(wakeuppfd[1], "x", 1) == -1) {
        perror("socket wakeup write");
        return -1;
    }
    return 0;
}

int local_tcp_socket_listener_transport_layer::shutdown() { return stop(); }
