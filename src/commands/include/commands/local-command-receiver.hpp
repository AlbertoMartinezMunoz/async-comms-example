#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_LOCAL_COMMAND_RECEIVER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_LOCAL_COMMAND_RECEIVER_H_

#include <cstddef>

class local_command_receiver {
  public:
    virtual ~local_command_receiver() {}
    virtual int send_fast_cmd() = 0;
    virtual int send_shutdown_cmd() = 0;
    virtual int send_slow_cmd(char *response_buffer, size_t response_buffer_size) = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_LOCAL_COMMAND_RECEIVER_H_
