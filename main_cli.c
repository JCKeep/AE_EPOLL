#include "client.h"
#include "config.h"

int socketfd;

void SIGINT_handler_client(int SIG);


int main(int argc, char **argv)
{
    ae_serial *cli;
    struct sockaddr_in cli_addr;
    char buf[SOCKET_SIZE], op[16], sbuf[SOCKET_SIZE];
    memset(buf, 0, SOCKET_SIZE);
    memset(&cli_addr, 0, sizeof(cli_addr));

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    cli_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &cli_addr.sin_addr);
    cli_addr.sin_port = htons(SERVER_PORT);
    while (connect(socketfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0) {
        printf("ERROR connection\n");
        sleep(4);
    }

    signal(SIGINT, SIGINT_handler_client);

    while (TRUE) {
WHILE1:
        *((int *)buf) = ADD_SERIAL_CLI;
        cli = (ae_serial *)(buf + 4);
        cli->fd = 0;
        printf("127.0.0.1:8087> ");
        scanf("%s", op);
        if (strcmp(op, "ADD") == 0) {
            scanf("%s %d", cli->filename, &cli->mask);
            write(socketfd, buf, SOCKET_SIZE);
            read(socketfd, &cli->fd, sizeof(int));
            if (!strcmp(&VOID2CHAR(&cli->fd), "ERRO")) {
                printf("ERROR: File not found\n");
                goto WHILE1;
            }
            else if (cli->fd)
                printf("\033[32madd event(%d) ok\n\033[0m", cli->fd);
            goto WHILE2;
        }
        else if (strcmp(op, "quit") == 0) {
quit:
            write(socketfd, "DISC", 4);
            printf("\033[32mBye bye\033[0m\n");
            sleep(1);
            goto EXIT;
        }
        else {
            printf("error command\n");
        }
    }

    while (TRUE) {
WHILE2:
        printf("127.0.0.1:8087%s> ", cli->filename);
        scanf("%s", op);
        if (!strcmp(op, "UP")) {
            VOID2INT(sbuf) = CMD_SERIAL_CLI;
            VOID2INT(sbuf + 4) = cli->fd;
            sbuf[8] = HEADER;
            sbuf[9] = TAILER;
            sbuf[8 + CMD_TEMP_POS] = OP_ADD;
            write(socketfd, sbuf, SOCKET_SIZE);
        }
        else if (!strcmp(op, "DOWN")) {
            VOID2INT(sbuf) = CMD_SERIAL_CLI;
            VOID2INT(sbuf + 4) = cli->fd;
            sbuf[8] = HEADER;
            sbuf[9] = TAILER;
            sbuf[8 + CMD_TEMP_POS] = OP_SUB;
            write(socketfd, sbuf, SOCKET_SIZE);
        }
        else if (!strcmp(op, "SET")) {
            int tmp;
            scanf("%d", &tmp);
            VOID2INT(sbuf) = CMD_SERIAL_CLI;
            VOID2INT(sbuf + 4) = cli->fd;
            sbuf[8] = HEADER;
            sbuf[9] = TAILER;
            sbuf[8 + CMD_TEMP_POS] = OP_SET;
            sbuf[8 + CMD_TEMP_DATA] = tmp;
            write(socketfd, sbuf, SOCKET_SIZE);
        }
        else if (!strcmp(op, "info")) {
            VOID2INT(sbuf) = SERIAL_INFO;
            write(socketfd, sbuf, SOCKET_SIZE);
            read(socketfd, sbuf, SOCKET_SIZE);
            printf("client: %d\tserial: %d\n", VOID2INT(sbuf), VOID2INT(sbuf + 4));
        }
        else if (!strcmp(op, "exit")) 
            goto WHILE1;
        else if (!strcmp(op, "quit"))
            goto quit;
        else 
            printf("error command\n");
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
