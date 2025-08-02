#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands-implementations.hpp>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;

class ShutdownReceiverMock : public shutdown_receiver {
  public:
    virtual ~ShutdownReceiverMock() {}
    MOCK_METHOD(int, shutdown, ());
};

class CommunicationsLayerMock : public communications_layer {
  public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer,
                (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(int, connect, ());
    MOCK_METHOD(int, disconnect, ());
    MOCK_METHOD(int, shutdown, ());
};

template <class T> command *CreateShutdownCommand(communications_layer *comms, shutdown_receiver *cli);

template <>
command *CreateShutdownCommand<local_shutdown_command>(communications_layer *comms, shutdown_receiver *cli) {
    return new local_shutdown_command(comms, cli);
}

template <>
command *CreateShutdownCommand<remote_shutdown_command>(communications_layer *comms, shutdown_receiver *cli) {
    return new remote_shutdown_command(comms, cli);
}

template <class T> void CreateShutdonCmdRcvExpectation(CommunicationsLayerMock *comms, ShutdownReceiverMock *cli);

template <>
void CreateShutdonCmdRcvExpectation<local_shutdown_command>(CommunicationsLayerMock *comms, ShutdownReceiverMock *cli) {
    EXPECT_CALL(*comms, connect()).Times(AnyNumber()).WillRepeatedly(Return(0));
    EXPECT_CALL(*comms, send(_, _)).Times(AnyNumber()).WillRepeatedly(Return(sizeof(command::remote_shutdown_cmd_id)));
    EXPECT_CALL(*comms, recv(_, _))
        .Times(AnyNumber())
        .WillRepeatedly(DoAll(SetArrayArgument<0>(command::ack, command::ack + sizeof(command::ack)),
                              Return(sizeof(command::ack))));
    EXPECT_CALL(*comms, disconnect()).Times(AnyNumber()).WillRepeatedly(Return(0));

    EXPECT_CALL(*cli, shutdown()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*comms, shutdown()).Times(1).WillOnce(Return(0));
}

template <>
void CreateShutdonCmdRcvExpectation<remote_shutdown_command>(CommunicationsLayerMock *comms,
                                                             ShutdownReceiverMock *cli) {
    EXPECT_CALL(*cli, shutdown()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*comms, shutdown()).Times(1).WillOnce(Return(0));
}

template <typename T> class TestShutdownCommands : public testing::Test {
  public:
    virtual void SetUp() {
        comms_mock = new CommunicationsLayerMock();
        cli_mock = new ShutdownReceiverMock();
        shutdown_command = CreateShutdownCommand<T>(comms_mock, cli_mock);
    }

    virtual void TearDown() {
        delete shutdown_command;
        delete cli_mock;
        delete comms_mock;
    }

  protected:
    command *shutdown_command;
    CommunicationsLayerMock *comms_mock;
    ShutdownReceiverMock *cli_mock;
};

TYPED_TEST_SUITE_P(TestShutdownCommands);

TYPED_TEST_P(TestShutdownCommands, WhenExecutingShutdownCmdIfOKThenShutdownCommsAndCli) {
    CreateShutdonCmdRcvExpectation<TypeParam>(this->comms_mock, this->cli_mock);
    ASSERT_EQ(0, this->shutdown_command->execute());
}

REGISTER_TYPED_TEST_SUITE_P(TestShutdownCommands, WhenExecutingShutdownCmdIfOKThenShutdownCommsAndCli);

using ShutdownTestingTypes = ::testing::Types<local_shutdown_command, remote_shutdown_command>;
INSTANTIATE_TYPED_TEST_SUITE_P(TestCommands, TestShutdownCommands, ShutdownTestingTypes);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
