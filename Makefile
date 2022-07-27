all: main

main: main.c handler.c ae_epoll.c
	gcc -g -Wall -o main main.c handler.c ae_epoll.c -lpthread


.PHONY: clean

clean:
	rm main
