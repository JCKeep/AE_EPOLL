#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "server.h"
#include "handler.h"
#include "ae_epoll.h"

/* 全局命令缓冲区 */
char cmd[128];
/* 全局文件事件互斥锁 */
pthread_mutex_t lock;
/* 全局文件事件控制器 */
ae_server *server;


int main(int argc, char **argv)
{
    server = aeCreateServer();
    signal(SIGINT, SIGINT_handler_server);

    pthread_t pt;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&pt, NULL, aeWaitConnection, server->eventLoop);

    aeMain(server->eventLoop);

    return 0;
}
