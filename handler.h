#ifndef EVENT_HANDLER
#define EVENT_HANDLER



#include "ae_epoll.h"

/*------------------------- 文件事件处理器 ---------------------------*/

void readProc(ae_event_loop *eventLoop, int fd);


/*------------------------- 系统信号处理器 ---------------------------*/

void SIGINT_handler(int SIG);



#endif