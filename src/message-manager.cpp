#include <stdio.h>
#include <cstdlib>

#include <communications-layer/communications-layer.hpp>
#include <communications-layer/socket-transport-layer.hpp>
#include <communications-layer/slip-application-layer.hpp>

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

class message_manager
{
public:
    message_manager(communications_layer *application_layer, communications_layer *transport_layer)
        : application_layer(application_layer), transport_layer(transport_layer)
    {
        this->application_layer->set_next_layer(this->transport_layer);
    }

    int send_fast_command()
    {
        printf("command_manager: send_fast_command\r\n");
        this->application_layer->send_message((uint8_t *)"fast_command", sizeof(this->fast_command));
        return 0;
    }

    int send_slow_command()
    {
        printf("command_manager: send_slow_command\r\n");
        this->application_layer->send_message((uint8_t *)"slow_command", sizeof(this->slow_command));
        return 0;
    }

    int command_processer()
    {
        printf("command_manager: command_processer\r\n");
        return 0;
    }

private:
    communications_layer *application_layer;
    communications_layer *transport_layer;

    uint8_t fast_command[13] = "fast_command";
    uint8_t slow_command[13] = "slow_command";
};

int main()
{
    printf("-------------------- Message Manager v2025.05.01 --------------------\r\n");

    slip_application_layer *application_layer = new slip_application_layer();
    socket_transport_layer *transport_layer = new socket_transport_layer();
    if (transport_layer->connect_socket(SOCKET_NAME) == -1)
    {
        perror("connect");
        exit(1);
    }
    message_manager *msg_manager = new message_manager(application_layer, transport_layer);

    msg_manager->send_fast_command();

    delete msg_manager;
    delete application_layer;
    transport_layer->disconnect_socket();
    delete transport_layer;
    return 0;
}
