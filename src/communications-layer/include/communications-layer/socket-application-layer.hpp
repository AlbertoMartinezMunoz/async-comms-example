#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_APPLICATION_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_APPLICATION_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class socket_application_layer : public communications_layer {
    public:
        socket_application_layer(){}

        int send_message(uint8_t *message) override;
};

#endif  // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SOCKET_APPLICATION_LAYER_H_
