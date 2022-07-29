#ifndef EVENT_HANDLER
#define EVENT_HANDLER



#include "ae_epoll.h"

/*------------------------- 文件事件处理器 ---------------------------*/

void serialReadProc(ae_event_loop *eventLoop, int fd, void *data);
void cliReadProc(ae_event_loop *eventLoop, int connectfd, void *data);
void cliWriteProc(ae_event_loop *eventLoop, int connectfd, void *data);
void stringFinalize(struct ae_event_loop *eventLoop, void *data);



/*-------------------------- 时间时间处理器 --------------------------*/

void scheduledTest(ae_event_loop *eventLoop, long long id, void *data);
void scheduledFinalizeTest(ae_event_loop *eventLoop, void *data);


/*------------------------- 系统信号处理器 ---------------------------*/

void SIGINT_handler_server(int SIG);


#endif
