#include "handler.h"
#include "client.h"

extern char cmd[128];

/* 串口READ_EVENT通用文件事件处理器 */
void serialReadProc(ae_event_loop *eventLoop, int fd)
{
    ae_event *e = &eventLoop->events[fd];
    read(fd, e->data, SIZE);
    if (e->data[0] == HEADER && e->data[SIZE - 1] == TAILER) {
#ifdef DEBUG
        for (int i = 0; i < SIZE; i++)
            printf("%02x ", e->data[i]);
        printf("\n");
#endif
        if (e->data[1] & TEMP_MARK) {
#ifdef DEBUG
            printf("同步温度ADC: %02x\n", e->data[TEMP_MARK + 1]);
#endif
            sprintf(cmd, "%s %s %d", CMD_HEADER, CMD_TEMP, e->data[TEMP_MARK + 1]);
            system(cmd);
            cmd[0] = '\0';
        }
        if (e->data[1] & LIGHT_MARK) {
#ifdef DEBUG
            printf("同步光照ADC: %02x\n", e->data[LIGHT_MARK + 1]);
#endif
            sprintf(cmd, "%s %s %d", CMD_HEADER, CMD_LIGHT, e->data[LIGHT_MARK + 1]);
            system(cmd);
            cmd[0] = '\0';
        }
        bzero(e->data, SIZE);
    }
}


/* Cliport缓冲区 */
static char buf[SOCKET_SIZE];


/* 客户端添加事件的READ_EVENT处理器 */
void cliReadProc(ae_event_loop *eventLoop, int connectfd)
{
    read(connectfd, buf, SOCKET_SIZE);
    if (strcmp(buf, "DISCONNECTED") == 0) {
        aeDeleteEvent(eventLoop, connectfd);
        return;
    }
    cliport *cli = (cliport *)buf;
    int fd = openSerial(cli->filename, B9600);
    if (fd < 0) {
        printf("File not existed\n");
        write(connectfd, "ERRO", 4);
        return;
    }
    aeAddEvent(eventLoop, serialReadProc, NULL, fd, cli->mask);
    memset(buf, 0, SOCKET_SIZE);
}


/* 客户端添加事件的WRITE_EVENT处理器 */
void cliWriteProc(ae_event_loop *eventLoop, int connectfd)
{
    write(connectfd, "PONG", 4);
}



extern ae_event_loop *eventLoop;



/* SIGINT终止释放内存 */ 
void SIGINT_handler(int SIG) 
{
    aeFreeEventLoop(eventLoop);
    printf("\n\033[32mBye bye!\033[0m\n");
    sleep(1);
    kill(-getpid(), SIGINT);
    _exit(0);
}
