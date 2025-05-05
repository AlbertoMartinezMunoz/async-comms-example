#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class slip_application_layer : public communications_layer {
    public:
        static const uint8_t END;
        static const uint8_t ESC;
        static const uint8_t ESC_END;
        static const uint8_t ESC_ESC;

        slip_application_layer(){}

        ~slip_application_layer();

        int send_message(uint8_t *message, size_t size) override;

    private:
        uint8_t *message = nullptr;
};

#endif  // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
