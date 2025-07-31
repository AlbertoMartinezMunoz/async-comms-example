#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <command-manager/command-ids.hpp>
#include <command-manager/command-manager.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

class CommunicationsLayerMock : public communications_layer_interface {
  public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer,
                (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(int, shutdown, ());
};

class CommandMock : public command {
  public:
    MOCK_METHOD(int, execute, (), (const));
};

class TestCommandManager : public ::testing::Test {
  public:
    virtual void SetUp() {
        communications_layer_mock = new CommunicationsLayerMock();
        fast_cmd_mock = new CommandMock();
        slow_cmd_mock = new CommandMock();
        command_mng = new command_manager(communications_layer_mock, NULL, NULL);
        command_mng->subscribe_fast_cmd(fast_cmd_mock);
        command_mng->subscribe_slow_cmd(slow_cmd_mock);
        memset(slow_response_buffer, 0, sizeof(slow_response_buffer));
    }

    virtual void TearDown() {
        delete command_mng;
        delete fast_cmd_mock;
        delete slow_cmd_mock;
        delete communications_layer_mock;
    }

  protected:
    command_manager *command_mng;
    CommunicationsLayerMock *communications_layer_mock;
    CommandMock *fast_cmd_mock;
    CommandMock *slow_cmd_mock;
    const char expected_slow_command_ack_response[13] = "ACK-RESPONSE";
    char slow_response_buffer[13];
};

TEST_F(TestCommandManager, WhenReceivingFastCommandIfAckThenResponseAndReturnAck) {
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(
            SetArrayArgument<0>(command_ids::fast_cmd_id, command_ids::fast_cmd_id + sizeof(command_ids::fast_cmd_id)),
            Return(sizeof(command_ids::fast_cmd_id))));
    EXPECT_CALL(*fast_cmd_mock, execute()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::ack), sizeof(command_ids::ack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::ack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingSlowCommandIfAckThenResponseAndReturnAck) {
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(
            SetArrayArgument<0>(command_ids::slow_cmd_id, command_ids::slow_cmd_id + sizeof(command_ids::slow_cmd_id)),
            Return(sizeof(command_ids::slow_cmd_id))));
    EXPECT_CALL(*slow_cmd_mock, execute()).Times(1).WillOnce(Return(0));
    ;
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::ack), sizeof(command_ids::ack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::ack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingFastCommandIfNAckThenResponseAndReturnNAck) {
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(
            SetArrayArgument<0>(command_ids::fast_cmd_id, command_ids::fast_cmd_id + sizeof(command_ids::fast_cmd_id)),
            Return(sizeof(command_ids::fast_cmd_id))));
    EXPECT_CALL(*fast_cmd_mock, execute()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::nack), sizeof(command_ids::nack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::nack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingSlowCommandIfNAckThenResponseAndReturnNAck) {
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(
            SetArrayArgument<0>(command_ids::slow_cmd_id, command_ids::slow_cmd_id + sizeof(command_ids::slow_cmd_id)),
            Return(sizeof(command_ids::slow_cmd_id))));
    EXPECT_CALL(*slow_cmd_mock, execute()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_ids::nack), sizeof(command_ids::nack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_ids::nack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingFastCommandIfRecvErrorThenReturnError) {
    int expected_error = -3;
    EXPECT_CALL(*communications_layer_mock, recv(_, _)).Times(1).WillOnce(Return(expected_error));
    ASSERT_EQ(expected_error, command_mng->incoming_message());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
