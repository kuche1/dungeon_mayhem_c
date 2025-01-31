
#include "net.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>

[[nodiscard]] int net_init(struct net * ctx, uint16_t port, int listen_queue)
{
    ctx->sock = socket(AF_INET, SOCK_STREAM, 0);
    if(ctx->sock < 0){
        perror("could not create socket");
        return 1;
    }

    if(setsockopt(ctx->sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("could not make socket reusable");
        net_deinit(ctx);
        return 1;
    }

    memset(& ctx->addr, 0, sizeof(ctx->addr));

    ctx->addr.sin_family = AF_INET;
    ctx->addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    ctx->addr.sin_port = htons(port);

    if(bind(ctx->sock, (struct sockaddr *) & ctx->addr, sizeof(ctx->addr))){
        perror("could not bind socket");
        net_deinit(ctx);
        return 1;
    }

    if(listen(ctx->sock, listen_queue)){ 
        perror("could not listen on socket");
        return 1;
    }

    return 0;
}

void net_deinit(struct net * ctx)
{
    if(shutdown(ctx->sock, SHUT_RDWR)){
        perror("could not shut down socket");
    }

    if(close(ctx->sock)){
        perror("could not close socket");
    }
}

[[nodiscard]] int net_client_pickup(struct net * ctx, int * sock)
{
    * sock = accept(ctx->sock, NULL, NULL);
    if(* sock < 0){
        perror("could not accept client");
        return 1;
    }

    return 0;
}

void net_client_hangup(int sock)
{
    if(shutdown(sock, SHUT_RDWR)){
        perror("could not shut down client socket");
    }

    if(close(sock)){
        perror("could not close client socket");
    }
}
