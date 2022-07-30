all: server client

server: server.c handler.c ae_epoll.c  logger.c
	gcc -g -Wall -Wunused-label -o server server.c handler.c ae_epoll.c logger.c -lpthread

client: client.c  
	gcc -g -Wall -Wunused-label -o cli client.c  


.PHONY: clean

clean:
	rm server cli
