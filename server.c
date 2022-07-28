#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ae_epoll.h"
#include "handler.h"

/* 全局命令缓冲区 */
char cmd[128];
/* 全局文件事件互斥锁，事件处理仍为单线程运行 */
pthread_mutex_t lock;
/* 全局文件事件控制器 */
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
