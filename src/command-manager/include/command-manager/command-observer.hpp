#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_
#include <string>

class fast_command_observer
{
    virtual int process_command(int ack) = 0;
};

class slow_command_observer
{
    virtual int process_command(int ack, std::string message) = 0;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_
