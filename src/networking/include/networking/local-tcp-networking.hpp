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

#endif // IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_LOCAL_TCP_NETWORKING_H_
