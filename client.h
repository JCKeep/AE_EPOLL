#ifndef CLIENT
#define CLIENT

#include "config.h"

typedef struct ae_serial {
    int mask;
    char filename[128];
    int fd;
    struct ae_serial *next;
} ae_serial;

typedef struct ae_client {
    int fd;
    int state;
    time_t last_connect_time;
    struct ae_client *next;
} ae_client;


void *aeCreateSerial(void *data);
void *aeCreateClient(int fd);


#endif

