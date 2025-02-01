
#include "net.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>

#define NCP_POLL_TIMEOUT_MS 1000

int net_init(struct net * ctx, uint16_t port, int listen_queue)
{
    ctx->sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(ctx->sock < 0){
        perror("ERROR: could not create socket");
        return 1;
    }

    if(setsockopt(ctx->sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("ERROR: could not make socket reusable");
        net_deinit(ctx);
        return 1;
    }

    memset(& ctx->addr, 0, sizeof(ctx->addr));

    ctx->addr.sin_family = AF_INET;
    ctx->addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    ctx->addr.sin_port = htons(port);

    if(bind(ctx->sock, (struct sockaddr *) & ctx->addr, sizeof(ctx->addr))){
        perror("ERROR: could not bind socket");
        net_deinit(ctx);
        return 1;
    }

    if(listen(ctx->sock, listen_queue)){ 
        perror("ERROR: could not listen on socket");
        return 1;
    }

    return 0;
}

void net_deinit(struct net * ctx)
{
    if(shutdown(ctx->sock, SHUT_RDWR)){
        perror("ERROR: could not shut down socket");
    }

    if(close(ctx->sock)){
        perror("ERROR: could not close socket");
    }
}

int net_client_pickup(struct net * ctx, int * sock)
{
    struct pollfd sock_poll = {
        .fd = ctx->sock,
        .events = POLLIN, // you can `|` together multiple events
    };

    {
        int ret = poll(& sock_poll, 1, NCP_POLL_TIMEOUT_MS);

        if(ret == 0){
            // timeout
            return 1;
        }else if(ret == 1){
            // event
        }else if(ret < 0){
            // error
            perror("ERROR: poll");
            return 1;
        }else{
            // unreachable
            fprintf(stderr, "ERROR: unreachable code reached\n");
            return 1;
        }
    }

    * sock = accept(ctx->sock, NULL, NULL);
    if(* sock < 0){
        perror("ERROR: could not accept client");
        return 1;
    }

    return 0;
}

void net_client_hangup(int sock)
{
    if(shutdown(sock, SHUT_RDWR)){
        fprintf(stderr, "ERROR: could not shut down client socket %d: %s\n", sock, strerror(errno));
    }

    if(close(sock)){
        fprintf(stderr, "ERROR: could not close client socket %d: %s\n", sock, strerror(errno));
    }
}
