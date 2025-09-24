#ifndef IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LISTENER_H_
#define IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LISTENER_H_

class listener_event_observer {
    void notify();
};

class listener {
  public:
    virtual int listen(listener_event_observer *connection_observer) = 0;
    virtual int stop() = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_LISTENERS_INCLUDE_LISTENERS_LISTENER_H_
