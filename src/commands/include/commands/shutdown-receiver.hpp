#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_SHUTDOWN_RECEIVER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_SHUTDOWN_RECEIVER_H_
class shutdown_receiver {
  public:
    virtual ~shutdown_receiver() {}
    virtual int shutdown() = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_SHUTDOWN_RECEIVER_H_
