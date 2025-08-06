#ifndef IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_

class command {
  public:
    virtual ~command() {}
    virtual int execute() const = 0;

    static constexpr char ack[4] = "ACK";
    static constexpr char nack[5] = "NACK";

    static constexpr char remote_fast_cmd_id[] = "FAST COMMAND";
    static constexpr char remote_slow_cmd_id[] = "SLOW COMMAND";
    static constexpr char remote_shutdown_cmd_id[] = "SHUTDOWN COMMAND";

    static constexpr char local_fast_cmd_id[] = "F";
    static constexpr char local_slow_cmd_id[] = "S";
    static constexpr char local_shutdown_cmd_id[] = "D";
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMANDS_INCLUDE_COMMANDS_COMMANDS_H_
