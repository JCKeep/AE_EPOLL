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
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>

/* debug时使用 */
#define DEBUG

/*--------- 消息协议 ----------*/

#define SIZE 0x08
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
#define EVENT_LIST "EVENT_LIST"


/*------ 事件分类 ------*/

#define READ_EVENT 0x1
#define WRITE_EVENT 0x2



#endif