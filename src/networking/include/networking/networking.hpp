#ifndef IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_
#define IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_

class networking_client {
  public:
    virtual int connect() = 0;
    virtual int disconnect() = 0;
    virtual ~networking_client() {}
};

class networking_server {};

#endif // IOT_MICRO_FIRMWARE_SRC_NETWORKING_INCLUDE_NETWORKING_NETWORKING_H_
