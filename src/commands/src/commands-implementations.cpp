#include <cstring>
#include <stdio.h>

#include <commands/commands-implementations.hpp>

local_fast_command::local_fast_command(communications_layer *comms) : comms(comms) {}

int local_fast_command::execute() const {
    char response[32];

    printf("********************** Local Fast Command **********************\r\n");

    int ret = comms->connect();
    if (ret == -1) {
        perror("local_fast_command::execute: connect");
        return -2;
    }

    ret = comms->send(command::remote_fast_cmd_id, sizeof(command::remote_fast_cmd_id));
    if (ret < (int)sizeof(command::remote_fast_cmd_id)) {
        perror("local_fast_command::execute: send");
        return -1;
    }

    printf("local_fast_command::execute: wait ack\r\n");
    response[0] = '\0';
    ret = comms->recv(response, sizeof(response));
    comms->disconnect();
    printf("local_fast_command::execute: response received[%d]\r\n", ret);
    if (ret < 0) {
        perror("local_fast_command::execute: recv");
        return ret;
    } else if (strncmp(command::nack, response, strlen(command::nack)) == 0) {
        printf("local_fast_command::execute: '%s' received\r\n", response);
        return 1;
    } else if (strncmp(command::ack, response, strlen(command::ack)) == 0) {
        printf("local_fast_command::execute: '%s' received\r\n", response);
        return 0;
    } else
        return -1;
};

local_slow_command::local_slow_command(communications_layer *comms) : comms(comms) {}

int local_slow_command ::execute() const {
    char response[32];

    printf("********************** Local Slow Command **********************\r\n");

    int ret = comms->connect();
    if (ret == -1) {
        perror("local_slow_command::execute: connect");
        return -2;
    }

    ret = comms->send(command::remote_slow_cmd_id, sizeof(command::remote_slow_cmd_id));
    if (ret < (int)sizeof(command::remote_slow_cmd_id)) {
        perror("local_slow_command::execute: send");
        return -1;
    }

    printf("local_slow_command::execute: wait ack\r\n");
    response[0] = '\0';
    ret = comms->recv(response, sizeof(response));
    comms->disconnect();
    printf("local_slow_command::execute: response received[%d]\r\n", ret);
    if (ret < 0) {
        perror("local_slow_command::execute: recv");
        return ret;
    } else if (strncmp(command::nack, response, strlen(command::nack)) == 0) {
        printf("local_slow_command::execute: '%s' received\r\n", response);
        return 1;
    } else if (strncmp(command::ack, response, strlen(command::ack)) == 0) {
        printf("local_slow_command::execute: '%s' received\r\n", response);
        return 0;
    } else
        return -1;
};

local_shutdown_command::local_shutdown_command(communications_layer *comms, shutdown_receiver *cli)
    : comms(comms), cli(cli) {}

int local_shutdown_command ::execute() const {
    char response[32];

    printf("********************** Local Shutdown Command **********************\r\n");

    int ret = comms->connect();
    if (ret == -1)
        perror("local_shutdown_command::execute: connect");

    ret = comms->send(command::remote_shutdown_cmd_id, sizeof(command::remote_shutdown_cmd_id));
    if (ret < (int)sizeof(command::remote_shutdown_cmd_id))
        perror("local_shutdown_command::execute: send");

    printf("local_shutdown_command::execute: wait ack\r\n");
    response[0] = '\0';
    ret = comms->recv(response, sizeof(response));
    comms->disconnect();
    printf("local_shutdown_command::execute: response received[%d]\r\n", ret);
    if (ret < 0)
        perror("local_shutdown_command::execute: recv");
    else if (strncmp(command::nack, response, strlen(command::nack)) == 0)
        printf("local_shutdown_command::execute: '%s' received\r\n", response);
    else if (strncmp(command::ack, response, strlen(command::ack)) == 0)
        printf("local_shutdown_command::execute: '%s' received\r\n", response);

    comms->shutdown();

    if (cli)
        cli->shutdown();
    return 0;
};

int remote_fast_command::execute() const {
    printf("********************** Remote Fast Command **********************\r\n");
    return 0;
}

int remote_slow_command::execute() const {
    printf("********************** Remote Slow Command **********************\r\n");
    return 0;
}

remote_shutdown_command::remote_shutdown_command(shutdown_receiver *comms, shutdown_receiver *cli)
    : comms(comms), cli(cli) {}

int remote_shutdown_command ::execute() const {
    printf("********************** Remote Shutdown Command **********************\r\n");
    comms->shutdown();
    cli->shutdown();
    return 0;
}
