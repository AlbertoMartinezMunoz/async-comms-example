#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <interactive-console/interactive-console.hpp>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/select.h>
#include <unistd.h>

#include "fff.h"

using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Return;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, fileno, FILE *);
FAKE_VALUE_FUNC(char *, readline, const char *);
FAKE_VOID_FUNC(add_history, const char *);
FAKE_VOID_FUNC(rl_callback_handler_install, const char *, rl_vcpfunc_t *);
FAKE_VOID_FUNC(rl_callback_read_char);
FAKE_VOID_FUNC(rl_callback_handler_remove);

#undef FD_CLR
FAKE_VOID_FUNC(FD_CLR, int, fd_set *);
#undef FD_ISSET
FAKE_VALUE_FUNC(int, FD_ISSET, int, fd_set *);
#undef FD_SET
FAKE_VOID_FUNC(FD_SET, int, fd_set *);
#undef FD_ZERO
FAKE_VOID_FUNC(FD_ZERO, fd_set *);
FAKE_VALUE_FUNC(int, select, int, fd_set *, fd_set *, fd_set *, struct timeval *);
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);

int select_mock(__attribute__((unused)) int nfds, __attribute__((unused)) fd_set *readfds,
                __attribute__((unused)) fd_set *writefds, __attribute__((unused)) fd_set *exceptfds,
                __attribute__((unused)) struct timeval *) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}

class CommandHandlerMock : public command_handler {
  public:
    virtual ~CommandHandlerMock() {}
    MOCK_METHOD(int, handle, ());
};

class TestInteractiveConsole : public testing::Test {
  public:
    virtual void SetUp() {
        RESET_FAKE(readline);
        RESET_FAKE(rl_callback_handler_install);
        RESET_FAKE(select);
        console = interactive_console::get_instance();
        readline_cb = rl_callback_handler_install_fake.arg1_val;
        select_fake.custom_fake = select_mock;
        cmd_handler_mock = new CommandHandlerMock();
    }

    virtual void TearDown() {
        delete console;
        delete cmd_handler_mock;
    }

    rl_vcpfunc_t *readline_cb;
    interactive_console *console;
    CommandHandlerMock *cmd_handler_mock = nullptr;
};

TEST_F(TestInteractiveConsole, GivenALineHasBeenReceivedWhenRetirevingThatLineThenReturnTheLine) {
    char line[32];
    char expected_line[] = "Expected Line\r\n";

    readline_cb(strdup(expected_line));

    ASSERT_EQ(sizeof(expected_line), console->recv(line, sizeof(line)));
    EXPECT_STREQ(expected_line, line);
}

TEST_F(TestInteractiveConsole, WhenListeningIfStopThenStopListeningAndShutdown) {
    std::thread t1(&interactive_console::listen, console, cmd_handler_mock);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    console->shutdown();
    t1.join();
}

TEST_F(TestInteractiveConsole, GivenConsoleIsListningWhenALineIsReceivedThenCommandHandlerIsCalled) {
    char expected_line[] = "Expected Line\r\n";
    EXPECT_CALL(*cmd_handler_mock, handle()).Times(1).WillOnce(Return(0));
    std::thread t1(&interactive_console::listen, console, cmd_handler_mock);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    readline_cb(strdup(expected_line));
    console->shutdown();
    t1.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
