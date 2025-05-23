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
    MOCK_METHOD(communications_layer_interface *, set_next_send_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(communications_layer_interface *, set_next_recv_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
};

class CommandManagerWrapper : public command_manager
{
public:
    CommandManagerWrapper(communications_layer_interface *application_layer, communications_layer_interface *transport_layer)
        : command_manager(application_layer, transport_layer) {}
    typedef int (CommandManagerWrapper::*SendSimpleCommandSignature)(void);
};

class TestCommandManagerParams
{
public:
    TestCommandManagerParams(std::string name, CommandManagerWrapper::SendSimpleCommandSignature send_cmd_func, const char *expected_cmd,
                             size_t expected_cmd_size, ssize_t send_expected_result, const char *expected_response, size_t expected_response_size,
                             ssize_t recv_expected_result, int expected_result)
        : name(name), send_cmd_func(send_cmd_func), expected_cmd(expected_cmd), expected_cmd_size(expected_cmd_size),
          expected_response(expected_response), expected_response_size(expected_response_size),
          expected_result(expected_result), send_expected_result(send_expected_result), recv_expected_result(recv_expected_result) {}

    static void *operator new(std::size_t count)
    {
        void *p = ::operator new(count);
        memset(p, 0, count);
        return p;
    }

    static void *operator new[](std::size_t count)
    {
        void *p = ::operator new[](count);
        memset(p, 0, count);
        return p;
    }

    std::string name;
    CommandManagerWrapper::SendSimpleCommandSignature send_cmd_func;
    const char *expected_cmd;
    size_t expected_cmd_size;
    const char *expected_response;
    size_t expected_response_size;
    int expected_result;
    ssize_t send_expected_result;
    ssize_t recv_expected_result;
};

class TestCommandManagerFastCmdParams : public TestCommandManagerParams
{
public:
    TestCommandManagerFastCmdParams(std::string name, ssize_t send_expected_result, const char *expected_response, size_t expected_response_size,
                                    ssize_t recv_expected_result, int expected_result)
        : TestCommandManagerParams(name, &command_manager::send_fast_cmd, "FAST COMMAND", sizeof("FAST COMMAND"),
                                   send_expected_result, expected_response, expected_response_size, recv_expected_result, expected_result) {}
};

class TestCommandManagerShutdownCmdParams : public TestCommandManagerParams
{
public:
    TestCommandManagerShutdownCmdParams(std::string name, ssize_t send_expected_result, const char *expected_response, size_t expected_response_size,
                                        ssize_t recv_expected_result, int expected_result)
        : TestCommandManagerParams(name, &command_manager::send_shutdown_cmd, command_manager::shutdown_cmd, sizeof(command_manager::shutdown_cmd),
                                   send_expected_result, expected_response, expected_response_size, recv_expected_result, expected_result) {}
};

class TestCommandManager : public testing::TestWithParam<TestCommandManagerParams>
{
public:
    virtual void SetUp()
    {
        application_layer_mock = new CommunicationsLayerMock();
        transport_layer_mock = new CommunicationsLayerMock();
        command_mng = new CommandManagerWrapper(application_layer_mock, transport_layer_mock);
        memset(slow_response_buffer, 0, sizeof(slow_response_buffer));
    }

    virtual void TearDown()
    {
        delete command_mng;
        delete application_layer_mock;
        delete transport_layer_mock;
    }

protected:
    CommandManagerWrapper *command_mng;
    CommunicationsLayerMock *application_layer_mock, *transport_layer_mock;
    const char expected_fast_command[13] = "FAST COMMAND";
    const char expected_slow_command[13] = "SLOW COMMAND";
    const char expected_slow_command_ack_response[13] = "ACK-RESPONSE";
    char slow_response_buffer[13];
};

TEST_P(TestCommandManager, TestNextSendHandlerMessage)
{
    EXPECT_CALL(*application_layer_mock, send(StrEq(GetParam().expected_cmd), GetParam().expected_cmd_size))
        .Times(1)
        .WillOnce(Return(GetParam().send_expected_result));

    if (GetParam().send_expected_result == (ssize_t)(GetParam().expected_cmd_size))
    {
        EXPECT_CALL(*transport_layer_mock, recv(_, _))
            .Times(1)
            .WillOnce(DoAll(SetArrayArgument<0>(GetParam().expected_response, GetParam().expected_response + GetParam().expected_response_size), Return(GetParam().recv_expected_result)));
    }

    CommandManagerWrapper::SendSimpleCommandSignature send_cmd_func = GetParam().send_cmd_func;

    ASSERT_EQ(GetParam().expected_result, (command_mng->*send_cmd_func)());
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestCommandManager,
    ::testing::Values(
        TestCommandManagerFastCmdParams{"WhenSendingFastCommandIfAckThenSendAndReturnAck", sizeof("FAST COMMAND"), command_manager::ack, sizeof(command_manager::ack), sizeof(command_manager::ack), 0},
        TestCommandManagerFastCmdParams{"WhenSendingFastCommandIfNackThenSendAndReturnNack", sizeof("FAST COMMAND"), command_manager::nack, sizeof(command_manager::nack), sizeof(command_manager::nack), 1},
        TestCommandManagerFastCmdParams{"WhenSendingFastCommandIfResponseErrorThenReturnError", sizeof("FAST COMMAND"), "BAD", sizeof("BAD"), sizeof("BAD"), -1},
        TestCommandManagerFastCmdParams{"WhenSendingFastCommandIfSendInternalErrorThenReturnError", -3, nullptr, 0, 0, -1},
        TestCommandManagerFastCmdParams{"WhenSendingFastCommandIfRecvInternalErrorThenReturnError", sizeof("FAST COMMAND"), nullptr, 0, -3, -3},
        TestCommandManagerShutdownCmdParams{"WhenSendingShutdownCommandIfAckThenSendAndReturnAck", sizeof(command_manager::shutdown_cmd), command_manager::ack, sizeof(command_manager::ack), sizeof(command_manager::ack), 0},
        TestCommandManagerShutdownCmdParams{"WhenSendingShutdownCommandIfNackThenSendAndReturnNack", sizeof(command_manager::shutdown_cmd), command_manager::nack, sizeof(command_manager::nack), sizeof(command_manager::nack), 1},
        TestCommandManagerShutdownCmdParams{"WhenSendingShutdownCommandIfResponseErrorThenReturnError", sizeof(command_manager::shutdown_cmd), "BAD", sizeof("BAD"), sizeof("BAD"), -1},
        TestCommandManagerShutdownCmdParams{"WhenSendingShutdownCommandIfSendInternalErrorThenReturnError", -3, nullptr, 0, 0, -1},
        TestCommandManagerShutdownCmdParams{"WhenSendingShutdownCommandIfRecvInternalErrorThenReturnError", sizeof(command_manager::shutdown_cmd), nullptr, 0, -3, -3}),
    [](const testing::TestParamInfo<TestCommandManagerParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestCommandManager, WhenSendingSlowCommandIfAckThenSendAndReturnAck)
{
    EXPECT_CALL(*application_layer_mock, send(StrEq(expected_slow_command), sizeof(expected_slow_command)))
        .Times(1)
        .WillOnce(Return(sizeof(expected_slow_command)));
    EXPECT_CALL(*transport_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::ack, command_manager::ack + sizeof(command_manager::ack)), Return(sizeof(command_manager::ack))));

    ASSERT_EQ(0, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfNackThenSendAndReturnNack)
{
    EXPECT_CALL(*application_layer_mock, send(StrEq(expected_slow_command), sizeof(expected_slow_command)))
        .Times(1)
        .WillOnce(Return(sizeof(expected_slow_command)));
    EXPECT_CALL(*transport_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::nack, command_manager::nack + sizeof(command_manager::nack)), Return(sizeof(command_manager::nack))));

    ASSERT_EQ(1, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfSendInternalErrorThenReturnError)
{
    int expected_error = -3;
    EXPECT_CALL(*application_layer_mock, send(StrEq(expected_slow_command), sizeof(expected_slow_command)))
        .Times(1)
        .WillOnce(Return(expected_error));

    ASSERT_EQ(-1, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfRecvInternalErrorThenReturnError)
{
    int expected_error = -3;
    EXPECT_CALL(*application_layer_mock, send(StrEq(expected_slow_command), sizeof(expected_slow_command)))
        .Times(1)
        .WillOnce(Return(sizeof(expected_slow_command)));
    EXPECT_CALL(*transport_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(command_manager::ack, command_manager::ack + sizeof(command_manager::nack)), Return(expected_error)));

    ASSERT_EQ(expected_error, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

TEST_F(TestCommandManager, WhenSendingSlowCommandIfResponseErrorThenReturnError)
{
    int expected_error = -1;
    char bad_response[] = "BAD";
    EXPECT_CALL(*application_layer_mock, send(StrEq(expected_slow_command), sizeof(expected_slow_command)))
        .Times(1)
        .WillOnce(Return(sizeof(expected_slow_command)));
    EXPECT_CALL(*transport_layer_mock, recv(_, _))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(bad_response, bad_response + sizeof(bad_response)), Return(sizeof(bad_response))));

    ASSERT_EQ(expected_error, command_mng->send_slow_cmd(slow_response_buffer, sizeof(slow_response_buffer)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
