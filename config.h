#ifndef CONFIG
#define CONFIG



#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* Debug时使用 */
#define DEBUG


/*--------- 消息协议 ----------*/

#define SIZE 0x08 /* 消息大小 */
#define TRUE 0x1
#define FALSE 0x0
#define HEADER 0xFE /* 消息头，用于验证消息正确性 */
#define TAILER 0xFF /* 消息尾，用于验证消息正确性 */
#define TEMP_MARK 0x01 /* 温度标识 */
#define LIGHT_MARK 0x02 /* 光照标识 */


/*---------- Command ------------*/

#define CMD_HEADER "redis-cli -c -p 7002 set "
#define CMD_TEMP "temp "
#define CMD_LIGHT "light "


/*------ Event ------*/

#define READ_EVENT 0x1
#define WRITE_EVENT 0x2
#define CLIENT_EVENT 0x8
#define SERIAL_EVENT 0x4
#define MIN_BLOCK_TIME 50


/*--------- Server-- -------*/

#define SERVER_PORT 8087
#define SOCKET_SIZE (sizeof(ae_serial) + 4)


/*--------- Client ----------*/

#define ADD_SERIAL_CLI 0x1
#define CMD_SERIAL_CLI 0x100
#define CMD_TEMP_POS 0x2
#define CMD_TEMP_DATA 0x3
#define ADD_JAVA_CLI 0x2
#define CMD_JAVA_CLI 0x200
#define SERIAL_INFO 0x400

#define OP_ADD 0x1
#define OP_SUB 0x2
#define OP_SET 0x4



#define VOID2UCHAR(p) (*((u_char *)(p)))
#define VOID2CHAR(p) (*((char *)(p)))
#define VOID2INT(p) (*((int *)(p)))
#define VOID2UINT(p) (*((uint32_t *)(p)))
#define VOID2LONG(p) (*((long *)(p)))
#define VOID2ULONG(p) (*((u_long *)(p)))

#endif
