#ifndef IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_SOCKET_H_
#define IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_SOCKET_H_

class event_listener_socket {
  public:
    virtual int init() = 0;
    virtual int connect() = 0;
    virtual int close() = 0;
    virtual int cleanup() = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_SOCKET_H_
