#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class slip_application_layer : public communications_layer
{
public:
    static const uint8_t END;
    static const uint8_t ESC;
    static const uint8_t ESC_END;
    static const uint8_t ESC_ESC;

    slip_application_layer() {}

    ~slip_application_layer();

    int send_message(const void *message, size_t size) override;

    int send_command(__attribute__((unused)) const void *command, __attribute__((unused)) size_t command_size,
                     __attribute__((unused)) void *response, __attribute__((unused)) size_t reponse_size) override
    {
        return 0;
    }

private:
    uint8_t *message = nullptr;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
