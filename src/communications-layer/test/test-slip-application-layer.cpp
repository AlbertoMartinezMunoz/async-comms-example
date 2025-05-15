#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <communications-layer/slip-application-layer.hpp>
#include <cstdlib>
#include <cstring>
#include <vector>

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAreArray;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::StrEq;

class CommunicationsLayerMock : public communications_layer_interface
{
public:
    MOCK_METHOD(communications_layer_interface *, set_next_layer, (communications_layer_interface * handler), (override));
    MOCK_METHOD(ssize_t, send, (const char *buffer, size_t buffer_size), (override));
    MOCK_METHOD(ssize_t, recv, (char *buffer, size_t buffer_size), (override));
};

class TestSlipTransportLayerParams
{
public:
    TestSlipTransportLayerParams(std::string name, int next_handler_return, char *msg, size_t msg_size, char *expected_msg, size_t expected_msg_size) : name(name), next_handler_return(next_handler_return), msg(msg), msg_size(msg_size), expected_msg(expected_msg), expected_msg_size(expected_msg_size) {}

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
    int next_handler_return;
    char *msg;
    size_t msg_size;
    const char *expected_msg;
    size_t expected_msg_size;
};

char decoded_simple_msg[4] = {0x01, 0x05, 0x06, 0x10};
char encoded_simple_msg[5] = {0x01, 0x05, 0x06, 0x10, (char)slip_application_layer::END};
char decoded_escape_end_msg[4] = {0x01, 0x05, (char)slip_application_layer::END, 0x10};
char encoded_escape_end_msg[6] = {0x01, 0x05, (char)slip_application_layer::ESC, (char)slip_application_layer::ESC_END, 0x10, (char)slip_application_layer::END};
char decoded_escape_esc_msg[4] = {0x01, 0x05, (char)slip_application_layer::ESC, 0x10};
char encoded_escape_esc_msg[6] = {0x01, 0x05, (char)slip_application_layer::ESC, (char)slip_application_layer::ESC_ESC, 0x10, (char)slip_application_layer::END};

class TestSendSlipTransportLayer : public testing::TestWithParam<TestSlipTransportLayerParams>
{
public:
    slip_application_layer *layer = nullptr;
    CommunicationsLayerMock *comms_layer_mock = nullptr;

    virtual void SetUp()
    {
        layer = new slip_application_layer();
        comms_layer_mock = new CommunicationsLayerMock();
        layer->set_next_layer(comms_layer_mock);
    }

    virtual void TearDown()
    {
        delete layer;
        delete comms_layer_mock;
    }
};

TEST_P(TestSendSlipTransportLayer, TestNextSendHandlerMessage)
{
    EXPECT_CALL(*comms_layer_mock, send(_, GetParam().expected_msg_size)).With(Args<0, 1>(ElementsAreArray(GetParam().expected_msg, GetParam().expected_msg_size))).Times(1).WillOnce(Return(GetParam().expected_msg_size));
    ASSERT_EQ(GetParam().expected_msg_size, layer->send(GetParam().msg, GetParam().msg_size));
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestSendSlipTransportLayer,
    ::testing::Values(
        TestSlipTransportLayerParams{"WhenSendingADatagramIfOkItShouldAddSlipEnd", 0, decoded_simple_msg, sizeof(decoded_simple_msg), encoded_simple_msg, sizeof(encoded_simple_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc", 0, decoded_escape_end_msg, sizeof(decoded_escape_end_msg), encoded_escape_end_msg, sizeof(encoded_escape_end_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd", 0, decoded_escape_esc_msg, sizeof(decoded_escape_esc_msg), encoded_escape_esc_msg, sizeof(encoded_escape_esc_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIfNextReturnErrorItShouldReturnError", -1, decoded_simple_msg, sizeof(decoded_simple_msg), encoded_simple_msg, sizeof(encoded_simple_msg)}),
    [](const testing::TestParamInfo<TestSlipTransportLayerParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestSendSlipTransportLayer, WhenSendingTwoDatagramIfOkItShouldAddSlipEnd)
{
    Sequence seq;
    char first_message[] = {0x01, 0x05, 0x06, 0x10};
    char second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50};
    char expected_first_message[] = {0x01, 0x05, 0x06, 0x10, (char)slip_application_layer::END};
    char expected_second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50, (char)slip_application_layer::END};

    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(expected_first_message))).With(Args<0, 1>(ElementsAreArray(expected_first_message, sizeof(expected_first_message)))).Times(1).InSequence(seq).WillOnce(Return(sizeof(expected_first_message)));
    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(expected_second_message))).With(Args<0, 1>(ElementsAreArray(expected_second_message, sizeof(expected_second_message)))).Times(1).InSequence(seq).WillOnce(Return(sizeof(expected_second_message)));

    ASSERT_EQ(sizeof(expected_first_message), layer->send(first_message, sizeof(first_message)));
    ASSERT_EQ(sizeof(expected_second_message), layer->send(second_message, sizeof(second_message)));
}

class TestRecvSlipTransportLayer : public testing::TestWithParam<TestSlipTransportLayerParams>
{
public:
    slip_application_layer *layer = nullptr;
    CommunicationsLayerMock *comms_layer_mock = nullptr;

    virtual void SetUp()
    {
        layer = new slip_application_layer();
        comms_layer_mock = new CommunicationsLayerMock();
        layer->set_next_layer(comms_layer_mock);
    }

    virtual void TearDown()
    {
        delete layer;
        delete comms_layer_mock;
    }
};

TEST_P(TestRecvSlipTransportLayer, TestNextRecvHandlerMessage)
{
    EXPECT_CALL(*comms_layer_mock, recv(_, GetParam().expected_msg_size)).With(Args<0, 1>(ElementsAreArray(GetParam().expected_msg, GetParam().expected_msg_size))).Times(1).WillOnce(Return(GetParam().expected_msg_size));
    ASSERT_EQ(GetParam().expected_msg_size, layer->recv(GetParam().msg, GetParam().msg_size));
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestRecvSlipTransportLayer,
    ::testing::Values(
        TestSlipTransportLayerParams{"WhenReceivingADatagramIfOkItShouldAddSlipEnd", 0, encoded_simple_msg, sizeof(encoded_simple_msg), decoded_simple_msg, sizeof(decoded_simple_msg)},
        TestSlipTransportLayerParams{"WhenReceivingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc", 0, encoded_escape_end_msg, sizeof(encoded_escape_end_msg), decoded_escape_end_msg, sizeof(decoded_escape_end_msg)},
        TestSlipTransportLayerParams{"WhenReceivingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd", 0, encoded_escape_esc_msg, sizeof(encoded_escape_esc_msg), decoded_escape_esc_msg, sizeof(decoded_escape_esc_msg)},
        TestSlipTransportLayerParams{"WhenReceivingADatagramIfNextReturnErrorItShouldReturnError", -1, encoded_simple_msg, sizeof(encoded_simple_msg), decoded_simple_msg, sizeof(decoded_simple_msg)}),
    [](const testing::TestParamInfo<TestSlipTransportLayerParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestRecvSlipTransportLayer, WhenReceivingTwoDatagramIfOkItShouldDecodeSlipEnd)
{
    Sequence seq;
    char decoded_first_message[] = {0x01, 0x05, 0x06, 0x10};
    char decoded_second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50};
    char encoded_first_message[] = {0x01, 0x05, 0x06, 0x10, (char)slip_application_layer::END};
    char encoded_second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50, (char)slip_application_layer::END};

    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(decoded_first_message))).With(Args<0, 1>(ElementsAreArray(decoded_first_message, sizeof(decoded_first_message)))).Times(1).InSequence(seq).WillOnce(Return(sizeof(decoded_first_message)));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(decoded_second_message))).With(Args<0, 1>(ElementsAreArray(decoded_second_message, sizeof(decoded_second_message)))).Times(1).InSequence(seq).WillOnce(Return(sizeof(decoded_second_message)));

    ASSERT_EQ(sizeof(decoded_first_message), layer->recv(encoded_first_message, sizeof(encoded_first_message)));
    ASSERT_EQ(sizeof(decoded_second_message), layer->recv(encoded_second_message, sizeof(encoded_second_message)));
}

TEST_F(TestRecvSlipTransportLayer, WhenReceivingIfMessageNoEndItShouldReturnError)
{
    Sequence seq;
    char message[] = {0x01, 0x05, 0x06, 0x10};
    ASSERT_EQ(-1, layer->recv(message, sizeof(message)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
