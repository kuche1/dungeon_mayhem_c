
#include "client.h"

#include "net.h"

#include <stdio.h>

void client_init(int sock, struct buf * clients)
{
    buf_lock(clients);

    struct client * client = buf_append(clients);

    client->sock = sock;

    buf_unlock(clients);
}

void client_deinit(struct client * client, struct buf * clients)
{
    buf_lock(clients);
    net_client_hangup(client->sock);
    buf_remove(clients, client);
    buf_unlock(clients);
    // TODO
    // za da izbegna tezi laina s postoqnnoto lock-vane kogato mi trqbva da napravq neshto elementarno
    // trqbva da napravq dummy pointer taka che kogato neshto se realloc-va to da si razmenq kude stoi
    // pointer-a v pametta, no ako veche sum stignal do tozi pointer moga da pravq kakvoto si iskam s nego
}

void client_serve(int sock, struct buf * clients)
{
    printf("client_serve: sock %d\n", sock);

    struct client * client = NULL;
    {
        buf_lock(clients);

        for(size_t cli_idx=0; cli_idx<clients->len; ++cli_idx){
            struct client * cli = buf_get(clients, cli_idx);
            printf("client_serve: looking for %d found %d\n", sock, cli->sock);
            if(sock == cli->sock){
                client = cli;
                break;
            }
        }

        buf_unlock(clients);
    }

    if(!client){
        fprintf(stderr, "ERROR: client_serve: could not find client\n");
        return;
    }

    char msg[20] = {0};
    recv(sock, msg, sizeof(msg) - 1, 0); // TODO error check i kurvi
    printf("client_serve: from %d: `%s`\n", sock, msg);

    client_deinit(client, clients);
}
