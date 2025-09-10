#ifndef IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_LOCAL_TCP_NETWORKING_H_
#define IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_LOCAL_TCP_NETWORKING_H_

#include <networking/networking.hpp>

class networking_local_client : public networking_client {
  public:
    networking_local_client(const char *path);
    ~networking_local_client();

    int connect() override;
    int disconnect() override;

  private:
    char *socket_path;
    int socket;
};

class networking_local_server : public networking_server {
  public:
    networking_local_server();
    ~networking_local_server();

    int listen(const char *socket_path, networking_server_observer *connection_observer) override;
    int stop() override;

  private:
    bool is_listening;
    int socket;
    int wakeuppfd[2];
};

#endif // IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_LOCAL_TCP_NETWORKING_H_
