#ifndef CONFIG
#define CONFIG



#include <stdio.h>
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

/* debug时使用 */
#define DEBUG

/*--------- 消息协议 ----------*/

#define SIZE 0x08 /* 消息大小 */
#define TRUE 0x1
#define FALSE 0x0
#define HEADER 0xFE /* 消息头，用于验证消息正确性 */
#define TAILER 0xFF /* 消息尾，用于验证消息正确性 */
#define TEMP_MARK 0x01 /* 温度标识 */
#define LIGHT_MARK 0x02 /* 光照标识 */

/*---------- 命令 ------------*/

#define CMD_HEADER "redis-cli -c -p 7002 set "
#define CMD_TEMP "temp "
#define CMD_LIGHT "light "

/*------ 事件 ------*/

#define READ_EVENT 0x1
#define WRITE_EVENT 0x2
#define MIN_BLOCK_TIME 50

/*--------- Socket-- -------*/

#define SERVER_PORT 8087
#define SOCKET_SIZE (sizeof(cliport) + 4)


#define SERIAL_CLI 0x1
#define JAVA_CLI 0x2



#endif
