#include <interactive-console/interactive-console.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <cerrno>
#include <readline/readline.h>
#include <readline/history.h>

#include <signal.h>

#include <mutex>

static std::mutex mutex_;
static interactive_console *pinstance_;

static bool running;

static interactive_console_command *shutdown_command = nullptr;
static interactive_console_command *fast_command = nullptr;
static interactive_console_command *slow_command = nullptr;

static sigset_t orig_mask;

interactive_console::interactive_console()
{
    sigset_t mask;
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_handler = signal_handler;
    sigaction(SIGUSR2, &act, 0);
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &mask, &orig_mask);

    rl_callback_handler_install("$", readline_cb);
}

interactive_console::~interactive_console()
{
    rl_callback_handler_remove();
}

void interactive_console::signal_handler(__attribute__((unused)) int sig)
{
    running = false;
}

interactive_console *interactive_console::get_instance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        pinstance_ = new interactive_console();
    }
    return pinstance_;
}

void interactive_console::readline_cb(char *line)
{
    if (line && *line)
    {
        add_history(line);
        switch (line[0])
        {
        case 'D':
            running = false;
            rl_callback_handler_remove();
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
            printf("Unknown command '%s'\r\n", line);
            break;
        }
    }

    if (line)
        free(line);
}

void interactive_console::listen(void)
{
    fd_set fds;
    int r;

    running = true;
    while (running)
    {
        FD_ZERO(&fds);
        FD_SET(fileno(rl_instream), &fds);
        r = pselect(FD_SETSIZE, &fds, NULL, NULL, NULL, &orig_mask);
        if (r < 0 && errno != EINTR)
        {
            perror("interactive_console::listen: select");
            rl_callback_handler_remove();
            break;
        }
        if (r < 0)
            continue;

        if (FD_ISSET(fileno(rl_instream), &fds))
            rl_callback_read_char();
    }

    rl_callback_handler_remove();

    printf("interactive_console::listen: exit loop.\r\n");
    return;
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
