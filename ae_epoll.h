#ifndef AE_EPOLL
#define AE_EPOLL



#include "config.h"

/*----------------------------- IO多路复用数据结构定义 ---------------------------------*/

struct ae_event;
struct ae_event_loop;

typedef void eventHandler(struct ae_event_loop *eventLoop, int fd);

typedef struct ae_event {
    int mask; /* 事件掩码 */
    unsigned char data[SIZE];   /* 数据包 */
    eventHandler *readProc;     /* 读事件处理器 */
    eventHandler *writeProc;    /* 写事件处理器 */
} ae_event;

typedef struct ae_event_loop {
    int epfd;   /* epoll文件描述符 */
    int size;   /* 已注册的事件大小 */
    int max_fd; /* 最大文件描述符 */
    int fired_max; /* 就绪队列大小 */
    ae_event *events; /* 事件缓冲区 */
    int      *fired;  /* 就绪队列 */
    struct epoll_event *event; /* 事件状态缓冲区 */
} ae_event_loop;


/*--------------------------- API  -------------------------------*/

int openSerial(char *filename, unsigned long bps);
int aeAddEvent(ae_event_loop *event_loop, int fd, int mask);
int aeDeleteEvent(ae_event_loop *event_loop, int fd);
int aePollEvent(ae_event_loop *event_loop);
ae_event_loop* aeCreateEventLoop();
void* aeWaitEvent(void *arg);
void aeFreeEventLoop(ae_event_loop *event_loop);
void aeMain(ae_event_loop *event_loop);
void aeProcessProc(ae_event_loop *event_loop);


#endif