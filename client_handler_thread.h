#ifndef CLIENT_HANDLER_THREAD
#define CLIENT_HANDLER_THREAD

#include <pthread.h>

struct cht_args
{
    int * shutting_down;
    struct buf * clients;
};

int client_handler_thread_spawn(pthread_t * thr, int * shutting_down, struct buf * clients);
void client_handler_thread_join(pthread_t * thr);

#endif