#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "config.h"
#include "ae_epoll.h"
#include "handler.h"

int loop;
char cmd[128];
pthread_mutex_t lock;
ae_event_loop *eventLoop;


int main(int argc, char **argv)
{
    eventLoop = aeCreateEventLoop();
    assert(eventLoop != NULL);
    signal(SIGINT, SIGINT_handler);

    pthread_t pt;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&pt, NULL, aeWaitEvent, eventLoop);

    aeMain(eventLoop);

    return 0;
}
