#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <communications-layer/socket-transport-layer.hpp>

#include <string.h>
#include <unistd.h>

#include "fff.h"

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Return;

typedef void (*handler_t)(int signal);

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(ssize_t, read, int, void *, size_t);

char message_buffer[32];
char expected_message[] = "TestMessage";

std::condition_variable cv;
std::mutex cv_m;

ssize_t write_mock(__attribute__((unused)) int fd, const void *buf, size_t count) {
    std::lock_guard<std::mutex> lk(cv_m);
    cv.notify_all();
    memcpy(message_buffer, buf, count);
    return count;
}

ssize_t read_mock(__attribute__((unused)) int fd, void *buf, __attribute__((unused)) size_t count) {
    memcpy(buf, expected_message, sizeof(expected_message));
    return sizeof(expected_message);
}

class CommunicationsLayerMock : public communications_layer_interface {
  public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer,
                (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(int, connect, ());
    MOCK_METHOD(int, disconnect, ());
    MOCK_METHOD(int, shutdown, ());
};

class CommandHandlerMock : public command_handler {
  public:
    virtual ~CommandHandlerMock() {}
    MOCK_METHOD(int, handle, ());
};

class SocketTransportLayerHelper : public socket_transport_layer {
  public:
    SocketTransportLayerHelper(int socket_id) : socket_transport_layer() { socket = socket_id; }
};

class TestSocketTransportLayer : public ::testing::Test {
  public:
    SocketTransportLayerHelper *layer;
    CommunicationsLayerMock *comms_layer_mock = nullptr;
    CommandHandlerMock *cmd_handler_mock = nullptr;
    const int expected_socket_fd = 1;

    virtual void SetUp() {
        RESET_FAKE(write);
        RESET_FAKE(read);
        memset(message_buffer, 0, sizeof(message_buffer));
        write_fake.custom_fake = write_mock;
        read_fake.custom_fake = read_mock;
        comms_layer_mock = new CommunicationsLayerMock();
        cmd_handler_mock = new CommandHandlerMock();
        layer = new SocketTransportLayerHelper(expected_socket_fd);
        layer->set_next_communications_layer(comms_layer_mock);
    }

    virtual void TearDown() {
        delete layer;
        delete comms_layer_mock;
        delete cmd_handler_mock;
    }

    handler_t signal_cb;
};

TEST_F(TestSocketTransportLayer, WhenSendingIfOKThenMessageIsSent) {
    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(expected_message)))
        .With(Args<0, 1>(ElementsAreArray(expected_message, sizeof(expected_message))))
        .Times(1)
        .WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(sizeof(expected_message), layer->send(expected_message, sizeof(expected_message)));
    ASSERT_EQ(expected_socket_fd, write_fake.arg0_val);
    ASSERT_EQ(sizeof(expected_message), write_fake.arg2_val);
    ASSERT_THAT(expected_message, ElementsAreArray(message_buffer, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfErrorThenReturnError) {
    RESET_FAKE(write);
    write_fake.return_val = -1;
    ASSERT_EQ(-1, layer->send(expected_message, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfOKThenMessageIsPassedToNextHandler) {
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(sizeof(expected_message), layer->recv(message_buffer, sizeof(message_buffer)));
    ASSERT_THAT(expected_message, ElementsAreArray(message_buffer, sizeof(expected_message)));
    ASSERT_EQ(expected_socket_fd, read_fake.arg0_val);
    ASSERT_EQ(sizeof(message_buffer), read_fake.arg2_val);
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfReadErrorThenReturnError) {
    RESET_FAKE(read);
    read_fake.return_val = -1;
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(-1, layer->recv(message_buffer, sizeof(message_buffer)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfRecvErrorThenReturnError) {
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(-2));
    ASSERT_EQ(-2, layer->recv(message_buffer, sizeof(message_buffer)));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
