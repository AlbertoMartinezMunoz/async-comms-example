#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <socket-comms/socket-comms.hpp>

#include "fff.h"
#include <unistd.h>

using testing::ElementsAreArray;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(ssize_t, read, int, void *, size_t);

char expected_write_buf[32];
char expected_read_buf[] = "TEST READ MESSAGE";

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

class TestSocketComms : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        layer = new socket_transport_layer(expected_socket_id);
        RESET_FAKE(write);
        RESET_FAKE(read);
        write_fake.custom_fake = write_mock;
        read_fake.custom_fake = read_mock;
    }

    virtual void TearDown()
    {
        delete layer;
    }

protected:
    socket_transport_layer *layer;
    const int expected_socket_id = 5;
};

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
