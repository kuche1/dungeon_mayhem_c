
#include "accept_connections_loop.h"

#include "client.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void accept_connections_loop(int * shutting_down, struct net * net, struct buf * clients, int clients_update_eventfd)
{
    while(!*shutting_down)
    {
        int sock;
        if(net_client_pickup(net, & sock)){
            continue;
        }

        printf("accept_connections_loop: client pick up (%d)\n", sock);

        struct client * client;
        client_init(sock, clients, & client);

        // TODO make into compiletime assert
        if(sizeof(client) != sizeof(uint64_t)){
            fprintf(stderr, "ERROR: accept_connections_loop: sizeof(client) != sizeof(uint64_t)\n");
            exit(1);
        }

        ssize_t written = write(clients_update_eventfd, & client, sizeof(client));
        if(written < 0){
            perror("ERROR: accept_connections_loop: write to eventfd failed");
            exit(1);
        }
        if(written != sizeof(client)){
            fprintf(stderr, "ERROR: accept_connections_loop: could not write full data to eventfd\n");
            exit(1);
        }

        printf("accept_connections_loop: buf len %ld\n", clients->len);
    }
}
