#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_COMMUNICATIONS_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_COMMUNICATIONS_LAYER_H_
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <commands/shutdown-receiver.hpp>

/**
 * The communications_layer_interface interface declares a method for building
 * the chain of handlers. It also declares a method for executing a request.
 */
class communications_layer_interface : public shutdown_receiver {
  public:
    virtual communications_layer_interface *set_next_communications_layer(communications_layer_interface *handler) = 0;
    virtual int connect() = 0;
    virtual int disconnect() = 0;
    virtual ssize_t send(const char *buffer, size_t buffer_size) = 0;
    virtual ssize_t recv(char *buffer, size_t buffer_size) = 0;
    virtual ~communications_layer_interface() {}
};

/**
 * The default chaining behavior can be implemented inside a base handler class.
 */
class communications_layer : public communications_layer_interface {
    /**
     * @var communications_layer_interface
     */
  private:
    communications_layer_interface *next_communications_layer_;

  public:
    communications_layer() : next_communications_layer_(nullptr) {}

    communications_layer_interface *set_next_communications_layer(communications_layer_interface *layer) override {
        this->next_communications_layer_ = layer;
        return layer;
    }

    ssize_t send(const char *buffer, size_t size) override {
        if (this->next_communications_layer_) {
            return this->next_communications_layer_->send(buffer, size);
        }

        return size;
    }

    ssize_t recv(char *buffer, size_t size) override {
        if (this->next_communications_layer_) {
            return this->next_communications_layer_->recv(buffer, size);
        }

        return size;
    }

    int connect() override {
        if (this->next_communications_layer_) {
            return this->next_communications_layer_->connect();
        }

        return 0;
    }

    int disconnect() override {
        if (this->next_communications_layer_) {
            return this->next_communications_layer_->disconnect();
        }

        return 0;
    }

    int shutdown() override {
        if (this->next_communications_layer_) {
            return this->next_communications_layer_->shutdown();
        }
        return 0;
    };
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYER_INCLUDE_COMMUNICATIONS_LAYER_COMMUNICATIONS_LAYER_H_
