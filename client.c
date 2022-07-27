#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "client.h"
#include "config.h"

int main()
{
    int fd = open("./mmap", O_RDWR);
    struct stat st;
    fstat(fd, &st);
    cliport *c = (cliport *)mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    printf("filename: ");
    scanf("%s", c->filename);
    printf("mask: ");
    scanf("%d", &c->mask);
    system(CMD_ADD_EVENT);
    close(fd);
    return 0;
}