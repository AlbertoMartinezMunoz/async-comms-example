#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <communications-layer/socket-transport-layer.hpp>

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "fff.h"

using testing::ElementsAreArray;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

uint8_t expected_write_buf[32];
uint8_t message[] = "TestMessage";

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count)
{
    memcpy(expected_write_buf, buf, count);
    return count;
}

class TestSocketTransportLayer : public ::testing::Test
{
public:
    socket_transport_layer *transport_layer;
    const int expected_socket_fd = 1;

    virtual void SetUp()
    {
        RESET_FAKE(socket);
        RESET_FAKE(connect);
        RESET_FAKE(write);
        socket_fake.return_val = expected_socket_fd;
        connect_fake.return_val = 0;
        write_fake.custom_fake = write_mock;
        transport_layer = new socket_transport_layer();
    }

    virtual void TearDown()
    {
        delete transport_layer;
    }
};

TEST_F(TestSocketTransportLayer, WhenConnectingIfOKThenUseUnixSocket)
{
    ASSERT_EQ(0, transport_layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(AF_UNIX, socket_fake.arg0_val);
    ASSERT_EQ(SOCK_SEQPACKET, socket_fake.arg1_val);
    ASSERT_EQ(1, connect_fake.call_count);
    ASSERT_EQ(expected_socket_fd, connect_fake.arg0_val);
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfSocketKOThenReturnKO)
{
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, transport_layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfConnectKOThenReturnKO)
{
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, transport_layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfOKThenMessageIsSent)
{
    ASSERT_EQ(0, transport_layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(0, transport_layer->send_message(message, sizeof(message)));
    ASSERT_EQ(expected_socket_fd, write_fake.arg0_val);
    ASSERT_EQ(sizeof(message), write_fake.arg2_val);
    ASSERT_THAT(message, ElementsAreArray(expected_write_buf, sizeof(message)));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfErrorThenReturnError)
{
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(0, transport_layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(-1, transport_layer->send_message(message, sizeof(message)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
