#include <interactive-console/interactive-console.hpp>

#include <cerrno>
#include <fcntl.h>
#include <mutex>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static std::mutex mutex_;
static interactive_console *pinstance_;

static bool running;

static interactive_console_command *shutdown_command = nullptr;
static interactive_console_command *fast_command = nullptr;
static interactive_console_command *slow_command = nullptr;

static interactive_console_observer *cli_observer = nullptr;

interactive_console::interactive_console() {
    if (pipe(wakeuppfd) == -1)
        throw std::runtime_error("Error creating wake-up pipe");

    int flags = fcntl(wakeuppfd[0], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("Error getting wake-up[0] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[0], F_SETFL, flags) == -1)
        throw std::runtime_error("Error setting wake-up[0] pipe non blocking flag");

    flags = fcntl(wakeuppfd[1], F_GETFL);
    if (flags == -1)
        throw std::runtime_error("Error getting wake-up[1] pipe flags");

    flags |= O_NONBLOCK;
    if (fcntl(wakeuppfd[1], F_SETFL, flags) == -1)
        throw std::runtime_error("Error setting wake-up[1] pipe non blocking flag");

    rl_callback_handler_install("$", readline_cb);
}

interactive_console::~interactive_console() {
    rl_callback_handler_remove();
    close(wakeuppfd[0]);
    close(wakeuppfd[1]);
}

interactive_console *interactive_console::get_instance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new interactive_console();
    }
    return pinstance_;
}

void interactive_console::readline_cb(char *line) {
    if (line && *line) {
        add_history(line);
        if (cli_observer)
            cli_observer->console_incoming_message(line);
        switch (line[0]) {
        case 'D':
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

void interactive_console::stop(void) {
    printf("interactive_console::stop\r\n");
    running = false;
    if (write(wakeuppfd[1], "x", 1) == -1)
        perror("interctive console: wakeup write");
}

void interactive_console::listen(interactive_console_observer *observer) {
    fd_set fds;
    int r;

    cli_observer = observer;

    running = true;
    while (running) {
        FD_ZERO(&fds);
        FD_SET(fileno(rl_instream), &fds);
        FD_SET(wakeuppfd[0], &fds);
        r = select(FD_SETSIZE, &fds, NULL, NULL, NULL);
        if (r < 0 && errno != EINTR) {
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

void interactive_console::set_shutdown_command(interactive_console_command *cmd) { shutdown_command = cmd; }

void interactive_console::set_fast_command(interactive_console_command *cmd) { fast_command = cmd; }

void interactive_console::set_slow_command(interactive_console_command *cmd) { slow_command = cmd; }
