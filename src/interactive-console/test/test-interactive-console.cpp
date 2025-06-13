#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <interactive-console/interactive-console.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/select.h>

#include "fff.h"

using ::testing::AnyNumber;
using ::testing::AtLeast;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, fileno, FILE *);
FAKE_VALUE_FUNC(char *, readline, const char *);
FAKE_VOID_FUNC(add_history, const char *);
FAKE_VOID_FUNC(rl_callback_handler_install, const char *, rl_vcpfunc_t *);
FAKE_VOID_FUNC(rl_callback_read_char);

#undef FD_CLR
FAKE_VOID_FUNC(FD_CLR, int, fd_set *);
#undef FD_ISSET
FAKE_VALUE_FUNC(int, FD_ISSET, int, fd_set *);
#undef FD_SET
FAKE_VOID_FUNC(FD_SET, int, fd_set *);
#undef FD_ZERO
FAKE_VOID_FUNC(FD_ZERO, fd_set *);
FAKE_VALUE_FUNC(int, pselect, int, fd_set *, fd_set *, fd_set *, const struct timespec *, const sigset_t *);

int pselect_mock(__attribute__((unused)) int nfds, __attribute__((unused)) fd_set *readfds,
                 __attribute__((unused)) fd_set *writefds, __attribute__((unused)) fd_set *exceptfds,
                 __attribute__((unused)) const struct timespec *timeout, __attribute__((unused)) const sigset_t *sigmask)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}

class InteractiveConsoleCommandMock : public interactive_console_command
{
public:
    MOCK_METHOD(void, execute, (), (const, override));
};

class InteractiveConsoleWrapper : public interactive_console
{
public:
    InteractiveConsoleWrapper()
        : interactive_console() {}
    typedef void (InteractiveConsoleWrapper::*SetCommand)(interactive_console_command *);
};

class TestInteractiveConsoleParams
{
public:
    TestInteractiveConsoleParams(std::string name, const char *console_cmd, InteractiveConsoleWrapper::SetCommand set_command_func)
        : name(name), console_cmd(console_cmd), set_command_func(set_command_func) {}

    static void *operator new(std::size_t count)
    {
        void *p = ::operator new(count);
        memset(p, 0, count);
        return p;
    }

    static void *operator new[](std::size_t count)
    {
        void *p = ::operator new[](count);
        memset(p, 0, count);
        return p;
    }

    std::string name;
    const char *console_cmd;
    InteractiveConsoleWrapper::SetCommand set_command_func;
};

class TestInteractiveConsole : public testing::TestWithParam<TestInteractiveConsoleParams>
{
public:
    virtual void SetUp()
    {
        RESET_FAKE(readline);
        RESET_FAKE(rl_callback_handler_install);
        RESET_FAKE(pselect);
        cmd_mock = new InteractiveConsoleCommandMock();
        console = new InteractiveConsoleWrapper();
        console->set_shutdown_command(nullptr);
        readline_cb = rl_callback_handler_install_fake.arg1_val;
        pselect_fake.custom_fake = pselect_mock;
    }

    virtual void TearDown()
    {
        delete console;
        delete cmd_mock;
    }

    rl_vcpfunc_t *readline_cb;
    InteractiveConsoleWrapper *console;
    InteractiveConsoleCommandMock *cmd_mock;
};

TEST_P(TestInteractiveConsole, WhenReceivesCommandIfOKThenExecutesTheCommand)
{
    InteractiveConsoleWrapper::SetCommand set_command_func = GetParam().set_command_func;
    (console->*set_command_func)(cmd_mock);

    EXPECT_CALL(*cmd_mock, execute()).Times(1);

    char *console_cmd = strdup(GetParam().console_cmd);
    readline_cb(console_cmd);
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestInteractiveConsole,
    ::testing::Values(
        TestInteractiveConsoleParams{"WhenReceivesShutdownCommandIfOKThenExecutesTheShutdownCommand", "D", &interactive_console::set_shutdown_command},
        TestInteractiveConsoleParams{"WhenReceivesFastCommandIfOKThenExecutesTheFastCommand", "F", &interactive_console::set_fast_command},
        TestInteractiveConsoleParams{"WhenReceivesSlowCommandIfOKThenExecutesTheSlowCommand", "S", &interactive_console::set_slow_command}),
    [](const testing::TestParamInfo<TestInteractiveConsoleParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestInteractiveConsole, WhenReceivesShudtownCommandIfOKThenStopListeningAndShutdown)
{
    char *console_cmd = strdup("D");
    std::thread t1(&interactive_console::listen, console);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    readline_cb(console_cmd);
    t1.join();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
