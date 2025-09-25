#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands-implementations.hpp>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

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

template <class T> command *CreateLocalCommand(communications_layer *comms, communications_layer *client);

template <>
command *CreateLocalCommand<local_shutdown_command>(communications_layer *comms, communications_layer *client) {
    return new local_shutdown_command(comms, client, nullptr, nullptr);
}

template <> command *CreateLocalCommand<local_fast_command>(communications_layer *comms, communications_layer *client) {
    return new local_fast_command(comms, client);
}

template <> command *CreateLocalCommand<local_slow_command>(communications_layer *comms, communications_layer *client) {
    return new local_slow_command(comms, client);
}

template <class T> void CreateLocalCmdRcvExpectation(CommunicationsLayerMock *comms, CommunicationsLayerMock *client);

template <>
void CreateLocalCmdRcvExpectation<local_shutdown_command>(CommunicationsLayerMock *comms,
                                                          CommunicationsLayerMock *client) {
    EXPECT_CALL(*client, connect()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*comms, send(StrEq(command::remote_shutdown_cmd_id), sizeof(command::remote_shutdown_cmd_id)))
        .Times(1)
        .WillOnce(Return(sizeof(command::remote_shutdown_cmd_id)));
    EXPECT_CALL(*comms, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command::ack, command::ack + sizeof(command::ack)),
                        Return(sizeof(command::ack))));
    EXPECT_CALL(*client, disconnect()).Times(1).WillOnce(Return(0));
}

template <>
void CreateLocalCmdRcvExpectation<local_fast_command>(CommunicationsLayerMock *comms, CommunicationsLayerMock *client) {
    EXPECT_CALL(*client, connect()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*comms, send(StrEq(command::remote_fast_cmd_id), sizeof(command::remote_fast_cmd_id)))
        .Times(1)
        .WillOnce(Return(sizeof(command::remote_fast_cmd_id)));
    EXPECT_CALL(*comms, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command::ack, command::ack + sizeof(command::ack)),
                        Return(sizeof(command::ack))));
    EXPECT_CALL(*client, disconnect()).Times(1).WillOnce(Return(0));
}

template <>
void CreateLocalCmdRcvExpectation<local_slow_command>(CommunicationsLayerMock *comms, CommunicationsLayerMock *client) {
    EXPECT_CALL(*client, connect()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*comms, send(StrEq(command::remote_slow_cmd_id), sizeof(command::remote_slow_cmd_id)))
        .Times(1)
        .WillOnce(Return(sizeof(command::remote_slow_cmd_id)));
    EXPECT_CALL(*comms, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command::ack, command::ack + sizeof(command::ack)),
                        Return(sizeof(command::ack))));
    EXPECT_CALL(*client, disconnect()).Times(1).WillOnce(Return(0));
}

template <typename T> class TestLocalCommands : public testing::Test {
  public:
    virtual void SetUp() {
        comms_mock = new CommunicationsLayerMock();
        client_mock = new CommunicationsLayerMock();
        local_command = CreateLocalCommand<T>(comms_mock, client_mock);
    }

    virtual void TearDown() {
        delete local_command;
        delete comms_mock;
        delete client_mock;
    }

  protected:
    command *local_command;
    CommunicationsLayerMock *comms_mock;
    CommunicationsLayerMock *client_mock;
};

TYPED_TEST_SUITE_P(TestLocalCommands);

TYPED_TEST_P(TestLocalCommands, WhenExecutingLocalCmdIfOKThenSendCommand) {
    CreateLocalCmdRcvExpectation<TypeParam>(this->comms_mock, this->client_mock);
    ASSERT_EQ(0, this->local_command->execute());
}

REGISTER_TYPED_TEST_SUITE_P(TestLocalCommands, WhenExecutingLocalCmdIfOKThenSendCommand);

using ShutdownTestingTypes = ::testing::Types<local_shutdown_command, local_fast_command, local_slow_command>;
INSTANTIATE_TYPED_TEST_SUITE_P(TestCommands, TestLocalCommands, ShutdownTestingTypes);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
