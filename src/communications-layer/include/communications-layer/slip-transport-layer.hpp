#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_TRANSPORT_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_TRANSPORT_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class slip_transport_layer : public communications_layer {
    public:
        slip_transport_layer(){}

        int send_message(uint8_t *message) override;
};

#endif  // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_TRANSPORT_LAYER_H_
