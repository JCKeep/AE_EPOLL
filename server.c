#include "server.h"
#include "handler.h"

void *aeCreateServer()
{
    ae_server *s = (ae_server *)malloc(sizeof(ae_server));
    s->client_size = 0;
    s->serial_size = 0;
    s->eventLoop = aeCreateEventLoop();
    s->serial_head = NULL;
    s->client_head = NULL;

    struct sockaddr_in server_addr;
    s->fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bind(s->fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(s->fd, 16);
    printf("\033[32mWelcom to JCKEEP HOME\n\033[0m---------------------\033[32m\nWaiting connection...\033[0m\n");

    char *buf = (char *)malloc(sizeof(struct sockaddr_in));
    memset(buf, 0, sizeof(struct sockaddr_in));
    aeAddFileEvent(s->eventLoop, serverReadProc, NULL, buf, NULL, stringFinalize, s->fd, READ_EVENT, 0);

    return s;
}

void aeServerPushClient(ae_server *server, ae_client *cli)
{
    cli->next = server->client_head;
    server->client_head = cli;
    server->client_size++;
}


void aeServerPopClient(ae_server *server, int fd)
{
    ae_client *cli = server->client_head, *prev = NULL;
    while (cli) {
        if (cli->fd == fd) {
            if (prev == NULL)
                server->client_head = cli->next;
            else
                prev->next = cli->next;
            free(cli);
            server->client_size--;
            return;
        }
        prev = cli;
        cli = cli->next;
    }
}


void aeServerPushSerial(ae_server *server, ae_serial *ser)
{
    ser->next = server->serial_head;
    server->serial_head = ser;
    server->serial_size++;
}


void aeServerPopSerial(ae_server *server, int fd)
{
    ae_serial *cli = server->serial_head, *prev = NULL;
    while (cli) {
        if (cli->fd == fd) {
            if (prev == NULL) 
                server->serial_head = cli->next;
            else 
                prev->next = cli->next;
            free(cli);
            server->serial_size--;
            return;
        }
        prev = cli;
        cli = cli->next;
    }
}

char *aeServer2String(ae_server *server)
{
    printf("client: %d\tserial %d\n", server->client_size, server->serial_size);

    return NULL;
}

