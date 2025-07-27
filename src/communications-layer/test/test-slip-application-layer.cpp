#include <communications-layer/slip-application-layer.hpp>
#include <cstdlib>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using ::testing::_;
using ::testing::Args;
using ::testing::DoAll;
using ::testing::ElementsAreArray;
using ::testing::Return;
using ::testing::Sequence;
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

class TestSlipTransportLayerParams {
  public:
    TestSlipTransportLayerParams(std::string name, int next_handler_return, char *msg, size_t msg_size,
                                 char *expected_msg, size_t expected_msg_size)
        : name(name), next_handler_return(next_handler_return), msg(msg), msg_size(msg_size),
          expected_msg(expected_msg), expected_msg_size(expected_msg_size) {}

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
    int next_handler_return;
    char *msg;
    size_t msg_size;
    const char *expected_msg;
    size_t expected_msg_size;
};

char decoded_simple_msg[] = {'S', 'i', 'm', 'p', 'l', 'e', ' ', 'M', 'e', 's', 's', 'a', 'g', 'e', '\0'};
char encoded_simple_msg[] = {'S', 'i', 'm', 'p', 'l', 'e', ' ',  'M',
                             'e', 's', 's', 'a', 'g', 'e', '\0', (char)slip_application_layer::END};
char decoded_escape_end_msg[] = {'E', 's', 'c', 'a', 'p', 'e', 'd', ' ', (char)slip_application_layer::END,
                                 ' ', 'M', 'e', 's', 's', 'a', 'g', 'e', '\0'};
char encoded_escape_end_msg[] = {'E',
                                 's',
                                 'c',
                                 'a',
                                 'p',
                                 'e',
                                 'd',
                                 ' ',
                                 (char)slip_application_layer::ESC,
                                 (char)slip_application_layer::ESC_END,
                                 ' ',
                                 'M',
                                 'e',
                                 's',
                                 's',
                                 'a',
                                 'g',
                                 'e',
                                 '\0',
                                 (char)slip_application_layer::END};
char decoded_escape_esc_msg[] = {'E', 's', 'c', 'a', 'p', 'e', 'd', ' ', (char)slip_application_layer::ESC,
                                 ' ', 'M', 'e', 's', 's', 'a', 'g', 'e', '\0'};
char encoded_escape_esc_msg[] = {'E',
                                 's',
                                 'c',
                                 'a',
                                 'p',
                                 'e',
                                 'd',
                                 ' ',
                                 (char)slip_application_layer::ESC,
                                 (char)slip_application_layer::ESC_ESC,
                                 ' ',
                                 'M',
                                 'e',
                                 's',
                                 's',
                                 'a',
                                 'g',
                                 'e',
                                 '\0',
                                 (char)slip_application_layer::END};

class TestSendSlipTransportLayer : public testing::TestWithParam<TestSlipTransportLayerParams> {
  public:
    slip_application_layer *layer = nullptr;
    CommunicationsLayerMock *comms_layer_mock = nullptr;

    virtual void SetUp() {
        layer = new slip_application_layer();
        comms_layer_mock = new CommunicationsLayerMock();
        layer->set_next_communications_layer(comms_layer_mock);
    }

    virtual void TearDown() {
        delete layer;
        delete comms_layer_mock;
    }
};

TEST_P(TestSendSlipTransportLayer, TestNextSendHandlerMessage) {
    EXPECT_CALL(*comms_layer_mock, send(_, GetParam().expected_msg_size))
        .With(Args<0, 1>(ElementsAreArray(GetParam().expected_msg, GetParam().expected_msg_size)))
        .Times(1)
        .WillOnce(Return(GetParam().expected_msg_size));
    ASSERT_EQ(GetParam().expected_msg_size, layer->send(GetParam().msg, GetParam().msg_size));
}

INSTANTIATE_TEST_SUITE_P(
    , TestSendSlipTransportLayer,
    ::testing::Values(TestSlipTransportLayerParams{"WhenSendingADatagramIfOkItShouldAddSlipEnd", 0, decoded_simple_msg,
                                                   sizeof(decoded_simple_msg), encoded_simple_msg,
                                                   sizeof(encoded_simple_msg)},
                      TestSlipTransportLayerParams{"WhenSendingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc", 0,
                                                   decoded_escape_end_msg, sizeof(decoded_escape_end_msg),
                                                   encoded_escape_end_msg, sizeof(encoded_escape_end_msg)},
                      TestSlipTransportLayerParams{"WhenSendingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd", 0,
                                                   decoded_escape_esc_msg, sizeof(decoded_escape_esc_msg),
                                                   encoded_escape_esc_msg, sizeof(encoded_escape_esc_msg)},
                      TestSlipTransportLayerParams{"WhenSendingADatagramIfNextReturnErrorItShouldReturnError", -1,
                                                   decoded_simple_msg, sizeof(decoded_simple_msg), encoded_simple_msg,
                                                   sizeof(encoded_simple_msg)}),
    [](const testing::TestParamInfo<TestSlipTransportLayerParams> &info) { return info.param.name; });

TEST_F(TestSendSlipTransportLayer, WhenSendingTwoDatagramIfOkItShouldAddSlipEnd) {
    Sequence seq;

    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(encoded_escape_end_msg)))
        .With(Args<0, 1>(ElementsAreArray(encoded_escape_end_msg, sizeof(encoded_escape_end_msg))))
        .Times(1)
        .InSequence(seq)
        .WillOnce(Return(sizeof(encoded_escape_end_msg)));
    EXPECT_CALL(*comms_layer_mock, send(_, sizeof(encoded_simple_msg)))
        .With(Args<0, 1>(ElementsAreArray(encoded_simple_msg, sizeof(encoded_simple_msg))))
        .Times(1)
        .InSequence(seq)
        .WillOnce(Return(sizeof(encoded_simple_msg)));

    ASSERT_EQ(sizeof(encoded_escape_end_msg), layer->send(decoded_escape_end_msg, sizeof(decoded_escape_end_msg)));
    ASSERT_EQ(sizeof(encoded_simple_msg), layer->send(decoded_simple_msg, sizeof(decoded_simple_msg)));
}

class TestRecvSlipTransportLayer : public testing::TestWithParam<TestSlipTransportLayerParams> {
  public:
    slip_application_layer *layer = nullptr;
    CommunicationsLayerMock *comms_layer_mock = nullptr;

    virtual void SetUp() {
        layer = new slip_application_layer();
        comms_layer_mock = new CommunicationsLayerMock();
        layer->set_next_communications_layer(comms_layer_mock);
        memset(buffer, 0, sizeof(buffer));
    }

    virtual void TearDown() {
        delete layer;
        delete comms_layer_mock;
    }
    char buffer[32];
};

TEST_P(TestRecvSlipTransportLayer, TestNextRecvHandlerMessage) {
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(buffer)))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(GetParam().msg, GetParam().msg + GetParam().msg_size),
                        Return(GetParam().msg_size)));
    ASSERT_EQ(GetParam().expected_msg_size, layer->recv(buffer, sizeof(buffer)));
    std::vector<char> expected(GetParam().expected_msg, GetParam().expected_msg + GetParam().expected_msg_size);
    ASSERT_THAT(expected, ElementsAreArray(buffer, GetParam().expected_msg_size));
}

INSTANTIATE_TEST_SUITE_P(
    , TestRecvSlipTransportLayer,
    ::testing::Values(TestSlipTransportLayerParams{"WhenReceivingADatagramIfOkItShouldAddSlipEnd", 0,
                                                   encoded_simple_msg, sizeof(encoded_simple_msg), decoded_simple_msg,
                                                   sizeof(decoded_simple_msg)},
                      TestSlipTransportLayerParams{"WhenReceivingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc", 0,
                                                   encoded_escape_end_msg, sizeof(encoded_escape_end_msg),
                                                   decoded_escape_end_msg, sizeof(decoded_escape_end_msg)},
                      TestSlipTransportLayerParams{"WhenReceivingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd", 0,
                                                   encoded_escape_esc_msg, sizeof(encoded_escape_esc_msg),
                                                   decoded_escape_esc_msg, sizeof(decoded_escape_esc_msg)},
                      TestSlipTransportLayerParams{"WhenReceivingADatagramIfNextReturnErrorItShouldReturnError", -1,
                                                   encoded_simple_msg, sizeof(encoded_simple_msg), decoded_simple_msg,
                                                   sizeof(decoded_simple_msg)}),
    [](const testing::TestParamInfo<TestSlipTransportLayerParams> &info) { return info.param.name; });

TEST_F(TestRecvSlipTransportLayer, WhenReceivingTwoDatagramIfOkItShouldDecodeSlipEnd) {
    Sequence seq;

    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(buffer)))
        .Times(1)
        .InSequence(seq)
        .WillOnce(DoAll(SetArrayArgument<0>(encoded_simple_msg, encoded_simple_msg + sizeof(encoded_simple_msg)),
                        Return(sizeof(encoded_simple_msg))));
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(buffer)))
        .Times(1)
        .InSequence(seq)
        .WillOnce(
            DoAll(SetArrayArgument<0>(encoded_escape_esc_msg, encoded_escape_esc_msg + sizeof(encoded_escape_esc_msg)),
                  Return(sizeof(encoded_escape_esc_msg))));

    ASSERT_EQ(sizeof(decoded_simple_msg), layer->recv(buffer, sizeof(buffer)));
    std::vector<char> expected(decoded_simple_msg, decoded_simple_msg + sizeof(decoded_simple_msg));
    ASSERT_THAT(expected, ElementsAreArray(buffer, sizeof(decoded_simple_msg)));

    ASSERT_EQ(sizeof(decoded_escape_esc_msg), layer->recv(buffer, sizeof(buffer)));
    expected.assign(decoded_escape_esc_msg, decoded_escape_esc_msg + sizeof(decoded_escape_esc_msg));
    ASSERT_THAT(expected, ElementsAreArray(buffer, sizeof(decoded_escape_esc_msg)));
}

TEST_F(TestRecvSlipTransportLayer, WhenReceivingIfMessageNoEndItShouldReturnError) {
    Sequence seq;
    EXPECT_CALL(*comms_layer_mock, recv(_, sizeof(buffer)))
        .Times(1)
        .InSequence(seq)
        .WillOnce(DoAll(SetArrayArgument<0>(decoded_simple_msg, decoded_simple_msg + sizeof(decoded_simple_msg)),
                        Return(sizeof(decoded_simple_msg))));
    ASSERT_EQ(-1, layer->recv(buffer, sizeof(buffer)));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
