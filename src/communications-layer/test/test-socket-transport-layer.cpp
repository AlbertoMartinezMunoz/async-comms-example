#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <communications-layer/socket-transport-layer.hpp>

#include <condition_variable>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "fff.h"

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Return;

typedef void (*handler_t)(int signal);

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(ssize_t, write, int, const void *, size_t);
FAKE_VALUE_FUNC(ssize_t, read, int, void *, size_t);
FAKE_VALUE_FUNC(int, bind, int, const struct sockaddr *, socklen_t);
FAKE_VALUE_FUNC(int, accept, int, struct sockaddr *, socklen_t *);
FAKE_VALUE_FUNC(int, listen, int, int);

#undef FD_CLR
FAKE_VOID_FUNC(FD_CLR, int, fd_set *);
#undef FD_ISSET
FAKE_VALUE_FUNC(int, FD_ISSET, int, fd_set *);
#undef FD_SET
FAKE_VOID_FUNC(FD_SET, int, fd_set *);
#undef FD_ZERO
FAKE_VOID_FUNC(FD_ZERO, fd_set *);
FAKE_VALUE_FUNC(int, select, int, fd_set *, fd_set *, fd_set *, struct timeval *);

FAKE_VALUE_FUNC(int, sigaction, int, const struct sigaction *, struct sigaction *);

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

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

int select_mock(__attribute__((unused)) int nfds, __attribute__((unused)) fd_set *readfds,
                __attribute__((unused)) fd_set *writefds, __attribute__((unused)) fd_set *exceptfds,
                __attribute__((unused)) struct timeval *timeout) {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    return 0;
}

class CommunicationsLayerMock : public communications_layer_interface {
  public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer,
                (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(int, shutdown, ());
};

class CommandHandlerMock : public command_handler {
  public:
    virtual ~CommandHandlerMock() {}
    MOCK_METHOD(int, handle, ());
};

class TestSocketTransportLayer : public ::testing::Test {
  public:
    socket_transport_layer *layer;
    CommunicationsLayerMock *comms_layer_mock = nullptr;
    CommandHandlerMock *cmd_handler_mock = nullptr;
    const int expected_socket_fd = 1;

    virtual void SetUp() {
        RESET_FAKE(socket);
        RESET_FAKE(connect);
        RESET_FAKE(write);
        RESET_FAKE(read);
        RESET_FAKE(select);
        RESET_FAKE(sigaction);
        memset(message_buffer, 0, sizeof(message_buffer));
        socket_fake.return_val = expected_socket_fd;
        connect_fake.return_val = 0;
        write_fake.custom_fake = write_mock;
        read_fake.custom_fake = read_mock;
        select_fake.custom_fake = select_mock;
        comms_layer_mock = new CommunicationsLayerMock();
        cmd_handler_mock = new CommandHandlerMock();
        layer = new socket_transport_layer();
        layer->set_next_communications_layer(comms_layer_mock);
        ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    }

    virtual void TearDown() {
        delete layer;
        delete comms_layer_mock;
        delete cmd_handler_mock;
    }

    handler_t signal_cb;
};

TEST_F(TestSocketTransportLayer, WhenConnectingIfOKThenUseUnixSocket) {
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

TEST_F(TestSocketTransportLayer, WhenConnectingIfSocketKOThenReturnKO) {
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    socket_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenConnectingIfConnectKOThenReturnKO) {
    RESET_FAKE(socket);
    RESET_FAKE(connect);
    connect_fake.return_val = -1;
    ASSERT_EQ(-1, layer->connect_socket(SOCKET_NAME));
}

TEST_F(TestSocketTransportLayer, WhenSendingIfOKThenMessageIsSent) {
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
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
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    ASSERT_EQ(-1, layer->send(expected_message, sizeof(expected_message)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfOKThenMessageIsPassedToNextHandler) {
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(sizeof(expected_message), layer->recv(message_buffer, sizeof(message_buffer)));
    ASSERT_THAT(expected_message, ElementsAreArray(message_buffer, sizeof(expected_message)));
    ASSERT_EQ(expected_socket_fd, read_fake.arg0_val);
    ASSERT_EQ(sizeof(message_buffer), read_fake.arg2_val);
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfReadErrorThenReturnError) {
    RESET_FAKE(read);
    read_fake.return_val = -1;
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(sizeof(expected_message)));
    ASSERT_EQ(-1, layer->recv(message_buffer, sizeof(message_buffer)));
}

TEST_F(TestSocketTransportLayer, WhenReceveingIfRecvErrorThenReturnError) {
    ASSERT_EQ(0, layer->connect_socket(SOCKET_NAME));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(message_buffer))).Times(1).WillOnce(Return(-2));
    ASSERT_EQ(-2, layer->recv(message_buffer, sizeof(message_buffer)));
}

TEST_F(TestSocketTransportLayer, WhenListeningIfStopListenThenShouldStop) {
    layer->set_next_communications_layer(nullptr);
    using ::testing::Mock;
    EXPECT_CALL(*cmd_handler_mock, handle()).Times(0);
    std::thread t1(&socket_transport_layer::listen_connections, layer, SOCKET_NAME, cmd_handler_mock);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(true, Mock::VerifyAndClearExpectations(cmd_handler_mock));
    EXPECT_CALL(*cmd_handler_mock, handle()).Times(1).WillOnce(Return(0));
    layer->shutdown();
    t1.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
