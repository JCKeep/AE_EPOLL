#include "client.h"
#include "config.h"

int main()
{
    struct sockaddr_in server_addr;
    char buf[SOCKET_SIZE], op[16];
    memset(buf, 0, SOCKET_SIZE);
    memset(&server_addr, 0, sizeof(server_addr));

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(SERVER_PORT);
    while (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("ERROR connection\n");
        sleep(5);
    }

    while (TRUE) {
        cliport *cli = (cliport *)buf;
        printf("127.0.0.1:8087> ");
        scanf("%s", op);
        if (strcmp(op, "ADD") == 0) {
            scanf("%s %d", cli->filename, &cli->mask);
            write(socketfd, buf, SOCKET_SIZE);
            char rec[5] = {0};
            read(socketfd, rec, 4);
            if (strcmp(rec, "PONG") == 0)
                printf("\033[32madd event ok\n\033[0m");

        }
        else if (strcmp(op, "quit") == 0) {
            write(socketfd, "DISCONNECTED", 12);
            printf("\033[32mBye bye\033[0m\n");
            sleep(1);
            goto EXIT;
        }
        else {
            printf("Argument error\n");
        }
    }

EXIT:
    close(socketfd);
    return 0;
}
