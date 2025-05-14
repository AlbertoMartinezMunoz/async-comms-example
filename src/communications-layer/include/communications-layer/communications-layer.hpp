#ifndef IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYERS_INCLUDE_COMMUNICATIONS_LAYERS_COMMUNICATION_LAYER_H_
#define IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYERS_INCLUDE_COMMUNICATIONS_LAYERS_COMMUNICATION_LAYER_H_
#include <cstdint>
#include <cstddef>
#include <cstdio>

/**
 * The communications_layer_interface interface declares a method for building the chain of handlers.
 * It also declares a method for executing a request.
 */
class communications_layer_interface
{
public:
    virtual communications_layer_interface *set_next_layer(communications_layer_interface *handler) = 0;
    virtual ssize_t send(const char *buffer, size_t buffer_size) = 0;
    virtual ssize_t recv(char *buffer, size_t buffer_size) = 0;
    virtual ~communications_layer_interface() {}
};

/**
 * The default chaining behavior can be implemented inside a base handler class.
 */
class communications_layer : public communications_layer_interface
{
    /**
     * @var communications_layer_interface
     */
private:
    communications_layer_interface *next_layer_;

public:
    communications_layer() : next_layer_(nullptr)
    {
    }
    communications_layer_interface *set_next_layer(communications_layer_interface *layer) override
    {
        this->next_layer_ = layer;
        return layer;
    }

    ssize_t send(const char *buffer, size_t size) override
    {
        if (this->next_layer_)
        {
            return this->next_layer_->send(buffer, size);
        }

        return {};
    }
};

#endif // IOT_MICRO_FIRMWARE_SRC_COMMUNICATIONS_LAYERS_INCLUDE_COMMUNICATIONS_LAYERS_COMMUNICATION_LAYER_H_
