all: server client

server: server.c handler.c ae_epoll.c  
	gcc -g -Wall -o server server.c handler.c ae_epoll.c -lpthread

client: client.c  
	gcc -g -Wall -o cli client.c  


.PHONY: clean

clean:
	rm server cli
