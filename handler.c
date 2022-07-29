#include "handler.h"
#include "client.h"

extern char cmd[128];

/* 串口READ_EVENT通用文件事件处理器 */
void serialReadProc(ae_event_loop *eventLoop, int fd, void *data)
{
    unsigned char *buf = (unsigned char *)data;
    read(fd, buf, SIZE);
    if (buf[0] == HEADER && buf[SIZE - 1] == TAILER) {
#ifdef DEBUG
        for (int i = 0; i < SIZE; i++)
            printf("%02x ", buf[i]);
        printf("\n");
#endif
        if (buf[1] & TEMP_MARK) {
#ifdef DEBUG
            printf("同步温度ADC: %02x\n", buf[TEMP_MARK + 1]);
#endif
            sprintf(cmd, "%s %s %d", CMD_HEADER, CMD_TEMP, buf[TEMP_MARK + 1]);
            system(cmd);
            cmd[0] = '\0';
        }
        if (buf[1] & LIGHT_MARK) {
#ifdef DEBUG
            printf("同步光照ADC: %02x\n", buf[LIGHT_MARK + 1]);
#endif
            sprintf(cmd, "%s %s %d", CMD_HEADER, CMD_LIGHT, buf[LIGHT_MARK + 1]);
            system(cmd);
            cmd[0] = '\0';
        }
        bzero(buf, SIZE);
    }
}


/* 客户端添加事件的READ_EVENT处理器 */
void cliReadProc(ae_event_loop *eventLoop, int connectfd, void *data)
{
    char *buf = (char *)data;
    read(connectfd, buf, SOCKET_SIZE);
    if (strcmp(buf, "DISC") == 0) {
        aeDeleteFileEvent(eventLoop, connectfd);
        close(connectfd);
        return;
    }
    int type = *((int *)buf);
    if (type & SERIAL_CLI) {
        cliport *cli = (cliport *)(buf + 4);
        int fd = openSerial(cli->filename, B9600);
        if (fd < 0) {
            printf("File not existed\n");
            write(connectfd, "ERRO", 4);
            return;
        }
        unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * SIZE);
        memset(data, 0, SIZE);
        aeAddFileEvent(eventLoop, serialReadProc, NULL, data, stringFinalize, fd, cli->mask);
        aeAddTimeEvent(eventLoop, 1000, scheduledTest, NULL, scheduledFinalizeTest);
        //aeAddTimeEvent(eventLoop, 1000, scheduledTest, (void *)1, scheduledFinalizeTest);
    }
    else if (type & JAVA_CLI){
        printf("This is a java client\n");
        printf("Not a serial client\n");
    }
    memset(buf, 0, SOCKET_SIZE);
}


/* 客户端添加事件的WRITE_EVENT处理器 */
void cliWriteProc(ae_event_loop *eventLoop, int connectfd, void *data)
{
    write(connectfd, "PONG", 4);
}


void scheduledTest(ae_event_loop *eventLoop, long long id, void *data)
{
    printf("--------This is a scheduled task--------\n");
    if (data)
        printf("------------------data------------------\n");
}


void scheduledFinalizeTest(ae_event_loop *eventLoop, void *data)
{
    aeAddTimeEvent(eventLoop, 1000, scheduledTest, data, scheduledFinalizeTest);
}


/* 字符串析构函数 */
void stringFinalize(struct ae_event_loop *eventLoop, void *data)
{
    free(data);
}


extern ae_event_loop *eventLoop;



/* SIGINT终止释放内存 */ 
void SIGINT_handler_server(int SIG) 
{
    aeFreeEventLoop(eventLoop);
    printf("\n\033[32mBye bye!\033[0m\n");
    sleep(1);
    kill(-getpid(), SIGINT);
    _exit(0);
}
