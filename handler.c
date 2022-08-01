#include "client.h"
#include "logger.h"
#include "server.h"
#include "handler.h"

extern char cmd[128];
extern ae_server *server;

/* 串口READ_EVENT通用文件事件处理器 */
void serialReadProc(ae_event_loop *eventLoop, int fd, void *data)
{
    u_char *buf = (u_char *)data;
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
            sprintf(cmd, "%s %s %d > output", CMD_HEADER, CMD_TEMP, buf[TEMP_MARK + 1]);
            //system(cmd);
            cmd[0] = '\0';
        }
        if (buf[1] & LIGHT_MARK) {
#ifdef DEBUG
            printf("同步光照ADC: %02x\n", buf[LIGHT_MARK + 1]);
#endif
            sprintf(cmd, "%s %s %d > output", CMD_HEADER, CMD_LIGHT, buf[LIGHT_MARK + 1]);
            //system(cmd);
            cmd[0] = '\0';
        }
        bzero(buf, SIZE);
    }
}



void serialWriteProc(ae_event_loop *eventLoop, int fd, void *data)
{
    write(fd, data, SIZE);
#ifdef DEBUG
    printf("ae_serial write\n");
#endif
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
    int type = VOID2INT(buf);
#ifdef DEBUG
    for (int i = 0; i < 0x10; i++) {
        printf("%02x ", (u_char)buf[i]);
    }
    printf("\n");
#endif
    if (type & ADD_SERIAL_CLI) {
        ae_serial *cli = (ae_serial *)(buf + 4);
        int fd = openSerial(cli->filename, B9600);
        if (fd < 0) {
            logger_error("File not found", cli->filename);
            write(connectfd, "ERRO", 4);
            return;
        }
        ae_serial *serial = aeCreateSerial(cli);
        serial->fd = fd;
        aeServerPushSerial(server, serial);
        u_char *rdata = (u_char *)malloc(sizeof(u_char) * SIZE);
        u_char *wdata = (u_char *)malloc(sizeof(u_char) * SIZE);
        memset(rdata, 0, SIZE);
        memset(wdata, 0, SIZE);
        aeAddFileEvent(eventLoop, serialReadProc, serialWriteProc, rdata, wdata, stringFinalize, fd, cli->mask, SERIAL_EVENT);
#ifdef DEBUG
        aeAddTimeEvent(eventLoop, 30000, scheduledTest, NULL, scheduledFinalizeTest);
#endif
        write(connectfd, &fd, sizeof(int));
    }
    else if (type & CMD_SERIAL_CLI) {
        int tmpfd = VOID2INT(buf + 4);
        ae_file_event *e = &eventLoop->events[tmpfd];
        eventLoop->post_event[eventLoop->post_process++] = tmpfd;
        e->mask |= WRITE_EVENT;
        for (int i = 0; i < SIZE; i++)
            VOID2UCHAR(e->wdata + i) = buf[8 + i];
    }
    else if (type & SERIAL_INFO) {
        ae_file_event *e = &eventLoop->events[connectfd];
        eventLoop->post_event[eventLoop->post_process++] = connectfd;
        e->mask |= WRITE_EVENT;
        VOID2INT(e->wdata) = server->client_size;
        VOID2INT(e->wdata + 4) = server->serial_size;
    }
    memset(buf, 0, SOCKET_SIZE);
}


/* 客户端添加事件的WRITE_EVENT处理器 */
void cliWriteProc(ae_event_loop *eventLoop, int connectfd, void *data)
{
    write(connectfd, data, SOCKET_SIZE);
}


void scheduledTest(ae_event_loop *eventLoop, long long id, void *data)
{
    printf("--------This is a scheduled task--------\n");
}


void scheduledFinalizeTest(ae_event_loop *eventLoop, void *data)
{
    aeAddTimeEvent(eventLoop, 30000, scheduledTest, data, scheduledFinalizeTest);
    // aeDeleteFileEvent(eventLoop, VOID2INT(data));
}


/* 字符串析构函数 */
void stringFinalize(struct ae_event_loop *eventLoop, void *data)
{
    free(data);
}


extern ae_server *server;



/* SIGINT终止释放内存 */ 
void SIGINT_handler_server(int SIG)
{
    printf("\n");
    ae_event_loop *eventLoop = server->eventLoop;
    ae_file_event *fe = eventLoop->events;
    for (int i = 0; i < 1024; i++)
        if (fe[i].fd > 0) aeDeleteFileEvent(eventLoop, fe[i].fd);

    aeFreeEventLoop(eventLoop);
    printf("\033[32mBye bye!\033[0m\n");
    sleep(1);
    kill(-getpid(), SIGINT);
    _exit(0);
}
