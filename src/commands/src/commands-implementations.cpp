#include <stdio.h>

#include <commands/commands-implementations.hpp>

int local_fast_command::execute() const {
  printf("********************** Local Fast Command "
         "**********************\r\n");
  return 0;
};

int local_slow_command ::execute() const {
  printf("********************** Local Slow Command "
         "**********************\r\n");
  return 0;
};

local_shutdown_command::local_shutdown_command(shutdown_receiver *comms,
                                               shutdown_receiver *cli)
    : comms(comms), cli(cli) {}

int local_shutdown_command ::execute() const {
  printf("********************** Local Shutdown Command "
         "**********************\r\n");
  comms->shutdown();
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

remote_shutdown_command::remote_shutdown_command(shutdown_receiver *comms,
                                                 shutdown_receiver *cli)
    : comms(comms), cli(cli) {}

int remote_shutdown_command ::execute() const {
  printf("********************** Remote Shutdown Command "
         "**********************\r\n");
  comms->shutdown();
  cli->shutdown();
  return 0;
}
