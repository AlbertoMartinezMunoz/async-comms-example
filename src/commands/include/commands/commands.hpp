#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_

class command {
public:
  virtual int execute() const = 0;
};

class local_fast_command : public command {
  int execute() const override;
};

class local_slow_command : public command {
  int execute() const override;
};

class local_shutdown_command : public command {
  int execute() const override;
};

class remote_fast_command : public command {
  int execute() const override;
};

class remote_slow_command : public command {
  int execute() const override;
};

class remote_shutdown_command : public command {
  int execute() const override;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_
