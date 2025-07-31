#ifndef IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_IDS_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_IDS_H_

class command_ids {
  public:
    static constexpr char ack[4] = "ACK";
    static constexpr char nack[5] = "NACK";
    static constexpr char fast_cmd_id[] = "FAST COMMAND";
    static constexpr char slow_cmd_id[] = "SLOW COMMAND";
    static constexpr char shutdown_cmd_id[] = "SHUTDOWN COMMAND";
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMAND_MANAGER_INCLUDE_COMMAND_MANAGER_COMMAND_IDS_H_
