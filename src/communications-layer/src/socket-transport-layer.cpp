#include <communications-layer/socket-transport-layer.hpp>

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace socket {
#include <sys/socket.h>
#include <sys/un.h>
} // namespace socket

#include <chrono>
#include <thread>

ssize_t socket_transport_layer::send(const char *buffer, size_t buffer_size) {
    int ret = write(socket, buffer, buffer_size);
    if (ret == -1) {
        printf("socket_transport_layer::send error\r\n");
        perror("write");
        return (-1);
    }
    printf("socket_transport_layer::send: '%s' [%d / %zu]\r\n", buffer, ret, buffer_size);
    return communications_layer::send(buffer, ret);
}

ssize_t socket_transport_layer::recv(char *buffer, size_t buffer_size) {
    int ret = communications_layer::recv(buffer, buffer_size);
    if (ret < 0) {
        perror("recv");
        return ret;
    }

    ret = read(socket, buffer, buffer_size);
    if (ret == -1) {
        perror("read");
    }
    printf("socket_transport_layer::recv '%s' [%d / %zu]\r\n", buffer, ret, buffer_size);

    return ret;
}
