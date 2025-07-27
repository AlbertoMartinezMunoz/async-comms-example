#include <stdio.h>

#include <commands/commands-implementations.hpp>

local_fast_command::local_fast_command(local_command_receiver *cmd_rcv) : cmd_rcv(cmd_rcv) {}

int local_fast_command::execute() const {
    printf("********************** Local Fast Command "
           "**********************\r\n");
    cmd_rcv->send_fast_cmd();
    return 0;
};

local_slow_command::local_slow_command(local_command_receiver *cmd_rcv) : cmd_rcv(cmd_rcv) {}

int local_slow_command ::execute() const {
    char hello[] = "Hello World!!!";
    printf("********************** Local Slow Command "
           "**********************\r\n");
    cmd_rcv->send_slow_cmd(hello, sizeof(hello));
    return 0;
};

local_shutdown_command::local_shutdown_command(local_command_receiver *cmd_rcv, shutdown_receiver *comms,
                                               shutdown_receiver *cli)
    : cmd_rcv(cmd_rcv), comms(comms), cli(cli) {}

int local_shutdown_command ::execute() const {
    printf("********************** Local Shutdown Command "
           "**********************\r\n");
    if (cmd_rcv)
        cmd_rcv->send_shutdown_cmd();

    if (comms)
        comms->shutdown();

    if (cli)
        cli->shutdown();
    return 0;
};

int remote_fast_command::execute() const {
    printf("********************** Remote Fast Command "
           "**********************\r\n");
    return 0;
}

int remote_slow_command::execute() const {
    printf("********************** Remote Slow Command "
           "**********************\r\n");
    return 0;
}

remote_shutdown_command::remote_shutdown_command(shutdown_receiver *comms, shutdown_receiver *cli)
    : comms(comms), cli(cli) {}

int remote_shutdown_command ::execute() const {
    printf("********************** Remote Shutdown Command "
           "**********************\r\n");
    comms->shutdown();
    cli->shutdown();
    return 0;
}
