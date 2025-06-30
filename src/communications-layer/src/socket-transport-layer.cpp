#include <communications-layer/socket-transport-layer.hpp>

#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include <unistd.h>

#include <chrono>
#include <thread>

static bool is_listening = false;

void socket_transport_layer::signal_handler(__attribute__((unused)) int sig)
{
    is_listening = false;
}

int socket_transport_layer::connect_socket(const char *socket_path)
{
    struct sockaddr_un addr;

    sending_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sending_socket == -1)
    {
        perror("socket");
        return (-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    int ret = connect(sending_socket, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        fprintf(stderr, "The server is down.\n");
        return (-1);
    }

    return 0;
}

int socket_transport_layer::disconnect_socket()
{
    close(sending_socket);

    return 0;
}

int socket_transport_layer::listen_connections(
    const char *path,
    communications_layer_observer *in_msg_observer)
{
    struct sockaddr_un name;

    sigset_t mask;
    struct sigaction act;
    sigset_t orig_mask;

    memset(&act, 0, sizeof(act));
    act.sa_handler = signal_handler;
    sigaction(SIGUSR1, &act, 0);
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, &orig_mask);

    socket_path = strdup(path);

    /* Create local socket. */
    listening_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listening_socket == -1)
    {
        perror("listening_socket");
        return (-1);
    }

    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */
    memset(&name, 0, sizeof(name));

    /* Bind socket to socket name. */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, socket_path, sizeof(name.sun_path) - 1);

    int ret = bind(listening_socket, (const struct sockaddr *)&name,
                   sizeof(name));
    if (ret == -1)
    {
        perror("bind");
        close(listening_socket);
        unlink(socket_path);
        return (-1);
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */

    ret = listen(listening_socket, 20);
    if (ret == -1)
    {
        perror("listen");
        close(listening_socket);
        unlink(socket_path);
        return (-1);
    }

    fd_set fds;
    int r;
    is_listening = true;
    while (is_listening)
    {
        FD_ZERO(&fds);
        FD_SET(listening_socket, &fds);
        r = pselect(FD_SETSIZE, &fds, NULL, NULL, NULL, &orig_mask);
        if (r < 0 && errno != EINTR)
        {
            perror("interactive_console::listen: select");
            break;
        }
        if (r < 0)
            continue;

        if (FD_ISSET(listening_socket, &fds))
        {
            sending_socket = accept(listening_socket, NULL, NULL);
            if (sending_socket == -1)
            {
                perror("accept");
                return -1;
            }
            printf("socket_transport_layer::listen_connections: incoming connection\r\n");
            in_msg_observer->incoming_message();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        close(sending_socket);
    }

    printf("socket_transport_layer::listen_connections: stopped\r\n");

    close(listening_socket);
    unlink(socket_path);
    free(socket_path);

    return 0;
}

void socket_transport_layer::stop_listening()
{
    printf("socket_transport_layer::stop_listening\r\n");
    is_listening = false;
}

ssize_t socket_transport_layer::send(const char *buffer, size_t buffer_size)
{
    int ret = write(sending_socket, buffer, buffer_size);
    if (ret == -1)
    {
        printf("socket_transport_layer::send error\r\n");
        perror("write");
        return (-1);
    }
    printf("socket_transport_layer::send: '%s' [%d / %zu]\r\n", buffer, ret, buffer_size);
    return communications_layer::send(buffer, ret);
}

ssize_t socket_transport_layer::recv(char *buffer, size_t buffer_size)
{
    int ret = communications_layer::recv(buffer, buffer_size);
    if (ret < 0)
    {
        perror("recv");
        return ret;
    }

    ret = read(sending_socket, buffer, buffer_size);
    if (ret == -1)
    {
        perror("read");
    }
    printf("socket_transport_layer::recv '%s' [%d / %zu]\r\n", buffer, ret, buffer_size);

    return ret;
}
