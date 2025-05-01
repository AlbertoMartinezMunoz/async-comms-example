#include <transport-manager/transport-manager.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <cstdlib>

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

TransportManager::TransportManager( ) {
    printf("\r\nCONSTRUCTOR\r\n");
}

int TransportManager::Connect(){
    // struct sockaddr_un socket_name;
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("\r\nHOLA\r\n");
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // /*
    //         * For portability clear the whole structure, since some
    //         * implementations have additional (nonstandard) fields in
    //         * the structure.
    //         */

    //         memset(&socket_namev, 0, sizeof(name));

    //         /* Bind socket to socket name. */
 
    //         socket_name.sun_family = AF_UNIX;
    //         strncpy(socket_name.sun_path, SOCKET_NAME, sizeof(socket_name.sun_path) - 1);
 
    //         ret = bind(connection_socket, (const struct sockaddr *) &socket_name, sizeof(socket_name));
    //         if (ret == -1) {
    //             perror("bind");
    //             exit(EXIT_FAILURE);
    //         }
 
    return 0;
}

    //    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    // struct sockaddr_un {
    //     sa_family_t AF_UNIX;               /* AF_UNIX */
    //     char        sun_path[108];            /* Pathname */
    // };
    // addrlen = sizeof(struct sockaddr_un);
