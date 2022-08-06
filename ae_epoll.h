#ifndef AE_EPOLL
#define AE_EPOLL



#include "config.h"
#include "logger.h"


struct ae_file_event;
struct ae_time_event;
struct ae_event_loop;

/*--------------------------------事件处理器接口 -------------------------------------*/

/* 文件事件处理器接口 */
typedef void fileEventHandler(struct ae_event_loop *eventLoop, int fd, void *data);
/* 文件事件析构接口 */
typedef void fileEventFinalizeHandler(struct ae_event_loop *eventLoop, void *data);
/* 时间事件处理器接口 */
typedef void timeEventHandler(struct ae_event_loop *eventLoop, long long id, void *data);
/* 时间事件析构接口 */
typedef void timeEventFinalizeHandler(struct ae_event_loop *eventLoop, void *data);



/*----------------------------- IO多路复用数据结构定义 ---------------------------------*/

/* 文件事件 */
typedef struct ae_file_event {
    int fd; /* 文件事件描述符 */
    int mask; /* 事件掩码 */
    int type; /*  */
    void *rdata;   /* 数据包 */
    void *wdata;
    fileEventHandler *readProc;     /* 读事件处理器 */
    fileEventHandler *writeProc;    /* 写事件处理器 */
    fileEventFinalizeHandler *finalizeProc; /* data析构函数 */
} ae_file_event;


/* 时间事件UP */
typedef struct ae_time_event {
    long long id; /* 时间事件唯一ID */
    long when_sec; /* 到达时间second */
    long when_ms; /* 到达时间millisecond */
    void *data; /* 时间事件数据包 */
    timeEventHandler *timeProc; /* 时间事件处理器 */
    timeEventFinalizeHandler *finalizeProc; /* data析构函数 */
    struct ae_time_event *next; /* 下一节点 */
} ae_time_event;


/* 文件事件控制器 */
typedef struct ae_event_loop {
    int epfd;   /* epoll文件描述符 */
    int size;   /* 已注册的事件大小 */
    int max_fd; /* 最大文件描述符 */
    int fired_max; /* 就绪队列大小 */
    int post_process; /* 是否有后续待处理事件 */
    long long next_time_id; /* 时间事件下一个唯一ID */
    ae_file_event *events; /* 事件缓冲区 */
    ae_time_event *time_event_head; /* 时间事件链表头节点 */
    int *fired;  /* 就绪队列 */
    int *post_event; /* 后就绪队列 */ 
    struct epoll_event *event; /* 事件状态缓冲区 */
} ae_event_loop;


/*--------------------------- API  -------------------------------*/

int openSerial(char *filename, unsigned long bps);
int aeAddFileEvent(ae_event_loop *event_loop, fileEventHandler *readProc, 
        fileEventHandler *writeProc, void *rdata, void *wdata,
        fileEventFinalizeHandler *finalizeProc, int fd, int mask, int type);
int aeDeleteFileEvent(ae_event_loop *event_loop, int fd);
int aePollFileEvent(ae_event_loop *event_loop);
ae_event_loop* aeCreateEventLoop();
// void* aeWaitConnection(void *arg);
void aeFreeEventLoop(ae_event_loop *event_loop);
void aeMain(ae_event_loop *event_loop);
void aeProcessFileEvent(ae_event_loop *event_loop);
void aeProcessPostEvent(ae_event_loop *event_loop);

void aeGetTime(long *seconds, long *milliseconds);
void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms);
long long aeAddTimeEvent(ae_event_loop *eventLoop, long long milliseconds,
        timeEventHandler *proc, void *data,
        timeEventFinalizeHandler *finalizerProc);
int aeDeleteTimeEvent(ae_event_loop *eventLoop, long long id);
int aeQuickDeleteTimeEvent(ae_event_loop *eventLoop, ae_time_event *time_event, ae_time_event *prev);
void aeProcessTimeEvent(ae_event_loop *event_loop);



#endif
