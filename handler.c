#include "handler.h"

extern char cmd[128];

void readProc(ae_event_loop *eventLoop, int fd)
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


extern ae_event_loop *eventLoop;

void SIGINT_handler(int SIG) 
{
    aeFreeEventLoop(eventLoop);
    printf("\nBye bye!\n");
    sleep(1);
    _exit(0);
}