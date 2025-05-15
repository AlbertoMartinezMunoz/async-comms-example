#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <communications-layer/socket-transport-layer.hpp>

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "fff.h"

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Return;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

char expected_write_buf[32];
char message[] = "TestMessage";

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count)
{
    memcpy(expected_write_buf, buf, count);
    return count;
}

class CommunicationsLayerMock : public communications_layer_interface
{
public:
    MOCK_METHOD(communications_layer_interface *, set_next_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
};

class TestSocketTransportLayer : public ::testing::Test
{
public:
    socket_transport_layer *layer;
    CommunicationsLayerMock *comms_layer_mock = nullptr;
    const int expected_socket_fd = 1;

    virtual void SetUp()
    {
        RESET_FAKE(socket);
        RESET_FAKE(connect);
        RESET_FAKE(write);
        socket_fake.return_val = expected_socket_fd;
        connect_fake.return_val = 0;
        write_fake.custom_fake = write_mock;
        comms_layer_mock = new CommunicationsLayerMock();
        layer = new socket_transport_layer();
        layer->set_next_layer(comms_layer_mock);
    }

    virtual void TearDown()
    {
        delete layer;
        delete comms_layer_mock;
    }
};

TEST_F(TestSocketTransportLayer, WhenConnectingIfOKThenUseUnixSocket)
{
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(AF_UNIX, socket_fake.arg0_val);
    ASSERT_EQ(SOCK_SEQPACKET, socket_fake.arg1_val);
    ASSERT_EQ(1, connect_fake.call_count);
    ASSERT_EQ(expected_socket_fd, connect_fake.arg0_val);
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfSocketKOThenReturnKO)
{
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfConnectKOThenReturnKO)
{
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfOKThenMessageIsSent)
{
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(message))).With(Args<0, 1>(ElementsAreArray(message, sizeof(message)))).Times(1).WillOnce(Return(sizeof(message)));
    ASSERT_EQ(sizeof(message), layer->send(message, sizeof(message)));
    ASSERT_EQ(expected_socket_fd, write_fake.arg0_val);
    ASSERT_EQ(sizeof(message), write_fake.arg2_val);
    ASSERT_THAT(message, ElementsAreArray(expected_write_buf, sizeof(message)));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfErrorThenReturnError)
{
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(-1, layer->send(message, sizeof(message)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
