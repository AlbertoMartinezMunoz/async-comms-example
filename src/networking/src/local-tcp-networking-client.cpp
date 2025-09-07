#include <networking/local-tcp-networking.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

networking_local_client::networking_local_client(const char *path) { socket_path = strdup(path); }

networking_local_client::~networking_local_client() { free(socket_path); }

int networking_local_client::connect() {
    struct ::sockaddr_un addr;

    socket = ::socket(AF_UNIX, ::SOCK_SEQPACKET, 0);
    if (socket == -1) {
        perror("socket");
        return (-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    int ret = ::connect(socket, (const struct ::sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        return (-1);
    }

    return 0;
}

int networking_local_client::disconnect() {
    ::close(socket);
    return 0;
}
