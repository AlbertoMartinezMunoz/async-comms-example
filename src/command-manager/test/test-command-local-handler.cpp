#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <command-manager/command-local-handler.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

class CommandMock : public command {
  public:
    MOCK_METHOD(int, execute, (), (const));
};

class InteractiveConsoleMock : public communications_layer_interface {
  public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer,
                (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(int, connect, ());
    MOCK_METHOD(int, disconnect, ());
    MOCK_METHOD(int, shutdown, ());
};

class TestCommandLocalHandlerParams {
  public:
    TestCommandLocalHandlerParams(std::string name, const char *local_command_id, size_t local_command_id_size)
        : name(name), local_command_id(local_command_id), local_command_id_size(local_command_id_size) {}

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
    const char *local_command_id;
    size_t local_command_id_size;
};

class TestCommandLocalHandler : public testing::TestWithParam<TestCommandLocalHandlerParams> {
  public:
    virtual void SetUp() {
        fast_cmd_mock = new CommandMock();
        slow_cmd_mock = new CommandMock();
        shutdown_cmd_mock = new CommandMock();
        cli_mock = new InteractiveConsoleMock();
        command_handler = new command_local_handler(cli_mock);
        command_handler->subscribe_fast_cmd(fast_cmd_mock);
        command_handler->subscribe_slow_cmd(slow_cmd_mock);
        command_handler->subscribe_shutdown_cmd(shutdown_cmd_mock);
    }

    virtual void TearDown() {
        delete command_handler;
        delete cli_mock;
        delete shutdown_cmd_mock;
        delete slow_cmd_mock;
        delete fast_cmd_mock;
    }

  protected:
    command_local_handler *command_handler;
    CommandMock *fast_cmd_mock;
    CommandMock *slow_cmd_mock;
    CommandMock *shutdown_cmd_mock;
    InteractiveConsoleMock *cli_mock;

    CommandMock *GetExpectedMock(const char *local_command) {
        if (strcmp(command::local_fast_cmd_id, local_command) == 0)
            return fast_cmd_mock;
        else if (strcmp(command::local_slow_cmd_id, local_command) == 0)
            return slow_cmd_mock;
        else if (strcmp(command::local_shutdown_cmd_id, local_command) == 0)
            return shutdown_cmd_mock;
        return nullptr;
    }
};

TEST_P(TestCommandLocalHandler, GivenCommandLocalHandlerWhenReceiveACommandThenExecutesTheCommand) {
    const char *cmd_id = (const char *)(GetParam().local_command_id);
    size_t cmd_id_size = (size_t)(GetParam().local_command_id_size);
    CommandMock *cmd_mock = GetExpectedMock(cmd_id);

    EXPECT_CALL(*cli_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(cmd_id, cmd_id + cmd_id_size), Return(cmd_id_size)));

    EXPECT_CALL(*cmd_mock, execute()).Times(1).WillOnce(Return(0));

    ASSERT_EQ(0, command_handler->handle());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    , TestCommandLocalHandler,
    ::testing::Values(
        TestCommandLocalHandlerParams{"FastCommand", command::local_fast_cmd_id, sizeof(command::local_fast_cmd_id)},
        TestCommandLocalHandlerParams{"SlowCommand", command::local_slow_cmd_id, sizeof(command::local_slow_cmd_id)},
        TestCommandLocalHandlerParams{"ShutdownCommand", command::local_shutdown_cmd_id, sizeof(command::local_shutdown_cmd_id)}),
    [](const testing::TestParamInfo<TestCommandLocalHandlerParams> &info) { return info.param.name; });
// clang-format on

TEST_F(TestCommandLocalHandler, GivenCommandLocalHandlerWhenRecvErrorThenReturnError) {
    int expected_error = -1;
    EXPECT_CALL(*cli_mock, recv(_, _)).Times(1).WillOnce(Return(expected_error));
    ASSERT_EQ(expected_error, command_handler->handle());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
