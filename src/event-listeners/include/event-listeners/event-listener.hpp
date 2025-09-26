#ifndef IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_H_
#define IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_H_

class event_listener_handler {
  public:
    virtual void handle() = 0;
};

class event_listener {
  public:
    virtual int listen(event_listener_handler *handler) = 0;
    virtual int stop() = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_EVENT_LISTENERS_INCLUDE_EVENT_LISTENERS_EVENT_LISTENER_H_
