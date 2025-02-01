
#include "client_handler_thread.h"

#include "net.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

#define CHT_POLL_TIMEOUT_MS -1 // negative for infinite timeout

struct cht_args
{
    int * shutting_down;
    struct buf * clients;
    int clients_update_eventfd;
};

static void * client_handler_thread(void * voidp_args);

int client_handler_thread_spawn(pthread_t * thr, int * shutting_down, struct buf * clients, int clients_update_eventfd)
{
    struct cht_args * args = malloc(sizeof(* args));
    if(!args){
        fprintf(stderr, "ERROR: could not allocate memory for thread arguments\n");
        return 1;
    }

    args->shutting_down = shutting_down;
    args->clients = clients;
    args->clients_update_eventfd = clients_update_eventfd;

    if(pthread_create(thr, NULL, client_handler_thread, args)){
        fprintf(stderr, "ERROR: could not spawn thread\n");
        free(args);
        return 1;
    }

    return 0;
}

void client_handler_thread_join(pthread_t thr)
{
    if(pthread_join(thr, NULL)){
        fprintf(stderr, "ERROR: could not join thread\n");
    }
}

static void * client_handler_thread(void * voidp_args)
{
    struct cht_args * args = voidp_args;

    struct buf polls;
    buf_init(& polls, sizeof(struct pollfd));

    const size_t BUF_START_IDX = 1;

    {
        // it's a good idea to keep this as the very first item
        // that means that when the update signal is sent we can
        // catch it right away and break out of the loop
        struct pollfd * pol_update = buf_append(& polls);
        pol_update->fd = args->clients_update_eventfd;
        pol_update->events = POLLIN;
    }

    while(!*args->shutting_down)
    {
        buf_clear(& polls, BUF_START_IDX);

        {
            buf_lock(args->clients);

            for(size_t cli_idx=0; cli_idx<args->clients->len; ++cli_idx)
            {
                struct pollfd * pol = buf_append(& polls);

                pol->fd = * (int *) buf_get(args->clients, cli_idx); // it IS valid to set fd to <0 (its just going to get ignored)
                pol->events = POLLIN;
            }

            buf_unlock(args->clients);
        }

        printf("client_handler_thread: waiting for event\n");

        int num_fds = poll(buf_get_all(& polls), polls.len, CHT_POLL_TIMEOUT_MS);

        if(num_fds == 0){
            // timeout
            printf("client_handler_thread: poll timeout\n");
            continue;
        }else if(num_fds < 0){
            // error
            perror("ERROR: client_handler_thread: poll");
            continue;
        }

        printf("client_handler_thread: num_fds %d\n", num_fds);

        for(size_t fd_idx=0; (fd_idx<polls.len) && (num_fds > 0); ++fd_idx){
            struct pollfd * pol = buf_get(& polls, fd_idx);

            if(pol->revents){

                int fd = pol->fd;

                if(fd == args->clients_update_eventfd){

                    printf("client_handler_thread: event on update fd (%d)\n", args->clients_update_eventfd);

                    {
                        uint64_t tmp;
                        ssize_t ret = read(fd, &tmp, sizeof(tmp));
                        if(ret < 0){
                            perror("ERROR: could not read from eventfd");
                        }
                        if(ret != sizeof(tmp)){
                            fprintf(stderr, "ERROR: could not fully read from eventfd\n");
                        }
                    }

                }else{

                    printf("client_handler_thread: event on fd %d\n", fd);

                    {
                        buf_lock(args->clients);
                        buf_remove(args->clients, & fd);
                        buf_unlock(args->clients);
                    }

                    net_client_hangup(fd);

                }

                // num_fds -= 1;
            }
        }
    }

    free(args);

    printf("client_handler_thread: done\n");

    return NULL;
}
