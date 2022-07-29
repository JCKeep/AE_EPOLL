#include "client.h"
#include "handler.h"
#include "ae_epoll.h"

extern pthread_mutex_t lock;

/*--------------------------- IO多路复用API实现 -----------------------------*/

/* 打开串口 */
int openSerial(char *filename, unsigned long bps)
{
    struct termios opt;
    int fd = open(filename, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0) {
        return -1;
    }

    tcgetattr(fd, &opt);
    cfsetispeed(&opt, bps);
    cfsetospeed(&opt, bps);
 
    opt.c_lflag   &=   ~(ECHO | ICANON | IEXTEN | ISIG);
    opt.c_iflag   &=   ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE | PARENB);
    opt.c_cflag   |=   CS8;
 
    opt.c_cc[VMIN]   =   SIZE;                                      
    opt.c_cc[VTIME]  =   150;
 
    if (tcsetattr(fd, TCSANOW, &opt)<0) {
        return -1;
    }
 
    return fd;
}


/* 添加文件事件 */
int aeAddFileEvent(ae_event_loop *event_loop, fileEventHandler *readProc, 
        fileEventHandler *writeProc, void *data, 
        fileEventFinalizeHandler *finalizeProc, int fd, int mask)
{
    pthread_mutex_lock(&lock);
    int mod = (event_loop->events[fd].readProc != NULL || event_loop->events[fd].writeProc != NULL) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    ae_file_event *e = &event_loop->events[fd];
    e->mask = 0;
    e->data = data;
    e->readProc = NULL;
    e->writeProc = NULL;
    e->finalizeProc = finalizeProc;
    struct epoll_event event;
    event.data.u64 = 0;
    event.events = 0;
    event.data.fd = fd;
    if (mask & READ_EVENT) {
        event.events |= (EPOLLIN | EPOLLET); /* 检测输入，边缘触发 */
        e->readProc = readProc;
    }
    if (mask & WRITE_EVENT) {
        event.events |= (EPOLLOUT | EPOLLET);
        e->writeProc = writeProc;
    }
    if (epoll_ctl(event_loop->epfd, mod, fd, &event) < 0) {
        perror("epoll_ctl add");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    event_loop->size++;
    event_loop->max_fd = (event_loop->max_fd > fd) ? event_loop->epfd : fd;
    printf("\033[32madd event(%d) ok\033[0m\n", fd);
    pthread_mutex_unlock(&lock);
    return 0;
}


/* 删除指定文件事件 */
int aeDeleteFileEvent(ae_event_loop *event_loop, int fd)
{
    pthread_mutex_lock(&lock);
    ae_file_event *e = &event_loop->events[fd];
    struct epoll_event event;
    event.data.u64 = 0;
    event.events = 0;
    event.data.fd = fd;
    if (e->readProc != NULL)
        event.events |= (EPOLLIN | EPOLLET);
    if (e->writeProc != NULL)
        event.events |= (EPOLLOUT | EPOLLET);
    if (e->finalizeProc)
        e->finalizeProc(event_loop, e->data);
    memset(e, 0, sizeof(ae_file_event));
    if (epoll_ctl(event_loop->epfd, EPOLL_CTL_DEL, fd, &event) < 0) {
        perror("epoll_ctl del");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    event_loop->size--;
    printf("\033[32mdelete event(%d) ok\033[0m\n", fd);
    pthread_mutex_unlock(&lock);
    return 0;
}


/* 阻塞拉取事件 */
int aePollFileEvent(ae_event_loop *event_loop)
{
    int i, write_num = 0;
    write_num = epoll_wait(event_loop->epfd, event_loop->event, 1024, MIN_BLOCK_TIME);
    for (i = 0; i < write_num; i++) {
        struct epoll_event *event = &event_loop->event[i];
        int fd = event->data.fd;
        ae_file_event *e = &event_loop->events[fd];
        if (event->events & EPOLLIN)
            e->mask |= READ_EVENT;
        if (event->events & EPOLLOUT)
            e->mask |= WRITE_EVENT;
        event_loop->fired[event_loop->fired_max++] = fd;
    }
    return write_num;
}


/* 创建新的ae_event_loop */
ae_event_loop* aeCreateEventLoop()
{
    ae_event_loop *event_loop = (ae_event_loop *)malloc(sizeof(ae_event_loop));
    if (!event_loop) {
        perror("malloc event_loop");
        return NULL;
    }

    event_loop->next_time_id = 0;
    event_loop->time_event_head = NULL;
    event_loop->epfd = epoll_create(1024);
    event_loop->fired = (int *)malloc(1024 * sizeof(int));
    event_loop->fired_max = 0;
    event_loop->max_fd = 0;
    event_loop->size = 0;
    event_loop->event = (struct epoll_event *)malloc(1024 * sizeof(struct epoll_event));
    event_loop->events = (ae_file_event *)malloc(1024 * sizeof(ae_file_event));
    memset(event_loop->event, 0, 1024 * sizeof(struct epoll_event));
    memset(event_loop->events, 0, 1024 * sizeof(ae_file_event));
    memset(event_loop->fired, 0, 1024 * sizeof(int));
    return event_loop;
}



void aeGetTime(long *seconds, long *milliseconds)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec / 1000;
}



void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms)
{
    long cur_sec, cur_ms, when_sec, when_ms;

    // 获取当前时间
    aeGetTime(&cur_sec, &cur_ms);

    // 计算增加 milliseconds 之后的秒数和毫秒数
    when_sec = cur_sec + milliseconds / 1000;
    when_ms = cur_ms + milliseconds % 1000;

    while (when_ms > 1000) {
        when_sec++;
        when_ms -= 1000;
    }

    *sec = when_sec;
    *ms = when_ms;
}



long long aeAddTimeEvent(ae_event_loop *eventLoop, long long milliseconds,
        timeEventHandler *proc, void *data,
        timeEventFinalizeHandler *finalizerProc)
{
    long long id = eventLoop->next_time_id++;
    ae_time_event *te = (ae_time_event *)malloc(sizeof(ae_time_event));
    if (te == NULL) {
        printf("add time event error\n");
        return -1;
    }

    te->id = id;
    aeAddMillisecondsToNow(milliseconds, &te->when_sec, &te->when_ms);
    te->data = data;
    te->timeProc = proc;
    te->finalizeProc = finalizerProc;
    te->next = eventLoop->time_event_head;
    eventLoop->time_event_head = te;

    return id;
}



int aeDeleteTimeEvent(ae_event_loop *eventLoop, long long id)
{
    ae_time_event *te = eventLoop->time_event_head, *prev = NULL;

    while (te) {
        if (te->id == id) {
            if (prev == NULL) 
                eventLoop->time_event_head = te->next;
            else
                prev->next = te->next;
            if (te->finalizeProc)
                te->finalizeProc(eventLoop, te->data);
            
            free(te);

            return 0;  
        }
        prev = te;
        te = te->next;
    }
    return -1;
}



int aeQuickDeleteTimeEvent(ae_event_loop *eventLoop, ae_time_event *time_event, ae_time_event *prev)
{
    if (time_event == NULL)
        return -1;
    if (eventLoop->time_event_head == time_event) {
        eventLoop->time_event_head = time_event->next;
    } else {
        prev->next = time_event->next;
    }
    if (time_event->finalizeProc)
        time_event->finalizeProc(eventLoop, time_event->data);
    return 0;
}



/* 删除并释放ae_event_loop */
void aeFreeEventLoop(ae_event_loop *event_loop)
{
    free(event_loop->event);
    free(event_loop->events);
    free(event_loop->fired);
    free(event_loop);
    event_loop = NULL;
}

/* IO多路复用器主循环 */
void aeMain(ae_event_loop *event_loop)
{
    int event_num;
    while (TRUE) {
        event_num = aePollFileEvent(event_loop);
        if (event_num > 0)
            aeProcessFileEvent(event_loop);
        aeProcessTimeEvent(event_loop);
    }
}

/* 处理所有已就绪的文件事件 */
void aeProcessFileEvent(ae_event_loop *event_loop)
{
    int max = event_loop->fired_max;
    for (int i = 0; i < max; i++) {
        int fd = event_loop->fired[i];
        ae_file_event *e = &event_loop->events[fd];
        if (e->mask & READ_EVENT)
            e->readProc(event_loop, fd, e->data);
        if (e->mask & WRITE_EVENT)
            e->writeProc(event_loop, fd, e->data);
        e->mask = 0;
    }
    event_loop->fired_max = 0;
}


void aeProcessTimeEvent(ae_event_loop *event_loop)
{
    ae_time_event *te = event_loop->time_event_head, *prev = NULL;
    long sec, ms;
    ae_time_event *tmp;
    while (te) {
        aeGetTime(&sec, &ms);
        if (sec > te->when_sec || (sec == te->when_sec && ms >= te->when_ms)) {
            if (te->timeProc)
                te->timeProc(event_loop, te->id, te->data);
            if (prev == NULL) {
                event_loop->time_event_head = te->next;
                tmp = te; 
                te = te->next;
                if (tmp->finalizeProc)
                    tmp->finalizeProc(event_loop, tmp->data);
                free(tmp);
            }
            else {
                prev->next = te->next;
                tmp = te;
                te = te->next;
                if (tmp->finalizeProc)
                    tmp->finalizeProc(event_loop, tmp->data);
                free(tmp);
            }
        }
        else {
            prev = te;
            te = te->next;
        }
    }
}

/* 等待创建新的连接 */
void* aeWaitConnection(void *arg)
{
    ae_event_loop *eventLoop = (ae_event_loop *)arg;
    struct sockaddr_in server_addr, cli_addr;
    socklen_t cliaddr_len;
    int size = SOCKET_SIZE;
    char buf[size];
    memset(buf, 0, size);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listenfd, 16);
    printf("\033[32mWelcom to JCKEEP HOME\n\033[0m---------------------\033[32m\nWaiting connection...\033[0m\n");

    while (TRUE) {
        cliaddr_len = sizeof(cli_addr);
        int connectfd = accept(listenfd, (struct sockaddr *)&cli_addr, &cliaddr_len);

        printf("\033[32mConnecting to port %u\033[0m\n", ntohl(cli_addr.sin_port));

        char *cli_data = (char *)malloc(SOCKET_SIZE * sizeof(char));
        memset(cli_data, 0, SOCKET_SIZE);
        aeAddFileEvent(eventLoop, cliReadProc, cliWriteProc, cli_data, stringFinalize, connectfd, READ_EVENT | WRITE_EVENT);
    }
}

