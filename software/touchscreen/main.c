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
#include "startosc.h"
#include "touchscreen.h"

//XTAL frequency in Hz
#define XTALFREQ  14745600
//#define CCLK      XTALFREQ*4

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
char s[20];

touchscreen_data tsData;

int main(void) {
	PINSEL0 |= 0x50000u;
	UART1_LCR = 0x80;
	UART1_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART1_DLL = BAUDRATEDIVISOR & 0x00ff;

	UART1_LCR = 3;

	PutStr("TESTE\n");

	memset(&tsData, 0, sizeof(tsData));
	InitTS(tsData);

	for (;;) {
		tsData = GetTS();
		sprintf(s, " X: %6ld", tsData.xvalue);
		PutStr(s);
		sprintf(s, " Y: %6ld", tsData.yvalue);
		PutStr(s);
		sprintf(s, " P: %6ld  \r", tsData.pvalue);
		PutStr(s);
		Delay(25);
	}
}
