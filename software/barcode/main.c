/*
 * main.c
 * Barcode test
 *
 * http://en.wikipedia.org/wiki/European_Article_Number
 * http://www.piclist.com/techref/barcode/ean13.htm
 *
 *  Created on: Jul 15, 2009
 *      Author: psampaio
 */
#include <stdarg.h>
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
void Uart1PutChar(char c) {
	while (!(UART1_LSR & (1 << 5)))
		;
	UART1_THR = c;
}

void Uart1PutStr(char *s) {
	while (*s) {
		Uart1PutChar(*s);
		s++;
	}
}

void Uart1Printf(char *format, ...) {
	va_list argptr;
	static char buffer[100];
	int len;

	va_start (argptr, format);
	len = vsprintf(buffer, format, argptr);
	Uart1PutStr(buffer);
	va_end(argptr);
}

unsigned short ticks;
void Timer0Handler(void) /*__irq */
{
	TIMER0_IR = 1; /* clear interrupt flag */
	ticks++;
//			if (ticks%2) {
//				IO0SET = (1 << 23);
//			}
//			else {
//				IO0CLR =(1 << 23);
//			}
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void Timer0ISRH(void);

/******************************************************************************
 ** Function name:		init_timer
 **
 ** Descriptions:		Initialize timer0, set timer interval, reset timer,
 **						install timer interrupt handler
 **
 ** parameters:			timer interval
 ** Returned value:		none
 **
 ******************************************************************************/
void initTimer(void) {
	ticks = 0;

	PCONP |= (1 << 1); // Enable TMR0 clk
	TIMER0_TCR = 0x2; // counting  disable and set reset
	TIMER0_TCR = 0x0; // release reset
	TIMER0_TCR = 0; // Timer Mode: every rising PCLK edge
	TIMER0_MCR = 0x3; // Enable Interrupt on MR0, Enable reset on MR0

	// set timer 0 period
	TIMER0_PR = 2;
	TIMER0_MR0 = 500;

	// init timer 0 interrupt
	TIMER0_IR |= (1 << 0); // clear pending interrupt

	// initialize the interrupt vector
	VICIntSelect &= ~VIC_BIT(VIC_TIMER0); // UART1 selected as IRQ
	VICIntEnable = VIC_BIT(VIC_TIMER0); // UART1 interrupt enabled
	VICVectCntl1 = VIC_ENABLE | VIC_TIMER0;
	VICVectAddr1 = (unsigned long) Timer0ISRH; // address of the ISR

	TIMER0_TCR |= (1 << 0); // counting Enable
}

void enableIRQ(void);

#define VECT_ADDR_INDEX	0x100
#define VECT_PRIO_INDEX 0x200

void initAIC(void) {
	unsigned int i = 0;
	unsigned int *vect_addr, *vect_prio;

	/* initialize VIC*/
	VICIntEnClr = 0xffffffff;
	VICVectAddr = 0;
	VICIntSelect = 0;

	/* set all the vector and vector control register to 0 */
	for (i = 0; i < 32; i++) {
		vect_addr = (unsigned int *) (VIC_BASE_ADDR + VECT_ADDR_INDEX + i * 4);
		vect_prio = (unsigned int *) (VIC_BASE_ADDR + VECT_PRIO_INDEX + i * 4);
		*vect_addr = 0x0;
		*vect_prio = 0xF;
	}
}

void delay(int d) {
	unsigned int v = ticks;
	while ((ticks - v) < d)
		;
}

unsigned int previous, actual, counter;

unsigned int counter_read() {
	previous = actual;
	actual = ticks;
	counter = (counter & 0xffff0000) + actual;
	if (actual < previous)
		counter = (counter + 0x10000);
	return counter;
}

#define timer_start(x)	(x = counter_read())

#define timer_restart(i, t) do {				\
	i += t;										\
} while(0)

#define timer_elapsed(i, e) do {				\
	unsigned int n = counter_read();						\
	e = n - i;									\
} while(0)

#define timer_delay(d) do {						\
	unsigned int i, e;									\
	timer_start(i);								\
	do {										\
		timer_elapsed(i, e);	  				\
	} while (e < d);							\
} while (0)

int read_signal() {
	return (IO0PIN & (1 << 21)) == 0;
}

char code[60];
int i_code;
char validData[13];

int hand_barcode() {
	size_t n;
	unsigned int timer, actualtime, bittime;

	int aux = read_signal();
	while (read_signal() == aux);
	timer_start(timer);
	aux = read_signal();
	while (read_signal() == aux) {
		timer_elapsed(timer, bittime);
		if (bittime > 10000)
			continue;
	}
	timer_restart(timer, bittime);
	code[0] = 1;
	i_code = 1;
	while (1) {
		aux = read_signal();
		while (read_signal() == aux) {
			timer_elapsed(timer, actualtime);
			if (actualtime > 5000) {
#ifdef DEBUG
				Uart1PutChar('>');
				for (n = 0; n < i_code; ++n)
					Uart1PutChar('0' + code[n]);
				Uart1Printf("\n\r");
#endif
				return i_code;
			}
		}
		timer_restart(timer, actualtime);
		n = (actualtime + (bittime / 2)) / bittime;
		bittime = (actualtime + bittime) / (n + 1);
		code[i_code++] = n;
		if (i_code > 60)
			return -1;
	}
}

char leftOdd[][4] = { {3,2,1,1}, {2,2,2,1}, {2,1,2,2}, {1,4,1,1}, {1,1,3,2}, {1,2,3,1},
		{1,1,1,4}, {1,3,1,2}, {1,2,1,3}, {3,1,1,2} };
char leftEven[][4] = { {1,1,2,3}, {1,2,2,2}, {2,2,1,2}, {1,1,4,1}, {2,3,1,1}, {1,3,2,1},
		{4,1,1,1}, {2,1,3,1}, {3,1,2,1}, {2,1,1,3} };

char right[][4] = { {3,2,1,1}, {2,2,2,1}, {2,1,2,2}, {1,4,1,1}, {1,1,3,2}, {1,2,3,1},
		{1,1,1,4}, {1,3,2,1}, {1,2,1,3}, {3,1,1,2} };

int Find(char *s, char tab[][4])
{
	int i;

	for(i = 0; i < 10; i++) {
		if (memcmp(s, tab[i], 4) == 0) return i;
	}
	return -1;
}

char *eanCodeSeq[] = {
		"LLLLLL","LLGLGG", "LLGGLG", "LLGGGL", "LGLLGG", "LGGLLG", "LGGGLL", "LGLGLG", "LGLGGL", "LGGLGL"
};

int EanCode(char *seq)
{
	int i;

	for(i = 0; i < 10; i++) {
		if (memcmp(seq, eanCodeSeq[i], 6) == 0) return i;
	}
	return -1;
}

int Decode(char *ptr)
{
	char ean_code[6];
	int i, d, reverse, pos;
	char pattern[5] = {1,1,1,1,1};

	if (memcmp(ptr, pattern, 3) != 0) return -1;
	ptr += 3;
	reverse = 0;
	pos = 1;
	for (i = 0; i < 6; i++) {
		if ((d = Find(ptr, leftOdd)) == -1) {
			if ((d = Find(ptr, leftEven)) == -1) {
				if ((d = Find(ptr, right)) == -1) {
					return -2;
				}
			}
			else {
				if (i == 0) {
					reverse = 1;
					pos = 12;
				}
				ean_code[i]='G';
			}
		}
		else {
			ean_code[i] = 'L';
		}
		validData[pos] = d;
		pos += (reverse == 1) ? -1: 1;
		ptr += 4;
	}
	if (reverse == 0) {
		validData[0] = EanCode(ean_code);
	}
	if (memcmp(ptr, pattern, 5) != 0) return -3;
	ptr += 5;
	for (i = 0; i < 6; i++) {
		if ((d = Find(ptr, leftOdd)) == -1) {
			if ((d = Find(ptr, leftEven)) == -1) {
				if ((d = Find(ptr, right)) == -1) {
					return -4;
				}
			}
			else {
				ean_code[5 - i] = 'L';
			}
		}
		else {
			ean_code[5 - i] = 'G';
		}
		validData[pos] = d;
		pos += (reverse == 1) ? -1: 1;
		ptr += 4;
	}
	if (reverse) {
		validData[0] = EanCode(ean_code);
	}
	return 0;
}

int main(void) {
	int i;

	initAIC();
	initTimer();

	PINSEL0 &= ~0x50000u;
	PINSEL0 |= 0x50000u;
	UART1_LCR = 0x80;
	UART1_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART1_DLL = BAUDRATEDIVISOR & 0x00ff;
	UART1_LCR = 3;

	PINSEL1 &= ~(0x3 << 10);	// set io pins for led P0.23
	IO0DIR |= (1 << 23);
	IO0SET = (1 << 23); 		// led off
	IO0CLR = (1 << 23); 		// led on

	enableIRQ();

	Uart1PutStr("\n\n*** TESTE BARCODE READER ***\n");
	while (1) {
		Uart1Printf("Reading:... ");
		if (hand_barcode() == 59) {
			Uart1Printf("OK.\nDECODE: ");
			if (Decode(code) == 0) {
				for(i = 0; i < 13; i++) {
					if ((i == 1) || (i == 7)) {
						Uart1PutChar('-');
					}
					Uart1PutChar('0'+validData[i]);
				}
				Uart1PutStr("\n");
			}
			else {
				Uart1Printf("Error\n");
			}
		}
		else {
			Uart1Printf("Error (reading)\n");
		}
	}
	return 0;
}
