#include <stdio.h>

class socket_application_layer {
    public:
        socket_application_layer(){}
};

class slip_transport_layer {
    public:
        slip_transport_layer(){}
};

class message_manager {
    public:
        message_manager(socket_application_layer *application_layer, slip_transport_layer *transport_layer)
        : application_layer(application_layer), transport_layer(transport_layer){
        }

        int send_fast_command(){
            printf("command_manager: send_fast_command\r\n");
            return 0;
        }

        int send_slow_command(){
            printf("command_manager: send_slow_command\r\n");
            return 0;
        }

        int command_processer() {
            printf("command_manager: command_processer\r\n");
            return 0;
        }

    private:
        socket_application_layer *application_layer;
        slip_transport_layer *transport_layer;
};

int main() {
    printf("-------------------- Message Manager v2025.05.01 --------------------\r\n");

    slip_transport_layer *transport_layer = new slip_transport_layer();
    socket_application_layer *application_layer = new socket_application_layer();
    message_manager *msg_manager = new message_manager(application_layer, transport_layer);

    msg_manager->send_fast_command();

    delete msg_manager;
    delete application_layer;
    delete transport_layer;
    return 0;
}