#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <interactive-console/interactive-console.hpp>
#include <readline/history.h>
#include <readline/readline.h>
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
                 __attribute__((unused)) const struct timespec *timeout,
                 __attribute__((unused)) const sigset_t *sigmask) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}

class InteractiveConsoleCommandMock : public interactive_console_command {
  public:
    MOCK_METHOD(void, execute, (), (const, override));
};

class InteractiveConsoleObserverMock : public interactive_console_observer {
  public:
    MOCK_METHOD(int, console_incoming_message, (const char *), (const, override));
};

class InteractiveConsoleWrapper : public interactive_console {
  public:
    InteractiveConsoleWrapper() : interactive_console() {}
    typedef void (InteractiveConsoleWrapper::*SetCommand)(interactive_console_command *);
};

class TestInteractiveConsoleParams {
  public:
    TestInteractiveConsoleParams(std::string name, const char *console_cmd,
                                 InteractiveConsoleWrapper::SetCommand set_command_func)
        : name(name), console_cmd(console_cmd), set_command_func(set_command_func) {}

    static void *operator new(std::size_t count) {
        void *p = ::operator new(count);
        memset(p, 0, count);
        return p;
    }

    static void *operator new[](std::size_t count) {
        void *p = ::operator new[](count);
        memset(p, 0, count);
        return p;
    }

    std::string name;
    const char *console_cmd;
    InteractiveConsoleWrapper::SetCommand set_command_func;
};

class TestInteractiveConsole : public testing::TestWithParam<TestInteractiveConsoleParams> {
  public:
    virtual void SetUp() {
        RESET_FAKE(readline);
        RESET_FAKE(rl_callback_handler_install);
        RESET_FAKE(pselect);
        cmd_mock = new InteractiveConsoleCommandMock();
        console = new InteractiveConsoleWrapper();
        console->set_shutdown_command(nullptr);
        readline_cb = rl_callback_handler_install_fake.arg1_val;
        pselect_fake.custom_fake = pselect_mock;
        observer_mock = new InteractiveConsoleObserverMock();
    }

    virtual void TearDown() {
        delete console;
        delete cmd_mock;
        delete observer_mock;
    }

    rl_vcpfunc_t *readline_cb;
    InteractiveConsoleWrapper *console;
    InteractiveConsoleCommandMock *cmd_mock;
    InteractiveConsoleObserverMock *observer_mock;
};

TEST_F(TestInteractiveConsole, WhenListeningIfStopThenStopListeningAndShutdown) {
    std::thread t1(&interactive_console::listen, console, std::ref(observer_mock));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    console->shutdown();
    t1.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
