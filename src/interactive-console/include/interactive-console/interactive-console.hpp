#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <interactive-console/interactive-console-command.hpp>

class interactive_console
{
public:
    interactive_console(void);

    void listen(void);

    void set_shutdown_command(interactive_console_command *cmd);

    void set_fast_command(interactive_console_command *cmd);

    void set_slow_command(interactive_console_command *cmd);

private:
    interactive_console_command *shutdown_command;
    interactive_console_command *fast_command;
    interactive_console_command *slow_command;
};

#endif // IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
