#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <socket-comms/socket-comms.hpp>

#include "fff.h"
#include <unistd.h>

using testing::ElementsAreArray;

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);

char expected_write_buf[32];

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count)
{
    memcpy(expected_write_buf, buf, count);
    return count;
}

class TestSocketComms : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        layer = new socket_transport_layer();
        RESET_FAKE(write);
        write_fake.custom_fake = write_mock;
    }

    virtual void TearDown()
    {
        delete layer;
    }

protected:
    socket_transport_layer *layer;
};

TEST_F(TestSocketComms, WhenSendingIfOKThenSendAndReturnOK)
{
    int expected_socket_id = 5;
    char buffer[] = "TEST MESSAGE";
    ASSERT_EQ(0, layer->send(expected_socket_id, buffer, sizeof(buffer)));
    ASSERT_EQ(1, write_fake.call_count);
    ASSERT_EQ(expected_socket_id, write_fake.arg0_val);
    ASSERT_EQ(sizeof(buffer), write_fake.arg2_val);
    ASSERT_THAT(buffer, ElementsAreArray(expected_write_buf, sizeof(buffer)));
}

TEST_F(TestSocketComms, WhenSendingIfWriteErrorThenReturnError)
{
    int expected_socket_id = 5;
    char buffer[] = "TEST MESSAGE";
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(-1, layer->send(expected_socket_id, buffer, sizeof(buffer)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
