#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "config.h"
#include "ae_epoll.h"
#include "handler.h"


char cmd[128];
pthread_mutex_t lock;
ae_event_loop *eventLoop;


int main(int argc, char **argv)
{
    //int fd = openSerial("/dev/ttyUSB0", B9600);
    eventLoop = aeCreateEventLoop();
    assert(eventLoop != NULL);
    //aeAddEvent(eventLoop, fd, READ_EVENT);
    signal(SIGINT, SIGINT_handler);

    pthread_t pt;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&pt, NULL, aeWaitEvent, eventLoop);

    while (TRUE) {
        if (aePollEvent(eventLoop) > 0) {
            int max = eventLoop->fired_max;
            for (int i = 0; i < max; i++) {
                int fd = eventLoop->fired[i];
                ae_event *e = &eventLoop->events[fd];
                if (e->mask & READ_EVENT)
                    e->readProc(eventLoop, fd);
            }
            eventLoop->fired_max = 0;
        }
    }

    return 0;
}
