#ifndef AE_EPOLL
#define AE_EPOLL



#include "config.h"

/*----------------------------- IO多路复用数据结构定义 ---------------------------------*/

struct ae_file_event;
struct ae_time_event;
struct ae_event_loop;

/* 文件事件处理器接口 */
typedef void fileEventHandler(struct ae_event_loop *eventLoop, int fd, void *data);
typedef void fileEventFinalizeHandler(struct ae_event_loop *eventLoop, void *data);
typedef void timeEventHandler(struct ae_event_loop *eventLoop, long long id, void *data);
typedef void timeEventFinalizeHandler(struct ae_event_loop *eventLoop, void *data);

/* 文件事件 */
typedef struct ae_file_event {
    int mask; /* 事件掩码 */
    void *data;   /* 数据包 */
    fileEventHandler *readProc;     /* 读事件处理器 */
    fileEventHandler *writeProc;    /* 写事件处理器 */
    fileEventFinalizeHandler *finalizeProc;
} ae_file_event;

typedef struct ae_time_event {
    long long id;
    long when_sec;
    long when_ms;
    void *data;
    timeEventHandler *timeProc;
    timeEventFinalizeHandler *finalizeProc;
    struct ae_time_event *next;
} ae_time_event;

/* 文件事件控制器 */
typedef struct ae_event_loop {
    int epfd;   /* epoll文件描述符 */
    int size;   /* 已注册的事件大小 */
    int max_fd; /* 最大文件描述符 */
    int fired_max; /* 就绪队列大小 */
    long long next_time_id;
    ae_file_event *events; /* 事件缓冲区 */
    ae_time_event *time_event_head;
    int *fired;  /* 就绪队列 */
    struct epoll_event *event; /* 事件状态缓冲区 */
} ae_event_loop;


/*--------------------------- API  -------------------------------*/


int openSerial(char *filename, unsigned long bps);
int aeAddFileEvent(ae_event_loop *event_loop, fileEventHandler *readProc, 
        fileEventHandler *writeProc, void *data, 
        fileEventFinalizeHandler *finalizeProc, int fd, int mask);
int aeDeleteFileEvent(ae_event_loop *event_loop, int fd);
int aePollFileEvent(ae_event_loop *event_loop);
ae_event_loop* aeCreateEventLoop();
void* aeWaitConnection(void *arg);
void aeFreeEventLoop(ae_event_loop *event_loop);
void aeMain(ae_event_loop *event_loop);
void aeProcessFileEvent(ae_event_loop *event_loop);

void aeGetTime(long *seconds, long *milliseconds);
void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms);
long long aeAddTimeEvent(ae_event_loop *eventLoop, long long milliseconds,
        timeEventHandler *proc, void *data,
        timeEventFinalizeHandler *finalizerProc);
int aeDeleteTimeEvent(ae_event_loop *eventLoop, long long id);
int aeQuickDeleteTimeEvent(ae_event_loop *eventLoop, ae_time_event *time_event, ae_time_event *prev);
void aeProcessTimeEvent(ae_event_loop *event_loop);



#endif
