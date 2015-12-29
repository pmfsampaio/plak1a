/*
 * main.c
 *
 *  Created on: Jul 15, 2009
 *      Author: psampaio
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "startosc.h"
#include "lpc22xx.h"

#include "smsc9118.h"


//XTAL frequency in Hz
#define XTALFREQ  14745600
#define CCLK      XTALFREQ*4

//pclk must always be XTALFREQ/4 when using the ROM monitor.
#define PCLKFREQ (CCLK)

#define BAUDRATE    9600
#define BAUDRATEDIVISOR (PCLKFREQ/(BAUDRATE*16))


void PutChar(char c) {
	while (!(UART1_LSR & (1 << 5)))
		;
	UART1_THR = c;
	if (c == '\n') {
		while (!(UART1_LSR & (1 << 5)))
			;
		UART1_THR = '\r';

	}
}

void PutStr(char *s) {
	while (*s) {
		PutChar(*s);
		s++;
	}
}

void Delay(int j) {
	int i;
	for (i = 0; i < 4000 * j; i++)
		;
}


int i, ch;
char s[200];

volatile unsigned long *pLan = (unsigned long *)0x82000000;
volatile unsigned long aux;

int main(void) {

	PINSEL0 |= 0x50000u;
	UART1_LCR = 0x80;
	UART1_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART1_DLL = BAUDRATEDIVISOR & 0x00ff;

	UART1_LCR = 3;

	PutStr("TESTE LAN\n");

	// Set Data bus functionality
	// 0000 1111 1000 0001 0100 1001 0010 0100
	PINSEL2 = 0x0F814924;
	/*
	 *  0010 0000 0000 0000 0000 1100 0010 0000
	 * |    |    |    |    |    |    |    |    |
	 *                       WST2   WST1   IDLE
	 * |00xx|RWP0|----|----|wwww|wBww|www-|iiii|
	 *    |  ||||                 |
	 *    |  |||+-- BUSERR        +-- RBLE
	 *    |  ||+--- WPERR
	 *    |  |+---- WRITE PROTECT
	 *    |  +----- Burst-ROM
	 *    +-------- 00 - 8 / 01 - 16 / 10 - 32 bits
	 */
	BCFG2 = 0x2000ffe0;

	pLan = (unsigned long *)0x82000064;
	aux = *pLan;

	pLan = (unsigned long *)0x82000084;
	aux = *pLan;

	pLan = (unsigned long *)0x82000050;
	aux = *pLan;

	eth_init(0);
	eth_halt();

#if 0
	for (;;) {
		aux = *pLan;
		if (aux != 0) {
			PutChar('*');
//			sprintf(s, "%p -> %x\n", pLan, aux);
//			PutStr(s);
		}
//		pLan++;
//		if (pLan > (unsigned int *)0x82000fff) pLan = (unsigned int *)0x82000000;

	}
#endif
}
