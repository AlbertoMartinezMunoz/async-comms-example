#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands-implementations.hpp>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class LocalCommandReceiverMock : public local_command_receiver {
  public:
    MOCK_METHOD(int, send_fast_cmd, ());
    MOCK_METHOD(int, send_shutdown_cmd, ());
    MOCK_METHOD(int, send_slow_cmd, (char *response_buffer, size_t response_buffer_size));
};

template <class T> command *CreateLocalCommand(local_command_receiver *cmd_rcv);

template <> command *CreateLocalCommand<local_shutdown_command>(local_command_receiver *cmd_rcv) {
    return new local_shutdown_command(cmd_rcv, nullptr, nullptr);
}

template <> command *CreateLocalCommand<local_fast_command>(local_command_receiver *cmd_rcv) {
    return new local_fast_command(cmd_rcv);
}

template <> command *CreateLocalCommand<local_slow_command>(local_command_receiver *cmd_rcv) {
    return new local_slow_command(cmd_rcv);
}

template <class T> void CreateLocalCmdRcvExpectation(LocalCommandReceiverMock *cmd_rcv);

template <> void CreateLocalCmdRcvExpectation<local_shutdown_command>(LocalCommandReceiverMock *cmd_rcv) {
    EXPECT_CALL(*cmd_rcv, send_shutdown_cmd()).Times(1).WillOnce(Return(0));
}

template <> void CreateLocalCmdRcvExpectation<local_fast_command>(LocalCommandReceiverMock *cmd_rcv) {
    EXPECT_CALL(*cmd_rcv, send_fast_cmd()).Times(1).WillOnce(Return(0));
}

template <> void CreateLocalCmdRcvExpectation<local_slow_command>(LocalCommandReceiverMock *cmd_rcv) {
    EXPECT_CALL(*cmd_rcv, send_slow_cmd(_, _)).Times(1).WillOnce(Return(0));
}

template <typename T> class TestLocalCommands : public testing::Test {
  public:
    virtual void SetUp() {
        cmd_receiver_mock = new LocalCommandReceiverMock();
        local_command = CreateLocalCommand<T>(cmd_receiver_mock);
    }

    virtual void TearDown() {
        delete local_command;
        delete cmd_receiver_mock;
    }

  protected:
    command *local_command;
    LocalCommandReceiverMock *cmd_receiver_mock;
};

TYPED_TEST_SUITE_P(TestLocalCommands);

TYPED_TEST_P(TestLocalCommands, WhenExecutingLocalCmdIfOKThenSendCommand) {
    CreateLocalCmdRcvExpectation<TypeParam>(this->cmd_receiver_mock);
    this->local_command->execute();
    ASSERT_EQ(0, 0);
}

REGISTER_TYPED_TEST_SUITE_P(TestLocalCommands, WhenExecutingLocalCmdIfOKThenSendCommand);

using ShutdownTestingTypes = ::testing::Types<local_shutdown_command, local_fast_command, local_slow_command>;
INSTANTIATE_TYPED_TEST_SUITE_P(TestCommands, TestLocalCommands, ShutdownTestingTypes);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
