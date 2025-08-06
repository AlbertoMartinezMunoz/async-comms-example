#ifndef IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_
#define IOT_MICRO_FIRMWARE_SRC_INTERACTIVE_CONSOLE_INCLUDE_INTERACTIVE_CONSOLE_INTERACTIVE_CONSOLE_H_

#include <command-manager/command-handler.hpp>
#include <commands/commands.hpp>
#include <communications-layer/communications-layer.hpp>
#include <cstdlib>

class interactive_console : public communications_layer {
  public:
    static interactive_console *get_instance();

    ~interactive_console();

    void listen(command_handler *cmd_handler);

    ssize_t recv(char *buffer, size_t buffer_size) override;

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
