/*
 * main.c
 *
 *  Created on: Jul 15, 2009
 *      Author: psampaio
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lpc22xx.h"
#include "vic.h"
#include "uart.h"
#include "systime.h"
#include "c328r.h"

#include "../glcd/ili9320.h"

unsigned char buff[40000];
int i, j, k;
char s[100];

unsigned short img[40000];

void isrSpurious(void);

int main(void) {
	VICIntEnClr = 0xFFFFFFFF; // clear all interrupts
	VICIntSelect = 0x00000000; // clear all FIQ selections
	VICDefVectAddr = (unsigned long) isrSpurious; // point unvectored IRQs to reset()


	initSysTime(); // initialize the system timer

	uart1Init(B9600, UART_8N1, UART_FIFO_OFF); // setup the UART
	uart0Init(B9600, UART_8N1, UART_FIFO_OFF);
	enableIRQ();

	uart1Puts("\n\r\n\r*** CAMERA TEST C328R ***\n\r");

	//	C328RInit();
	GLcdInit();
	GLcdClear(BLACK);

	uart0TxFlush();

	for (;;) {
		for (;;) {
			if (Sync() != -1) {
				uart1Puts("Sync: OK\n\r");
				pause(ONE_SEC);
				if ((i = SetBaudRate(Baud115200)) == 0)
					uart1Puts("Baudrate:OK\n\r");
				break;
			}
			uart1Puts("Not Sync\n");
		}
		uart0Init(B115200, UART_8N1, UART_FIFO_OFF);
		i = Initial(Color16, R160x128, 0);
		if (i)
			continue;
		uart1Puts("Initial: OK\n\r");
		i = LightFrequency(F50Hz);
		if (i)
			continue;
		uart1Puts("LigthFrequency: OK\n\r");
		for (;;) {
			i = Snapshot(Uncompressed, 0);
			if (i)
				break;
			uart1Puts("Snapshot: OK\n\r");

			i = GetRawPicture(R_Snapshot, buff, 0);
			if (i == -1) {
				uart1Puts("GetRawPicError\n\r");
				break;
			}
			sprintf(s, "GetRawPicture: OK (Len: %d)\n\r", i);
			uart1Puts(s);
			img[0] = 128;
			img[1] = 160;
			for (k = 2, j = 0; j < i; k++) {
				img[k] = buff[j] << 8 | buff[j + 1];
				j += 2;
			}
			GLcdImage(0, 0, img, 1);
			//		pause(FIVE_SEC);
		}
	}
}
