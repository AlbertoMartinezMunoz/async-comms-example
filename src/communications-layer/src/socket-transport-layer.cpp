#include <communications-layer/socket-transport-layer.hpp>

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <chrono>
#include <thread>

socket_transport_layer::socket_transport_layer() {
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

socket_transport_layer::~socket_transport_layer() {
  close(wakeuppfd[0]);
  close(wakeuppfd[1]);
}

int socket_transport_layer::connect_socket(const char *socket_path) {
  struct sockaddr_un addr;

  sending_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (sending_socket == -1) {
    perror("socket");
    return (-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

  int ret =
      connect(sending_socket, (const struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    fprintf(stderr, "The server is down.\n");
    return (-1);
  }

  return 0;
}

int socket_transport_layer::disconnect_socket() {
  close(sending_socket);

  return 0;
}

void listen_cleanup(int listening_socket, const char *socket_path) {
  close(listening_socket);
  unlink(socket_path);
}

int socket_transport_layer::listen_connections(
    const char *socket_path, communications_layer_observer *in_msg_observer) {
  struct sockaddr_un name;

  listening_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (listening_socket == -1) {
    perror("listening_socket");
    return (-1);
  }

  memset(&name, 0, sizeof(name));
  name.sun_family = AF_UNIX;
  strncpy(name.sun_path, socket_path, sizeof(name.sun_path) - 1);
  int ret =
      bind(listening_socket, (const struct sockaddr *)&name, sizeof(name));
  if (ret == -1) {
    perror("bind");
    listen_cleanup(listening_socket, socket_path);
    return (-1);
  }

  ret = listen(listening_socket, 20);
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
      perror("interactive_console::listen: select");
      break;
    }
    if (r < 0)
      continue;

    if (FD_ISSET(listening_socket, &readfds)) {
      sending_socket = accept(listening_socket, NULL, NULL);
      if (sending_socket == -1) {
        perror("accept");
        break;
      }
      printf("socket_transport_layer::listen_connections: incoming "
             "connection\r\n");
      in_msg_observer->incoming_message();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    close(sending_socket);
  }

  printf("socket_transport_layer::listen_connections: stopped\r\n");

  listen_cleanup(listening_socket, socket_path);

  return 0;
}

void socket_transport_layer::stop_listening() {
  printf("socket_transport_layer::stop_listening\r\n");
  is_listening = false;
  if (write(wakeuppfd[1], "x", 1) == -1)
    perror("socket wakeup write");
}

ssize_t socket_transport_layer::send(const char *buffer, size_t buffer_size) {
  int ret = write(sending_socket, buffer, buffer_size);
  if (ret == -1) {
    printf("socket_transport_layer::send error\r\n");
    perror("write");
    return (-1);
  }
  printf("socket_transport_layer::send: '%s' [%d / %zu]\r\n", buffer, ret,
         buffer_size);
  return communications_layer::send(buffer, ret);
}

ssize_t socket_transport_layer::recv(char *buffer, size_t buffer_size) {
  int ret = communications_layer::recv(buffer, buffer_size);
  if (ret < 0) {
    perror("recv");
    return ret;
  }

  ret = read(sending_socket, buffer, buffer_size);
  if (ret == -1) {
    perror("read");
  }
  printf("socket_transport_layer::recv '%s' [%d / %zu]\r\n", buffer, ret,
         buffer_size);

  return ret;
}
