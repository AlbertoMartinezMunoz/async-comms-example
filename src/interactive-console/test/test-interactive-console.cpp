#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <interactive-console/interactive-console.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "fff.h"

using ::testing::AtLeast;
using ::testing::AnyNumber;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(char *, readline, const char *);
FAKE_VOID_FUNC(add_history, const char *);

char *readline_shutdown_mock(__attribute__((unused)) const char *prompt)
{
    return strdup("D");
}

char *readline_fast_mock(__attribute__((unused)) const char *prompt)
{
    return strdup("F");
}

char *readline_slow_mock(__attribute__((unused)) const char *prompt)
{
    return strdup("S");
}

typedef char *(*readline_ptr)(const char *);

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
    TestInteractiveConsoleParams(std::string name, readline_ptr readline_handler, InteractiveConsoleWrapper::SetCommand set_command_func)
        : name(name), readline_handler(readline_handler), set_command_func(set_command_func) {}

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
    readline_ptr readline_handler;
    InteractiveConsoleWrapper::SetCommand set_command_func;
};

class TestInteractiveConsole : public testing::TestWithParam<TestInteractiveConsoleParams>
{
public:
    virtual void SetUp()
    {
        RESET_FAKE(readline);
        cmd_mock = new InteractiveConsoleCommandMock();
        shutdown_cmd_mock = new InteractiveConsoleCommandMock();
        console = new InteractiveConsoleWrapper();
    }

    virtual void TearDown()
    {
        delete console;
        delete cmd_mock;
        delete shutdown_cmd_mock;
    }

    InteractiveConsoleWrapper *console;
    InteractiveConsoleCommandMock *cmd_mock;
    InteractiveConsoleCommandMock *shutdown_cmd_mock;
};

TEST_P(TestInteractiveConsole, WhenReceivesCommandIfOKThenExecutesTheCommand)
{
    char *(*custom_fakes[])(const char *) = {GetParam().readline_handler,
                                             readline_shutdown_mock};
    SET_CUSTOM_FAKE_SEQ(readline, custom_fakes, 2);

    InteractiveConsoleWrapper::SetCommand set_command_func = GetParam().set_command_func;
    (console->*set_command_func)(cmd_mock);

    EXPECT_CALL(*cmd_mock, execute()).Times(1);
    EXPECT_CALL(*shutdown_cmd_mock, execute()).Times(AnyNumber());
    console->listen();
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestInteractiveConsole,
    ::testing::Values(
        TestInteractiveConsoleParams{"WhenReceivesShutdownCommandIfOKThenExecutesTheShutdownCommand", readline_shutdown_mock, &interactive_console::set_shutdown_command},
        TestInteractiveConsoleParams{"WhenReceivesFastCommandIfOKThenExecutesTheFastCommand", readline_fast_mock, &interactive_console::set_fast_command},
        TestInteractiveConsoleParams{"WhenReceivesSlowCommandIfOKThenExecutesTheSlowCommand", readline_slow_mock, &interactive_console::set_slow_command}),
    [](const testing::TestParamInfo<TestInteractiveConsoleParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestInteractiveConsole, WhenReceivesShudtownCommandIfOKThenStopListeningAndShutdown)
{
    readline_fake.custom_fake = readline_shutdown_mock;
    console->listen();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
