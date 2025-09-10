#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <networking/local-tcp-networking.hpp>

#include <bits/select.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include <condition_variable>
#include <memory>

#include <iostream>

#include "fff.h"

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(int, bind, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(int, accept, int, struct sockaddr *, socklen_t *);
FAKE_VALUE_FUNC(int, listen, int, int);

FAKE_VALUE_FUNC(int, select, int, fd_set *, fd_set *, fd_set *, struct timeval *);
#undef FD_ISSET
FAKE_VALUE_FUNC(int, FD_ISSET, int, fd_set *);

std::condition_variable cv;
std::mutex cv_m;

ssize_t write_mock(__attribute__((unused)) int fd, __attribute__((unused)) const void *buf, size_t count) {
    std::lock_guard<std::mutex> lk(cv_m);
    cv.notify_one();
    return count;
}

int select_mock(__attribute__((unused)) int nfds, __attribute__((unused)) fd_set *readfds,
                __attribute__((unused)) fd_set *writefds, __attribute__((unused)) fd_set *exceptfds,
                __attribute__((unused)) struct timeval *timeout) {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    return 0;
}

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.0.socket"

class NetworkingServerObserverMock : public networking_server_observer {
  public:
    MOCK_METHOD(void, client_connected, ());
};

class TestLocalTCPNetworkingServer : public ::testing::Test {
  public:
    virtual void SetUp() {
        RESET_FAKE(write);
        RESET_FAKE(select);
        RESET_FAKE(socket);
        RESET_FAKE(bind);
        RESET_FAKE(listen);
        RESET_FAKE(accept);
        RESET_FAKE(FD_ISSET);

        FD_ISSET_fake.return_val = 0;
        write_fake.custom_fake = write_mock;
        select_fake.custom_fake = select_mock;

        server = std::make_unique<networking_local_server>();
        observer_mock = std::make_unique<NetworkingServerObserverMock>();
    }

    virtual void TearDown() {}

  protected:
    std::unique_ptr<networking_server> server;
    std::unique_ptr<NetworkingServerObserverMock> observer_mock;
};

TEST_F(TestLocalTCPNetworkingServer, GivenServerIsListeningWhenStopMethodThenServerStopListeningAndReturnsOK) {
    EXPECT_CALL(*observer_mock, client_connected()).Times(1);
    std::thread t(&networking_server::listen, server.get(), SOCKET_NAME, observer_mock.get());
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    server->stop();
    t.join();
}

TEST_F(TestLocalTCPNetworkingServer, GivenServerWhenSocketErrorThenServerStopListeningAndReturnsError) {
    socket_fake.return_val = -1;
    EXPECT_CALL(*observer_mock, client_connected()).Times(0);
    EXPECT_EQ(-1, server->listen(SOCKET_NAME, observer_mock.get()));
}

TEST_F(TestLocalTCPNetworkingServer, GivenServerWhenBindingErrorThenServerStopListeningAndReturnsError) {
    bind_fake.return_val = -1;
    EXPECT_CALL(*observer_mock, client_connected()).Times(0);
    EXPECT_EQ(-1, server->listen(SOCKET_NAME, observer_mock.get()));
}

TEST_F(TestLocalTCPNetworkingServer, GivenServerWhenListeningErrorThenServerStopListeningAndReturnsError) {
    listen_fake.return_val = -1;
    EXPECT_CALL(*observer_mock, client_connected()).Times(0);
    EXPECT_EQ(-1, server->listen(SOCKET_NAME, observer_mock.get()));
}

TEST_F(TestLocalTCPNetworkingServer, GivenServerWhenSelectErrorThenServerStopListeningAndReturnsOK) {
    RESET_FAKE(select);
    select_fake.return_val = -1;
    EXPECT_CALL(*observer_mock, client_connected()).Times(0);
    EXPECT_EQ(0, server->listen(SOCKET_NAME, observer_mock.get()));
}

TEST_F(TestLocalTCPNetworkingServer, GivenServerWhenAcceptErrorThenServerStopListeningAndReturnsOK) {
    accept_fake.return_val = -1;
    EXPECT_CALL(*observer_mock, client_connected()).Times(0);
    std::thread t(&networking_server::listen, server.get(), SOCKET_NAME, observer_mock.get());
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        FD_ISSET_fake.return_val = 1;
        cv.notify_one();
    }
    t.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
