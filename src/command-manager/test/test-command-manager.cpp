#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <command-manager/command-manager.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;

class CommunicationsLayerMock : public communications_layer_interface
{
public:
    MOCK_METHOD(communications_layer_interface *, set_next_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(int, send_message, (const void *message, size_t size), (override));
    MOCK_METHOD(ssize_t, send_command, (const char *command, size_t command_size, char *response, size_t reponse_size), (override));
    MOCK_METHOD(ssize_t, recv_command, (const char *command, size_t command_size, char *response, size_t reponse_size), (override));
};

class TestCommandManager : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        comms_layer_mock = new CommunicationsLayerMock();
        command_mng = new command_manager(comms_layer_mock);
        memset(slow_response_buffer, 0, sizeof(slow_response_buffer));
    }

    virtual void TearDown()
    {
        delete command_mng;
        delete comms_layer_mock;
    }

protected:
    command_manager *command_mng;
    CommunicationsLayerMock *comms_layer_mock;
    const char expected_fast_command[13] = "FAST COMMAND";
    const char expected_slow_command[13] = "SLOW COMMAND";
    const char expected_slow_command_ack_response[13] = "ACK-RESPONSE";
    char slow_response_buffer[13];
};

TEST_F(TestCommandManager, WhenSendingFastCommandIfAckThenSendAndReturnAck)
{
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_fast_command), sizeof(expected_fast_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(command_manager::ack, command_manager::ack + sizeof(command_manager::ack)), Return(sizeof(command_manager::ack))));

    ASSERT_EQ(0, command_mng->send_fast_cmd());
}

TEST_F(TestCommandManager, WhenSendingFastCommandIfNackThenSendAndReturnNack)
{
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_fast_command), sizeof(expected_fast_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(command_manager::nack, command_manager::nack + sizeof(command_manager::nack)), Return(sizeof(command_manager::nack))));

    ASSERT_EQ(1, command_mng->send_fast_cmd());
}

TEST_F(TestCommandManager, WhenSendingFastCommandIfInternalErrorThenReturnError)
{
    int expected_error = -3;
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_fast_command), sizeof(expected_fast_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(command_manager::nack, command_manager::nack + sizeof(command_manager::nack)), Return(expected_error)));

    ASSERT_EQ(expected_error, command_mng->send_fast_cmd());
}

TEST_F(TestCommandManager, WhenSendingFastCommandIfResponseErrorThenReturnError)
{
    int expected_error = -1;
    char bad_response[] = "BAD";
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_fast_command), sizeof(expected_fast_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(bad_response, bad_response + sizeof(bad_response)), Return(sizeof(bad_response))));

    ASSERT_EQ(expected_error, command_mng->send_fast_cmd());
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfAckThenSendAndReturnAck)
{
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_slow_command), sizeof(expected_slow_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(expected_slow_command_ack_response, expected_slow_command_ack_response + sizeof(expected_slow_command_ack_response)), Return(sizeof(expected_slow_command_ack_response))));

    ASSERT_EQ(0, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
    ASSERT_STREQ(expected_slow_command_ack_response + 4, slow_response_buffer);
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfResponseBiggerThanBufferThenReturnError)
{
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_slow_command), sizeof(expected_slow_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(expected_slow_command_ack_response, expected_slow_command_ack_response + sizeof(expected_slow_command_ack_response)), Return(sizeof(slow_response_buffer) + 1)));

    ASSERT_EQ(-1, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfNackThenSendAndReturnNack)
{
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_slow_command), sizeof(expected_slow_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(command_manager::nack, command_manager::nack + sizeof(command_manager::nack)), Return(sizeof(command_manager::nack))));

    ASSERT_EQ(1, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfInternalErrorThenReturnError)
{
    int expected_error = -3;
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_slow_command), sizeof(expected_slow_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(command_manager::nack, command_manager::nack + sizeof(command_manager::nack)), Return(expected_error)));

    ASSERT_EQ(expected_error, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfResponseErrorThenReturnError)
{
    int expected_error = -1;
    char bad_response[] = "BAD";
    EXPECT_CALL(*comms_layer_mock, send_command(StrEq(expected_slow_command), sizeof(expected_slow_command), _, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<2>(bad_response, bad_response + sizeof(bad_response)), Return(sizeof(bad_response))));

    ASSERT_EQ(expected_error, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
