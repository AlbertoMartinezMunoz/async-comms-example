#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <communications-layer/slip-application-layer.hpp>
#include <cstdlib>
#include <cstring>
#include <vector>

using testing::ElementsAreArray;

class mock_application_layer : public communications_layer
{
public:
    uint8_t *message = nullptr;

    mock_application_layer() {}

    int send_message(const void *message, size_t size) override
    {
        this->message = (uint8_t *)realloc(this->message, size);
        if (this->message == nullptr)
        {
            perror("malloc");
            return -1;
        }
        memcpy(this->message, message, size);
        return send_message_return;
    }

    int send_command(__attribute__((unused)) const void *command, __attribute__((unused)) size_t command_size,
                     __attribute__((unused)) void *response, __attribute__((unused)) size_t reponse_size) override
    {
        return 0;
    }

    void set_send_message_return(int return_value)
    {
        send_message_return = return_value;
    }

    ~mock_application_layer()
    {
        free(message);
        message = nullptr;
    }

private:
    int send_message_return = 0;
};

class TestSlipTransportLayerParams
{
public:
    TestSlipTransportLayerParams(std::string name, int next_handler_return, uint8_t *msg, size_t msg_size, uint8_t *expected_msg, size_t expected_msg_size) : name(name), next_handler_return(next_handler_return), msg(msg), msg_size(msg_size), expected_msg(expected_msg), expected_msg_size(expected_msg_size) {}

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
    uint8_t *msg;
    size_t msg_size;
    uint8_t *expected_msg;
    size_t expected_msg_size;
};

uint8_t simple_msg[4] = {0x01, 0x05, 0x06, 0x10};
uint8_t expected_simple_msg[5] = {0x01, 0x05, 0x06, 0x10, slip_application_layer::END};
uint8_t escape_end_msg[4] = {0x01, 0x05, slip_application_layer::END, 0x10};
uint8_t expected_escape_end_msg[6] = {0x01, 0x05, slip_application_layer::ESC, slip_application_layer::ESC_END, 0x10, slip_application_layer::END};
uint8_t escape_esc_msg[4] = {0x01, 0x05, slip_application_layer::ESC, 0x10};
uint8_t expected_escape_esc_msg[6] = {0x01, 0x05, slip_application_layer::ESC, slip_application_layer::ESC_ESC, 0x10, slip_application_layer::END};

class TestSlipTransportLayer : public testing::TestWithParam<TestSlipTransportLayerParams>
{
public:
    slip_application_layer *layer = nullptr;
    mock_application_layer *mock_layer = nullptr;

    virtual void SetUp()
    {
        layer = new slip_application_layer();
        mock_layer = new mock_application_layer();
        mock_layer->set_send_message_return(0);
        layer->set_next_layer(mock_layer);
    }

    virtual void TearDown()
    {
        delete mock_layer;
        delete layer;
    }
};

TEST_P(TestSlipTransportLayer, TestNextSendHandlerMessage)
{
    std::vector<uint8_t> v;
    v.assign(GetParam().expected_msg, GetParam().expected_msg + GetParam().expected_msg_size);

    mock_layer->set_send_message_return(GetParam().next_handler_return);
    ASSERT_EQ(GetParam().next_handler_return, layer->send_message(GetParam().msg, GetParam().msg_size));
    ASSERT_THAT(v, ElementsAreArray(mock_layer->message, GetParam().expected_msg_size));
}

INSTANTIATE_TEST_SUITE_P(
    ,
    TestSlipTransportLayer,
    ::testing::Values(
        TestSlipTransportLayerParams{"WhenSendingADatagramIfOkItShouldAddSlipEnd", 0, simple_msg, sizeof(simple_msg), expected_simple_msg, sizeof(expected_simple_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc", 0, escape_end_msg, sizeof(escape_end_msg), expected_escape_end_msg, sizeof(expected_escape_end_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd", 0, escape_esc_msg, sizeof(escape_esc_msg), expected_escape_esc_msg, sizeof(expected_escape_esc_msg)},
        TestSlipTransportLayerParams{"WhenSendingADatagramIfNextReturnErrorItShouldReturnError", -1, simple_msg, sizeof(simple_msg), expected_simple_msg, sizeof(expected_simple_msg)}),
    [](const testing::TestParamInfo<TestSlipTransportLayerParams> &info)
    {
        return info.param.name;
    });

TEST_F(TestSlipTransportLayer, WhenSendingTwoDatagramIfOkItShouldAddSlipEnd)
{
    uint8_t first_message[] = {0x01, 0x05, 0x06, 0x10};
    uint8_t second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50};
    uint8_t expected_first_message[] = {0x01, 0x05, 0x06, 0x10, slip_application_layer::END};
    uint8_t expected_second_message[] = {0x01, 0x05, 0x06, 0x11, 0x50, slip_application_layer::END};

    ASSERT_EQ(0, layer->send_message(first_message, sizeof(first_message)));
    ASSERT_THAT(expected_first_message, ElementsAreArray(mock_layer->message, sizeof(expected_first_message)));
    ASSERT_EQ(0, layer->send_message(second_message, sizeof(second_message)));
    ASSERT_THAT(expected_second_message, ElementsAreArray(mock_layer->message, sizeof(expected_second_message)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
