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

class InteractiveConsoleObserverMock : public interactive_console_observer {
  public:
    MOCK_METHOD(int, console_incoming_message, (const char *), (const, override));
};

class TestInteractiveConsole : public testing::Test {
  public:
    virtual void SetUp() {
        RESET_FAKE(readline);
        RESET_FAKE(rl_callback_handler_install);
        RESET_FAKE(pselect);
        console = interactive_console::get_instance();
        console->set_shutdown_command(nullptr);
        readline_cb = rl_callback_handler_install_fake.arg1_val;
        pselect_fake.custom_fake = pselect_mock;
        observer_mock = new InteractiveConsoleObserverMock();
    }

    virtual void TearDown() {
        delete console;
        delete observer_mock;
    }

    rl_vcpfunc_t *readline_cb;
    interactive_console *console;
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
