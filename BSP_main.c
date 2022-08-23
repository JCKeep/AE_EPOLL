#include "STC15F2K60S2.H" 
#include "sys.H"
#include "displayer.H"
#include "key.H"
#include "Vib.H"
#include "beep.H" 
#include "hall.H"
#include "music.h" 
#include "adc.h"
#include "uart1.h"
#include "uart2.h"
#include "IR.h"
#include "stepmotor.h"
#include "DS1302.h"
#include "FM_Radio.h"
#include "M24C02.h"
#include "EXT.h"

#define NULL ((void *)0)
#define SIZE 0x8
#define HEADER 0xFE
#define TAILER 0xFF
#define OP_TEMP 0x2
#define OP_TEMP_DATA 0x3

#define OP_ADD 0x1
#define OP_SUB 0x2
#define OP_SET 0x4

/* 温度 */
#define TEMP_MARK 0x01

/* 光照 */
#define LIGHT_MARK 0x02

typedef unsigned char uchar;

code unsigned long SysClock=11059200;
code unsigned char decode_table[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7D, 0x07, 0x7F, 0x6F};

/* 温度对照表 */
code int tempdata[]={239,197,175,160,150,142,135,129,124,120,116,113,109,107,104,101, 
										  99, 97, 95, 93, 91, 90, 88, 86, 85, 84, 82, 81, 80, 78, 77, 76, 
										  75, 74, 73, 72, 71, 70, 69, 68, 67, 67, 66, 65, 64, 63, 63, 62, 
										  61, 61, 60, 59, 58, 58, 57, 57, 56, 55, 55, 54, 54, 53, 52, 52, 
										  51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 
										  43, 43, 42, 42, 41, 41, 41, 40, 40, 39, 39, 38, 38, 38, 37, 37, 
										  36, 36, 36, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 31, 
										  30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25,
										  24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 
										  19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 
										  13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 
										  7, 7, 6, 6,5, 5, 5, 4,4, 3, 3,3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, 
										  -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, 
										  -10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17, 
										  -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, 
										  -30, -32, -33, -35, -36, -38, -40, -43, -46, -50, -55, -63, 361};

unsigned char i;
unsigned char buf[SIZE] = {HEADER, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, TAILER};
unsigned char rev[SIZE] = {0x00};
unsigned char temp = 25;
unsigned char match[] = {HEADER, TAILER};
unsigned char count = 0;
struct_ADC adc;
int AD = 0;


void callback1ms() {}

void callback100ms()
{
	if (count++ % 20 == 0) {
	adc = GetADC();
	if (GetUart1TxStatus() == enumUart1TxFree) {
		if (adc.Rop) {
			buf[1] |= LIGHT_MARK;
			buf[LIGHT_MARK + 1] = adc.Rop;
		}
		if (adc.Rt) {
			buf[1] |= TEMP_MARK;
			buf[TEMP_MARK + 1] = tempdata[adc.Rt >> 0x2];
		}
		Uart1Print(buf, SIZE);
	}
	}
	Seg7Print(0, 0, 0, 0, 0, 0, temp / 10, temp % 10);
}

void uart1_callback()
{
	if (rev[OP_TEMP] & OP_ADD) {
		if (temp < 30)
			temp++;
	}
	else if (rev[OP_TEMP] & OP_SUB) {
		if (temp > 16)
			temp--;
	}
	else if (rev[OP_TEMP] & OP_SET) {
		if (rev[OP_TEMP_DATA] >= 16 && rev[OP_TEMP_DATA] <= 30)
			temp = rev[OP_TEMP_DATA];
	}
}

void f() {

}

int main()
{
	DisplayerInit();
	Uart1Init(9600L);
	AdcInit(ADCincEXT);
	SetDisplayerArea(6, 7);
	SetUart1Rxd(rev, 8, match, 2);
	SetEventCallBack(enumEventSys1mS, callback1ms);
	SetEventCallBack(enumEventSys100mS, callback100ms);
	SetEventCallBack(enumEventXADC, f);
	SetEventCallBack(enumEventUart1Rxd, uart1_callback);
	LedPrint(0);
	MySTC_Init();
	while (1) {
		MySTC_OS();
	}
}