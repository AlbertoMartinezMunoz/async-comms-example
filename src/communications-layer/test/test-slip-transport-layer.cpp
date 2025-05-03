#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <communications-layer/slip-transport-layer.hpp>
#include <cstdlib>
#include <cstring>

using testing::ElementsAreArray;

class mock_application_layer : public communications_layer {
    public:
        uint8_t *message = nullptr;

        mock_application_layer(){}

        int send_message(uint8_t *message, size_t size) override {
            this->message = (uint8_t *)realloc(this->message, size);
            if(this->message == nullptr)
            {
                perror("malloc");
                return -1;
            }
            memcpy(this->message, message, size);
            return send_message_return;
        }

        void set_send_message_return(int return_value) {
            send_message_return = return_value;
        }

        ~mock_application_layer() {
            free(message);
            message = nullptr;
        }
    
    private:
        int send_message_return = 0;
};

class TestSlipTransportLayer: public ::testing::Test {
public:
    slip_transport_layer *layer = nullptr;
    mock_application_layer *mock_layer = nullptr;

	virtual void SetUp() {
		layer = new slip_transport_layer();
        mock_layer = new mock_application_layer();
        mock_layer->set_send_message_return(0);
        layer->set_next_layer(mock_layer);
	}

	virtual void TearDown() {
        delete mock_layer;
		delete layer;
	}
};

TEST_F(TestSlipTransportLayer, WhenSendingADatagramIfOkItShouldAddSlipEnd) {
	uint8_t message[] = {0x01, 0x05, 0x06, 0x10};
    uint8_t expected_message[] = {0x01, 0x05, 0x06, 0x10, slip_transport_layer::END};

    ASSERT_EQ(0, layer->send_message(message, sizeof(message)));
    ASSERT_THAT(expected_message, ElementsAreArray(mock_layer->message,sizeof(expected_message)));
}

TEST_F(TestSlipTransportLayer, WhenSendingTwoDatagramIfOkItShouldAddSlipEnd) {
	uint8_t first_message[] = {0x01, 0x05, 0x06, 0x10};
    uint8_t second_message[] = {0x01, 0x05, 0x06, 0x10, 0x50};
    uint8_t expected_first_message[] = {0x01, 0x05, 0x06, 0x10, slip_transport_layer::END};
    uint8_t expected_second_message[] = {0x01, 0x05, 0x06, 0x10, 0x50, slip_transport_layer::END};
    
    ASSERT_EQ(0, layer->send_message(first_message, sizeof(first_message)));
    ASSERT_THAT(expected_first_message, ElementsAreArray(mock_layer->message,sizeof(expected_first_message)));
    ASSERT_EQ(0, layer->send_message(second_message, sizeof(second_message)));
    ASSERT_THAT(expected_second_message, ElementsAreArray(mock_layer->message,sizeof(expected_second_message)));
}

TEST_F(TestSlipTransportLayer, WhenSendingADatagramIf0xc0PresentItShouldChangeTo0xdb0xdc) {
	uint8_t message[] = {0x01, 0x05, slip_transport_layer::END, 0x10};
    uint8_t expected_message[] = {0x01, 0x05, slip_transport_layer::ESC, slip_transport_layer::ESC_END, 0x10, slip_transport_layer::END};

    ASSERT_EQ(0, layer->send_message(message, sizeof(message)));
    ASSERT_THAT(expected_message, ElementsAreArray(mock_layer->message,sizeof(expected_message)));
}

TEST_F(TestSlipTransportLayer, WhenSendingADatagramIf0xdbPresentItShouldChangeTo0xdb0xdd) {
	uint8_t message[] = {0x01, 0x05, slip_transport_layer::ESC, 0x10};
    uint8_t expected_message[] = {0x01, 0x05, slip_transport_layer::ESC, slip_transport_layer::ESC_ESC, 0x10, slip_transport_layer::END};

    ASSERT_EQ(0, layer->send_message(message, sizeof(message)));
    ASSERT_THAT(expected_message, ElementsAreArray(mock_layer->message,sizeof(expected_message)));
}

TEST_F(TestSlipTransportLayer, WhenSendingADatagramIfNextReturnErrorItShouldReturnError) {
	uint8_t message[] = {0x01, 0x05, slip_transport_layer::ESC, 0x10};
    uint8_t expected_message[] = {0x01, 0x05, slip_transport_layer::ESC, slip_transport_layer::ESC_ESC, 0x10, slip_transport_layer::END};
    int expected_return = -1;

    mock_layer->set_send_message_return(expected_return);
    ASSERT_EQ(expected_return, layer->send_message(message, sizeof(message)));
    ASSERT_THAT(expected_message, ElementsAreArray(mock_layer->message,sizeof(expected_message)));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
