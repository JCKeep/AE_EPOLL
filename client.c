#include "client.h"
#include "config.h"

int socketfd;

void SIGINT_handler_client(int SIG);

int main(int argc, char **argv)
{
    struct sockaddr_in cli_addr;
    char buf[SOCKET_SIZE], op[16];
    memset(buf, 0, SOCKET_SIZE);
    memset(&cli_addr, 0, sizeof(cli_addr));

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    cli_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &cli_addr.sin_addr);
    cli_addr.sin_port = htons(SERVER_PORT);
    while (connect(socketfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0) {
        printf("ERROR connection\n");
        sleep(5);
    }

    signal(SIGINT, SIGINT_handler_client);

    while (TRUE) {
        *((int *)buf) = SERIAL_CLI;
        cliport *cli = (cliport *)(buf + 4);
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
            write(socketfd, "DISC", 4);
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


void SIGINT_handler_client(int SIG)
{
    write(socketfd, "DISC", 4);
    printf("\n\033[32mBye bye\033[0m\n");
    sleep(1);
    close(socketfd);
    _exit(0);
}
