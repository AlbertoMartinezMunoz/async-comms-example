#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <interactive-console/interactive-console-command.hpp>
#include <interactive-console/interactive-console-observer.hpp>

#include <commands/shutdown-receiver.hpp>

class interactive_console : public shutdown_receiver {
  public:
    static interactive_console *get_instance();

    ~interactive_console();

    void listen(interactive_console_observer *observer);

    void set_shutdown_command(interactive_console_command *cmd);

    void set_fast_command(interactive_console_command *cmd);

    void set_slow_command(interactive_console_command *cmd);

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
