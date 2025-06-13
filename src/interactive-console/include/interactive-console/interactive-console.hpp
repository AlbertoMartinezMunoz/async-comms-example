#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <interactive-console/interactive-console-command.hpp>

class interactive_console
{
public:
    void init(void);

    void listen(void);

    void set_shutdown_command(interactive_console_command *cmd);

    void set_fast_command(interactive_console_command *cmd);

    void set_slow_command(interactive_console_command *cmd);

    interactive_console(interactive_console &other) = delete;

    void operator=(const interactive_console &) = delete;

    static interactive_console *get_instance();

protected:
    interactive_console() {}

private:
    static void readline_cb(char *line);

    static void signal_handler(int sig);
};

#endif // IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
