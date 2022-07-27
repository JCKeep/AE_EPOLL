#ifndef AE_EPOLL
#define AE_EPOLL



#include "config.h"

/*----------------------------- IO多路复用数据结构定义 ---------------------------------*/

struct ae_event;
struct ae_event_loop;

typedef void eventHandler(struct ae_event_loop *eventLoop, int fd);

typedef struct ae_event {
    int mask;
    unsigned char data[SIZE];
    eventHandler *readProc;
    eventHandler *writeProc;
} ae_event;

typedef struct ae_event_loop {
    int epfd;
    int size;
    int max_fd;
    int fired_max;
    ae_event *events;
    int      *fired;
    struct epoll_event *event;
} ae_event_loop;


/*--------------------------- API  -------------------------------*/

int openSerial(char *filename, unsigned long bps);
int aeAddEvent(ae_event_loop *event_loop, int fd, int mask);
int aePollEvent(ae_event_loop *event_loop);
ae_event_loop* aeCreateEventLoop();
void* aeWaitEvent(void *arg);
void aeFreeEventLoop(ae_event_loop *event_loop);



#endif