#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <interactive-console/interactive-console-command.hpp>
#include <signal.h>

class interactive_console
{
public:
    void init(void);

    void listen(void);

    void set_shutdown_command(interactive_console_command *cmd);

    void set_fast_command(interactive_console_command *cmd);

    void set_slow_command(interactive_console_command *cmd);

private:
    static void readline_cb(char *line);

    static void signal_handler(int sig);

    sigset_t orig_mask;
};

#endif // IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
