#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <networking/local-tcp-networking.hpp>

#include <sys/socket.h>
#include <unistd.h>

#include "fff.h"

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(int, close, int);

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

class TestLocalTCPNetworkingClient : public ::testing::Test {
  public:
    virtual void SetUp() {
        RESET_FAKE(socket);
        RESET_FAKE(connect);
        RESET_FAKE(close);
        socket_fake.return_val = expected_socket_fd;
        client = new networking_local_client(SOCKET_NAME);
    }

    virtual void TearDown() { delete client; }

    networking_client *client;
    const int expected_socket_fd = 15;
};

TEST_F(TestLocalTCPNetworkingClient, GivenClientIsConfiguredWhenConnectingOKThenMakeUnixConnectAndReturnsOK) {
    ASSERT_EQ(0, client->connect());
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(AF_UNIX, socket_fake.arg0_val);
    ASSERT_EQ(SOCK_SEQPACKET, socket_fake.arg1_val);
    ASSERT_EQ(1, connect_fake.call_count);
    ASSERT_EQ(expected_socket_fd, connect_fake.arg0_val);
}

TEST_F(TestLocalTCPNetworkingClient, GivenClientIsConfiguredWhenConnectingSocketErrorThenReturnsKO) {
    RESET_FAKE(socket);
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, client->connect());
}

TEST_F(TestLocalTCPNetworkingClient, GivenClientIsConfiguredWhenConnectingErrorThenReturnsKO) {
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, client->connect());
}

TEST_F(TestLocalTCPNetworkingClient, GivenClientIsConnectedWhenDisconnectingThenClosesSocketAndReturnsOK) {
    ASSERT_EQ(0, client->connect());
    ASSERT_EQ(0, client->disconnect());
    ASSERT_EQ(expected_socket_fd, close_fake.arg0_val);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
