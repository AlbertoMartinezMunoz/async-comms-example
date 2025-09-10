#ifndef IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_
#define IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_

class networking_client {
  public:
    virtual int connect() = 0;
    virtual int disconnect() = 0;
    virtual ~networking_client() {}
};

class networking_server_observer {
  public:
    virtual void client_connected() = 0;
};

class networking_server {
  public:
    virtual int listen(const char *socket_path, networking_server_observer *connection_observer) = 0;
    virtual int stop() = 0;

  private:
    networking_server_observer *connection_observer;
};

#endif // IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_
