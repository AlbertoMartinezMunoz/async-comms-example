#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <remote-command-manager/command-manager.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

class CommunicationsLayerMock : public communications_layer_interface
{
public:
    MOCK_METHOD(communications_layer_interface *, set_next_communications_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
};

class FastCmdObseverMock : public command_observer
{
public:
    MOCK_METHOD(int, process_command, ());
};

class SlowCmdObseverMock : public command_observer
{
public:
    MOCK_METHOD(int, process_command, ());
};

class TestCommandManager : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        communications_layer_mock = new CommunicationsLayerMock();
        fast_cmd_observer_mock = new FastCmdObseverMock();
        slow_cmd_observer_mock = new SlowCmdObseverMock();
        command_mng = new command_manager(communications_layer_mock);
        command_mng->subscribe_fast_cmd(fast_cmd_observer_mock);
        command_mng->subscribe_slow_cmd(slow_cmd_observer_mock);
        memset(slow_response_buffer, 0, sizeof(slow_response_buffer));
    }

    virtual void TearDown()
    {
        delete command_mng;
        delete fast_cmd_observer_mock;
        delete slow_cmd_observer_mock;
        delete communications_layer_mock;
    }

protected:
    command_manager *command_mng;
    CommunicationsLayerMock *communications_layer_mock;
    FastCmdObseverMock *fast_cmd_observer_mock;
    SlowCmdObseverMock *slow_cmd_observer_mock;
    const char expected_slow_command_ack_response[13] = "ACK-RESPONSE";
    char slow_response_buffer[13];
};

TEST_F(TestCommandManager, WhenReceivingFastCommandIfAckThenResponseAndReturnAck)
{
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::fast_cmd, command_manager::fast_cmd + sizeof(command_manager::fast_cmd)), Return(sizeof(command_manager::fast_cmd))));
    EXPECT_CALL(*fast_cmd_observer_mock, process_command()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_manager::ack), sizeof(command_manager::ack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_manager::ack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingSlowCommandIfAckThenResponseAndReturnAck)
{
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::slow_cmd, command_manager::slow_cmd + sizeof(command_manager::slow_cmd)), Return(sizeof(command_manager::slow_cmd))));
    EXPECT_CALL(*slow_cmd_observer_mock, process_command()).Times(1).WillOnce(Return(0));
    ;
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_manager::ack), sizeof(command_manager::ack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_manager::ack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingFastCommandIfNAckThenResponseAndReturnNAck)
{
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::fast_cmd, command_manager::fast_cmd + sizeof(command_manager::fast_cmd)), Return(sizeof(command_manager::fast_cmd))));
    EXPECT_CALL(*fast_cmd_observer_mock, process_command()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_manager::nack), sizeof(command_manager::nack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_manager::nack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingSlowCommandIfNAckThenResponseAndReturnNAck)
{
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::slow_cmd, command_manager::slow_cmd + sizeof(command_manager::slow_cmd)), Return(sizeof(command_manager::slow_cmd))));
    EXPECT_CALL(*slow_cmd_observer_mock, process_command()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*communications_layer_mock, send(StrEq(command_manager::nack), sizeof(command_manager::nack)))
        .Times(1)
        .WillOnce(Return(sizeof(command_manager::nack)));
    ASSERT_EQ(0, command_mng->incoming_message());
}

TEST_F(TestCommandManager, WhenReceivingFastCommandIfRecvErrorThenReturnError)
{
    int expected_error = -3;
    EXPECT_CALL(*communications_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(Return(expected_error));
    ASSERT_EQ(expected_error, command_mng->incoming_message());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
