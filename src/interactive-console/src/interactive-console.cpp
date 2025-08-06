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

static command_handler *local_cmd_handler = nullptr;

static char line_buffer[32] = "";

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
    pinstance_ = nullptr;
}

interactive_console *interactive_console::get_instance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new interactive_console();
    }
    return pinstance_;
}

ssize_t interactive_console::recv(char *buffer, size_t buffer_size) {
    (void)buffer;
    (void)buffer_size;

    strcpy(buffer, line_buffer);
    return strlen(line_buffer) + 1;
}

void interactive_console::readline_cb(char *line) {
    if (line && *line) {
        add_history(line);

        strcpy(line_buffer, line);

        if (local_cmd_handler)
            local_cmd_handler->handle();
    }

    if (line)
        free(line);
}

int interactive_console::shutdown() {
    printf("interactive_console::shutdown\r\n");
    running = false;
    if (write(wakeuppfd[1], "x", 1) == -1) {
        perror("interctive console: wakeup write");
        return -1;
    }
    return 0;
}

void interactive_console::listen(command_handler *cmd_handler) {
    fd_set fds;
    int r;

    local_cmd_handler = cmd_handler;

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
