#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_

class command_observer {
public:
  virtual int process_command() = 0;
  virtual ~command_observer() {}
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_OBSERVER_H_
