#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <command-manager/command-ids.hpp>
#include <command-manager/command-remote-handler.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

class CommandMock : public command {
  public:
    MOCK_METHOD(int, execute, (), (const));
};

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

class TestCommandRemoteHandlerParams {
  public:
    TestCommandRemoteHandlerParams(std::string name, const char *remote_command_id, size_t remote_command_id_size)
        : name(name), remote_command_id(remote_command_id), remote_command_id_size(remote_command_id_size) {}

    static void *operator new(std::size_t count) {
        void *p = ::operator new(count);
        memset(p, 0, count);
        return p;
    }

    static void *operator new[](std::size_t count) {
        void *p = ::operator new[](count);
        memset(p, 0, count);
        return p;
    }

    std::string name;
    const char *remote_command_id;
    size_t remote_command_id_size;
};

class TestCommandRemoteHandler : public testing::TestWithParam<TestCommandRemoteHandlerParams> {
  public:
    virtual void SetUp() {
        communications_layer_mock = new CommunicationsLayerMock();
        fast_cmd_mock = new CommandMock();
        slow_cmd_mock = new CommandMock();
        shutdown_cmd_mock = new CommandMock();
        command_handler = new command_remote_handler(communications_layer_mock);
        command_handler->subscribe_fast_cmd(fast_cmd_mock);
        command_handler->subscribe_slow_cmd(slow_cmd_mock);
        command_handler->subscribe_shutdown_cmd(shutdown_cmd_mock);
    }

    virtual void TearDown() {
        delete command_handler;
        delete shutdown_cmd_mock;
        delete slow_cmd_mock;
        delete fast_cmd_mock;
        delete communications_layer_mock;
    }

  protected:
    command_remote_handler *command_handler;
    CommunicationsLayerMock *communications_layer_mock;
    CommandMock *fast_cmd_mock;
    CommandMock *slow_cmd_mock;
    CommandMock *shutdown_cmd_mock;

    CommandMock *GetExpectedMock(const char *remote_command) {
        if (strcmp(command_ids::fast_cmd_id, remote_command) == 0)
            return fast_cmd_mock;
        else if (strcmp(command_ids::slow_cmd_id, remote_command) == 0)
            return slow_cmd_mock;
        else if (strcmp(command_ids::shutdown_cmd_id, remote_command) == 0)
            return shutdown_cmd_mock;
        return nullptr;
    }
};

TEST_P(TestCommandRemoteHandler, GivenCommandRemoteHandlerWhenReceiveACommandThenExecutesTheCommand) {
    const char *cmd_id = (const char *)(GetParam().remote_command_id);
    size_t cmd_id_size = (size_t)(GetParam().remote_command_id_size);
    CommandMock *cmd_mock = GetExpectedMock(cmd_id);

    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(cmd_id, cmd_id + cmd_id_size), Return(cmd_id_size)));
    EXPECT_CALL(*cmd_mock, execute()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::ack), sizeof(command_ids::ack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::ack)));
    ASSERT_EQ(0, command_handler->handle());
}

TEST_P(TestCommandRemoteHandler, GivenCommandRemoteHandlerWhenCommandErrorThenReturnNack) {
    const char *cmd_id = (const char *)(GetParam().remote_command_id);
    size_t cmd_id_size = (size_t)(GetParam().remote_command_id_size);
    CommandMock *cmd_mock = GetExpectedMock(cmd_id);

    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(cmd_id, cmd_id + cmd_id_size), Return(cmd_id_size)));
    EXPECT_CALL(*cmd_mock, execute()).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::nack), sizeof(command_ids::nack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::nack)));
    ASSERT_EQ(0, command_handler->handle());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    , TestCommandRemoteHandler,
    ::testing::Values(
        TestCommandRemoteHandlerParams{"FastCommand", command_ids::fast_cmd_id, sizeof(command_ids::fast_cmd_id)},
        TestCommandRemoteHandlerParams{"SlowCommand", command_ids::slow_cmd_id, sizeof(command_ids::slow_cmd_id)},
        TestCommandRemoteHandlerParams{"ShutdownCommand", command_ids::shutdown_cmd_id, sizeof(command_ids::shutdown_cmd_id)}),
    [](const testing::TestParamInfo<TestCommandRemoteHandlerParams> &info) { return info.param.name; });
// clang-format on

TEST_F(TestCommandRemoteHandler, GivenCommandRemoteHandlerWhenRecvErrorThenReturnError) {
    int expected_error = -1;
    EXPECT_CALL(*communications_layer_mock, recv(_, _)).Times(1).WillOnce(Return(expected_error));
    ASSERT_EQ(expected_error, command_handler->handle());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
