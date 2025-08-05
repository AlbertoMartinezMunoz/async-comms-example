#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <commands/commands.hpp>
#include <commands/shutdown-receiver.hpp>
#include <cstdlib>

class interactive_console : public shutdown_receiver {
  public:
    static interactive_console *get_instance();

    ~interactive_console();

    void listen();

    ssize_t recv(char *buffer, size_t buffer_size);

    void set_shutdown_command(command *cmd);

    void set_fast_command(command *cmd);

    void set_slow_command(command *cmd);

    interactive_console(interactive_console &other) = delete;

    void operator=(const interactive_console &) = delete;

    int shutdown() override;

  protected:
    interactive_console();

  private:
    int wakeuppfd[2];

    static void readline_cb(char *line);
};

#endif // IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
