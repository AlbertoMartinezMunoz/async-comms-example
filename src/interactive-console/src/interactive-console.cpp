#include <interactive-console/interactive-console.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

interactive_console::interactive_console(void)
    : shutdown_command(nullptr), fast_command(nullptr), slow_command(nullptr) {}

void interactive_console::listen(void)
{
    static char *line_read = (char *)NULL;
    bool run = true;

    while (run)
    {
        /* Get a line from the user. */
        line_read = readline("$ ");

        /* If the line has any text in it,
           save it on the history. */
        if (line_read && *line_read)
        {
            add_history(line_read);
            switch (line_read[0])
            {
            case 'D':
                run = false;
                if (shutdown_command)
                    shutdown_command->execute();
                break;
            case 'F':
                if (fast_command)
                    fast_command->execute();
                break;
            case 'S':
                if (slow_command)
                    slow_command->execute();
                break;
            default:
                printf("Unknown command '%s'\r\n", line_read);
                break;
            }
        }

        if (line_read)
        {
            free(line_read);
            line_read = (char *)NULL;
        }
    }
}

void interactive_console::set_shutdown_command(interactive_console_command *cmd)
{
    shutdown_command = cmd;
}

void interactive_console::set_fast_command(interactive_console_command *cmd)
{
    fast_command = cmd;
}

void interactive_console::set_slow_command(interactive_console_command *cmd)
{
    slow_command = cmd;
}
