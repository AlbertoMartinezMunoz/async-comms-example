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
FAKE_VALUE_FUNC(ssize_t, read, int, void *, size_t);

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

char message_buffer[32];
char expected_message[] = "TestMessage";

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count)
{
    memcpy(message_buffer, buf, count);
    return count;
}

ssize_t read_mock(__attribute__((unused)) int fd, void *buf, __attribute__((unused)) size_t count)
{
    memcpy(buf, expected_message, sizeof(expected_message));
    return sizeof(expected_message);
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
        RESET_FAKE(read);
        socket_fake.return_val = expected_socket_fd;
        connect_fake.return_val = 0;
        write_fake.custom_fake = write_mock;
        read_fake.custom_fake = read_mock;
        comms_layer_mock = new CommunicationsLayerMock();
        layer = new socket_transport_layer();
        layer->set_next_layer(comms_layer_mock);
        ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    }

    virtual void TearDown()
    {
        delete layer;
        delete comms_layer_mock;
    }
};

TEST_F(TestSocketTransportLayer, WhenConnectingIfOKThenUseUnixSocket)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = expected_socket_fd;
    connect_fake.return_val = 0;
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(AF_UNIX, socket_fake.arg0_val);
    ASSERT_EQ(SOCK_SEQPACKET, socket_fake.arg1_val);
    ASSERT_EQ(1, connect_fake.call_count);
    ASSERT_EQ(expected_socket_fd, connect_fake.arg0_val);
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfSocketKOThenReturnKO)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfConnectKOThenReturnKO)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfOKThenMessageIsSent)
{
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(expected_message))).With(Args<0, 1>(ElementsAreArray(expected_message, sizeof(expected_message)))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(sizeof(expected_message), layer->send(expected_message, sizeof(expected_message)));
    ASSERT_EQ(expected_socket_fd, write_fake.arg0_val);
    ASSERT_EQ(sizeof(expected_message), write_fake.arg2_val);
    ASSERT_THAT(expected_message, ElementsAreArray(message_buffer, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfErrorThenReturnError)
{
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(-1, layer->send(expected_message, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfOKThenMessageIsPassedToNextHandler)
{
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(expected_message))).With(Args<0, 1>(ElementsAreArray(expected_message, sizeof(expected_message)))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(sizeof(expected_message), layer->recv(message_buffer, sizeof(message_buffer)));
    ASSERT_EQ(expected_socket_fd, read_fake.arg0_val);
    ASSERT_EQ(sizeof(message_buffer), read_fake.arg2_val);
    ASSERT_THAT(expected_message, ElementsAreArray(message_buffer, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfReadErrorThenReturnError)
{
    RESET_FAKE(read);
    read_fake.return_val = -1;
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(-1, layer->recv(message_buffer, sizeof(message_buffer)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
