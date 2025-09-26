#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <event-listeners/socket-event-listener.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "fff.h"
#include <sys/select.h>
#include <unistd.h>

using ::testing::AnyNumber;
using ::testing::Return;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(int, select, int, fd_set *, fd_set *, fd_set *, struct timeval *);

std::condition_variable cv;
std::mutex cv_m;

bool is_event = false;
int expected_listening_socket = 3;

ssize_t write_mock(__attribute__((unused)) int fd, __attribute__((unused)) const void *buf, size_t count) {
    std::lock_guard<std::mutex> lk(cv_m);
    is_event = false;
    cv.notify_one();
    return count;
}

int select_mock(__attribute__((unused)) int nfds, fd_set *readfds, __attribute__((unused)) fd_set *writefds,
                __attribute__((unused)) fd_set *exceptfds, __attribute__((unused)) struct timeval *timeout) {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    FD_ZERO(readfds);
    if (is_event)
        FD_SET(expected_listening_socket, readfds);
    return 0;
}

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.0.socket"

void stop_listener(socket_event_listener *l) {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    l->stop();
}

void event_received(socket_event_listener *l) {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        is_event = true;
        cv.notify_one();
    }
    stop_listener(l);
}

class EventListenerSocketMock : public event_listener_socket {
  public:
    MOCK_METHOD(int, init, ());
    MOCK_METHOD(int, connect, ());
    MOCK_METHOD(int, close, ());
    MOCK_METHOD(int, cleanup, ());
};

class EventHandlerMock : public event_listener_handler {
  public:
    MOCK_METHOD(void, handle, ());
};

class TestSocketEventListener : public ::testing::Test {
  public:
    virtual void SetUp() {
        RESET_FAKE(write);
        RESET_FAKE(select);

        write_fake.custom_fake = write_mock;
        select_fake.custom_fake = select_mock;

        event_handler = std::make_unique<EventHandlerMock>();
        socket_listener = std::make_unique<EventListenerSocketMock>();
        listener = std::make_unique<socket_event_listener>(SOCKET_NAME, socket_listener.get());

        EXPECT_CALL(*socket_listener.get(), init()).WillRepeatedly(Return(expected_listening_socket));
        EXPECT_CALL(*socket_listener.get(), cleanup()).Times(AnyNumber());
        EXPECT_CALL(*socket_listener.get(), connect()).Times(AnyNumber());
        EXPECT_CALL(*socket_listener.get(), close()).Times(AnyNumber());
    }

    virtual void TearDown() {}

    std::unique_ptr<socket_event_listener> listener;
    std::unique_ptr<EventHandlerMock> event_handler;
    std::unique_ptr<EventListenerSocketMock> socket_listener;
};

TEST_F(TestSocketEventListener, GivenIsListenigWhenStopThenWillReturnOK) {
    EXPECT_CALL(*event_handler.get(), handle()).Times(0);
    std::thread t(stop_listener, listener.get());
    EXPECT_EQ(0, listener->listen(event_handler.get()));
    t.join();
}

TEST_F(TestSocketEventListener, GivenListenerWhenIncomingEventThenListenerCallsHandler) {
    EXPECT_CALL(*event_handler.get(), handle()).Times(1);
    std::thread t(event_received, listener.get());
    EXPECT_EQ(0, listener->listen(event_handler.get()));
    t.join();
}

TEST_F(TestSocketEventListener, GivenListenerWhenListeningSocketErrorThenListenerStopListeningAndReturnsError) {
    EXPECT_CALL(*socket_listener.get(), init()).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(*event_handler.get(), handle()).Times(0);
    EXPECT_EQ(-1, listener->listen(event_handler.get()));
}

TEST_F(TestSocketEventListener, GivenListenerWhenSelectErrorThenListenerStopListeningAndReturnsError) {
    RESET_FAKE(select);
    select_fake.return_val = -1;
    EXPECT_CALL(*event_handler.get(), handle()).Times(0);
    EXPECT_EQ(-1, listener->listen(event_handler.get()));
}

TEST_F(TestSocketEventListener, GivenListenerWhenAcceptErrorThenListenerStopListeningAndReturnsOK) {
    EXPECT_CALL(*socket_listener.get(), connect()).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(*event_handler.get(), handle()).Times(0);
    std::thread t(event_received, listener.get());
    EXPECT_EQ(-1, listener->listen(event_handler.get()));
    t.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
