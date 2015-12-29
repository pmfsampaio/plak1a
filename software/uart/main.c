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

//XTAL frequency in Hz
#define XTALFREQ  14745600
#define CCLK      XTALFREQ*4

//pclk must always be XTALFREQ/4 when using the ROM monitor.
#define PCLKFREQ (CCLK)

#define BAUDRATE    9600
#define BAUDRATEDIVISOR (PCLKFREQ/(BAUDRATE*16))

void Uart0PutChar(char c) {
	while (!(UART0_LSR & (1 << 5)))
		;
	UART0_THR = c;
/*
	if (c == '\n') {
		while (!(UART0_LSR & (1 << 5)))
			;
		UART0_THR = '\r';
	}
*/

}

void Uart0PutStr(char *s) {
	while (*s) {
		Uart0PutChar(*s);
		s++;
	}
}

void Uart0PutStrLen(char *s, int len) {
	for(;len;len--) {
		Uart0PutChar(*s);
		s++;
	}
}


int Uart0HasCh(void)
{
	if (UART0_LSR & (1 << 0)) {
		return UART0_RBR;
	}
	return 0;
}

int Uart0GetCh(void)
{
	while ( !(UART0_LSR & (1 << 0)) ); // wait for character
	return UART0_RBR;                // return character
}


void Uart1PutChar(char c) {
	while (!(UART1_LSR & (1 << 5)))
		;
	UART1_THR = c;
/*
	if (c == '\n') {
		while (!(UART1_LSR & (1 << 5)))
			;
		UART1_THR = '\r';
	}
*/
}

void Uart1PutStr(char *s) {
	while (*s) {
		Uart1PutChar(*s);
		s++;
	}
}

int Uart1GetCh(void)
{
	while ( !(UART1_LSR & (1 << 0)) ); // wait for character
	return UART1_RBR;                // return character
}

void Delay(int j) {
	int i;
	for (i = 0; i < 4000 * j; i++)
		;
}


int i, j, ch;
char s[20];

char sync[] = { 0xaa, 0x0d, 0, 0, 0, 0};
int main(void) {
	PINSEL0 &= ~0x50005u;
	PINSEL0 |= 0x50005u;
	UART1_LCR = 0x80;
	UART1_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART1_DLL = BAUDRATEDIVISOR & 0x00ff;

	UART1_LCR = 3;

	UART0_LCR = 0x80;
	UART0_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART0_DLL = BAUDRATEDIVISOR & 0x00ff;

	UART0_LCR = 3;

	Uart0PutStr("TESTE\n");

	for (;;) {
		if (UART1_LSR & (1 << 0)) {
			ch =UART1_RBR;
			Uart0PutChar(ch);
		}
		if (UART0_LSR & (1 << 0)) {
			ch =UART0_RBR;
			Uart1PutChar(ch);
		}
	}
}
