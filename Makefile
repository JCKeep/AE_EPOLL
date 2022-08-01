all: server client

server: main_server.c server.c handler.c ae_epoll.c  logger.c
	gcc -g -Wall -Wunused-label -o server main_server.c server.c handler.c ae_epoll.c logger.c client.c -lpthread

client: client.c main_cli.c
	gcc -g -Wall -Wunused-label -o cli client.c main_cli.c  


.PHONY: clean

clean:
	rm server cli
