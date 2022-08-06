#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "server.h"
#include "handler.h"
#include "ae_epoll.h"

char cmd[128];
ae_server *server;


int main(int argc, char **argv)
{
    server = aeCreateServer();
    signal(SIGINT, SIGINT_handler_server);

    aeMain(server->eventLoop);

    return 0;
}
