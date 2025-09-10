#include <networking/local-tcp-networking.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

networking_local_server::networking_local_server() {
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

networking_local_server::~networking_local_server() {}

void listen_cleanup(int listening_socket, const char *socket_path) {
    close(listening_socket);
    unlink(socket_path);
}

int networking_local_server::listen(const char *socket_path, networking_server_observer *connection_observer) {
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
        perror("bind");
        listen_cleanup(listening_socket, socket_path);
        return (-1);
    }

    ret = ::listen(listening_socket, 20);
    if (ret == -1) {
        perror("listen");
        listen_cleanup(listening_socket, socket_path);
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
            perror("networking_local_server::listen: select");
            break;
        }
        if (r < 0)
            continue;

        if (FD_ISSET(listening_socket, &readfds)) {
            socket = ::accept(listening_socket, NULL, NULL);
            if (socket == -1) {
                perror("accept");
                break;
            }
            printf("networking_local_server::listen_connections: incoming "
                   "connection\r\n");
            connection_observer->client_connected();
            close(socket);
        }
    }

    printf("networking_local_server::listen_connections: stopped\r\n");

    listen_cleanup(listening_socket, socket_path);

    return 0;
}

int networking_local_server::stop() {
    printf("networking_local_server::shutdown\r\n");
    is_listening = false;
    if (write(wakeuppfd[1], "x", 1) == -1) {
        perror("socket wakeup write");
        return -1;
    }
    return 0;
}
