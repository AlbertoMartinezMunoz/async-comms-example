#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_

#include <communications-layer/communications-layer.hpp>

class slip_application_layer : public communications_layer {
public:
  static const uint8_t END;
  static const uint8_t ESC;
  static const uint8_t ESC_END;
  static const uint8_t ESC_ESC;

  ssize_t send(const char *buffer, size_t buffer_size) override;

  ssize_t recv(char *buffer, size_t buffer_size) override;
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_SLIP_APPLICATION_LAYER_H_
