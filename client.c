#include "client.h"
#include "config.h"


void *aeCreateSerial(void *data)
{
    int size = sizeof(ae_serial);
    ae_serial *s = (ae_serial *)malloc(sizeof(ae_serial));
    strncpy(&VOID2CHAR(s), data, size);
    s->next = NULL;
    return s;
}


void *aeCreateClient(int fd)
{
    ae_client *c = (ae_client *)malloc(sizeof(ae_client));
    c->fd = fd;
    c->state = 0x1;
    time(&c->last_connect_time);
    c->next = NULL;
    return c;
}

