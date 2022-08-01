#ifndef SERVER
#define SERVER

#include "client.h"
#include "ae_epoll.h"


typedef struct ae_server {
    int serial_size;
    int client_size;
    ae_serial *serial_head;
    ae_client *client_head;
    ae_event_loop *eventLoop;
} ae_server;


void *aeCreateServer();
void aeServerPushClient(ae_server *server, ae_client *cli);
void aeServerPopClient(ae_server *server, int fd);
void aeServerPushSerial(ae_server *server, ae_serial *ser);
void aeServerPopSerial(ae_server *server, int fd);
char *aeServer2String(ae_server *server);


#endif