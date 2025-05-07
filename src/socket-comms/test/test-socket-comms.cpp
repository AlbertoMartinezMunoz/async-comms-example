#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <socket-comms/socket-comms.hpp>

#include "fff.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

using testing::ElementsAreArray;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(ssize_t, read, int, void *, size_t);
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const struct sockaddr *, socklen_t);

char expected_write_buf[32];
char expected_read_buf[18] = "TEST READ MESSAGE";
struct sockaddr_un connection_addr;

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count)
{
    memcpy(expected_write_buf, buf, count);
    return count;
}

ssize_t read_mock(__attribute__((unused)) int fd, void *buf, size_t count)
{
    if (sizeof(expected_read_buf) < count)
        count = sizeof(expected_read_buf);
    memcpy(buf, expected_read_buf, count);
    return count;
}

int connect_mock(__attribute__((unused)) int sockfd, __attribute__((unused)) const struct sockaddr *addr,
                 __attribute__((unused)) socklen_t addrlen)
{
    memcpy(&connection_addr, addr, sizeof(connection_addr));
    return 0;
}

class TestSocketComms : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        layer = new socket_transport_layer(expected_socket_id);
        RESET_FAKE(write);
        RESET_FAKE(read);
        RESET_FAKE(socket);
        RESET_FAKE(connect);
        write_fake.custom_fake = write_mock;
        read_fake.custom_fake = read_mock;
        socket_fake.return_val = expected_socket_id;
        connect_fake.custom_fake = connect_mock;
        layer->connect_remote(data_server);
    }

    virtual void TearDown()
    {
        delete layer;
    }

protected:
    socket_transport_layer *layer;
    const int expected_socket_id = 5;
    const char data_server[22] = "TEST_DATA_SERVER_PATH";
};

TEST_F(TestSocketComms, WhenConnectingIfOKThenConnectAndReturnOK)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = expected_socket_id;
    connect_fake.custom_fake = connect_mock;
    ASSERT_EQ(0, layer->connect_remote(data_server));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(AF_UNIX, socket_fake.arg0_val);
    ASSERT_EQ(SOCK_SEQPACKET, socket_fake.arg1_val);
    ASSERT_EQ(0, socket_fake.arg2_val);

    ASSERT_EQ(1, connect_fake.call_count);
    ASSERT_EQ(expected_socket_id, connect_fake.arg0_val);
    ASSERT_STREQ(data_server, connection_addr.sun_path);
    ASSERT_EQ(sizeof(connection_addr), connect_fake.arg2_val);
}

TEST_F(TestSocketComms, WhenConnectingIfSocketErrorThenReturnError)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_remote(data_server));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(0, connect_fake.call_count);
}

TEST_F(TestSocketComms, WhenConnectingIfConnectionErrorThenReturnError)
{
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = expected_socket_id;
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_remote(data_server));
    ASSERT_EQ(1, socket_fake.call_count);
    ASSERT_EQ(1, connect_fake.call_count);
}

TEST_F(TestSocketComms, WhenSendingIfOKThenSendAndReturnOK)
{
    char buffer[] = "TEST MESSAGE";
    ASSERT_EQ(0, layer->send(buffer, sizeof(buffer)));
    ASSERT_EQ(1, write_fake.call_count);
    ASSERT_EQ(expected_socket_id, write_fake.arg0_val);
    ASSERT_EQ(sizeof(buffer), write_fake.arg2_val);
    ASSERT_THAT(buffer, ElementsAreArray(expected_write_buf, sizeof(buffer)));
}

TEST_F(TestSocketComms, WhenSendingIfWriteErrorThenReturnError)
{
    char buffer[] = "TEST MESSAGE";
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(-1, layer->send(buffer, sizeof(buffer)));
}

TEST_F(TestSocketComms, WhenReceivingIfOKThenReadAndReturnReadBytes)
{
    char buffer[32];
    ASSERT_EQ(sizeof(expected_read_buf), layer->receive(buffer, sizeof(buffer)));
    ASSERT_EQ(1, read_fake.call_count);
    ASSERT_EQ(expected_socket_id, read_fake.arg0_val);
    ASSERT_EQ(sizeof(buffer), read_fake.arg2_val);
    ASSERT_THAT(expected_read_buf, ElementsAreArray(buffer, sizeof(expected_read_buf)));
}

TEST_F(TestSocketComms, WhenReceivingIfReadErrorThenReturnError)
{
    char buffer[32];
    RESET_FAKE(read);
    read_fake.return_val = -1;
    ASSERT_EQ(-1, layer->receive(buffer, sizeof(buffer)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
