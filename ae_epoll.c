#include "ae_epoll.h"
#include "handler.h"
#include "client.h"

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
int aeAddEvent(ae_event_loop *event_loop, eventHandler *readProc, eventHandler *writeProc, int fd, int mask)
{
    pthread_mutex_lock(&lock);
    int mod = (event_loop->events[fd].readProc != NULL || event_loop->events[fd].writeProc != NULL) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    ae_event *e = &event_loop->events[fd];
    e->mask = 0;
    e->readProc = NULL;
    e->writeProc = NULL;
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
    memset(e->data, 0, SIZE);
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
int aeDeleteEvent(ae_event_loop *event_loop, int fd)
{
    pthread_mutex_lock(&lock);
    ae_event *e = &event_loop->events[fd];
    struct epoll_event event;
    event.data.u64 = 0;
    event.events = 0;
    event.data.fd = fd;
    if (e->readProc != NULL)
        event.events |= (EPOLLIN | EPOLLET);
    if (e->writeProc != NULL)
        event.events |= (EPOLLOUT | EPOLLET);
    memset(e->data, 0, SIZE);
    e->mask = 0;
    e->readProc = NULL;
    e->writeProc = NULL;
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
int aePollEvent(ae_event_loop *event_loop)
{
    int i, write_num = 0;
    while (1) {
        write_num = epoll_wait(event_loop->epfd, event_loop->event, 1024, -1);
        if (write_num == 0)
            return 0;
        for (i = 0; i < write_num; i++) {
            int fd = event_loop->event[i].data.fd;
            ae_event *e = &event_loop->events[fd];
            struct epoll_event *event = &event_loop->event[i];
            if (event->events & EPOLLIN || event->events & EPOLLOUT) {
                if (event->events & EPOLLIN)
                    e->mask |= READ_EVENT;
                if (event->events & EPOLLOUT)
                    e->mask |= WRITE_EVENT;
                event_loop->fired[event_loop->fired_max++] = fd;
            }
        }
        if (write_num > 0)
            return 1;
    }
    return 0;
}


/* 创建新的ae_event_loop */
ae_event_loop* aeCreateEventLoop()
{
    ae_event_loop *event_loop = (ae_event_loop *)malloc(sizeof(ae_event_loop));
    if (!event_loop) {
        perror("malloc event_loop");
        return NULL;
    }

    event_loop->epfd = epoll_create(1024);
    event_loop->fired = (int *)malloc(sizeof(int) * 1024);
    event_loop->fired_max = 0;
    event_loop->max_fd = 0;
    event_loop->size = 0;
    event_loop->event = (struct epoll_event *)malloc(1024 * sizeof(struct epoll_event));
    event_loop->events = (ae_event *)malloc(1024 * sizeof(ae_event));
    memset(event_loop->event, 0, 1024 * sizeof(struct epoll_event));
    memset(event_loop->events, 0, 1024 * sizeof(ae_event));
    memset(event_loop->fired, 0, sizeof(int) * 1024);
    return event_loop;
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
    while (TRUE) {
        if (aePollEvent(event_loop) > 0) {
            aeProcessProc(event_loop);
        }
    }
}

/* 处理所有已就绪的文件事件 */
void aeProcessProc(ae_event_loop *event_loop)
{
    int max = event_loop->fired_max;
    for (int i = 0; i < max; i++) {
        int fd = event_loop->fired[i];
        ae_event *e = &event_loop->events[fd];
        if (e->mask & READ_EVENT)
            e->readProc(event_loop, fd);
        if (e->mask & WRITE_EVENT)
            e->writeProc(event_loop, fd);
        e->mask = 0;
    }
    event_loop->fired_max = 0;
}

/* 等待创建新的连接 */
void* aeWaitEvent(void *arg) 
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

        aeAddEvent(eventLoop, cliReadProc, cliWriteProc, connectfd, READ_EVENT | WRITE_EVENT);
    }
}

