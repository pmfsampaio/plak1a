#include <stdio.h>
#include "lpc22xx.h"

#define PLOCK 0x400

/*
 * Setting the Phased Lock Loop (PLL)
 * PLAK1a has a 14.7456 MHz crystal
 *
 * Put PLAK1a run at 53.2368 MHz (has to be an even multiple of crystal)
 * According to the Philips LPC2106 manual:
 * M = cclk / Fosc		where:		M = PLL multiplier (PLLCFG)
 * 									cclk = 53236800 Hz
 * 									Fosc = 14745600 Hz
 * M = 3 (after round up and then subtract 1)
 *
 *
 * The Current Controlled Oscilator (CCO) must operate in the range 156 MHz to 320 MHz
 * According to the Philips LPC2106 manual:
 * Fcco = cclk * 2 * P    where:	Fcco = CCO frequency
 * 									cclk = 53236800 Hz
 * 									P = PLL divisor (PLLCFG)
 * P = 2 (produce a Fcco = 212947200 Hz)
 *
 * PLLCFG = 0x23
 */
void StartOsc(void)
{
	// Setting Multiplier and Divider values
	PLLCFG = 0x23;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// Enabling the PLL */
	PLLCON = 0x1;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// Wait for the PLL to lock to set frequency
	while (!(PLLSTAT & PLOCK))
		;
	// Connect the PLL as the clock source
	PLLCON = 0x3;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// Enabling MAM and setting number of clocks used for Flash memory fetch (4 cclks in this case)
	MAMCR = 0x2;
	MAMTIM = 0x3;
	// Setting peripheral Clock (pclk) to System Clock (cclk)
	VPBDIV = 0x1;
}



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

char *itoa_1(unsigned long integer, char *string) {
	if (integer / 10 != 0) {
		string = itoa_1(integer / 10, string);
	}
	*string++ = (char) ('0' + integer % 10);
	return string;
}

volatile unsigned i, i2;
char s[20];
int j;
int fail;

#define SRAM_BASE_ADDRESS	0x81000000ul
#define SRAM_SIZE     		0x400000

volatile unsigned int *ptrSram, *ptrBase;

void TestSRAM() {
	PutStr("Testing SRAM \n");

	unsigned int i = 0; //need for cycles
	unsigned int* pAdd = 0; //pointer to SRAM

	PutStr("Writing (1) data to SRAM \n");
	for (i = 0, pAdd = (unsigned int *) SRAM_BASE_ADDRESS; pAdd
			< (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4; ++pAdd, ++i) {
		*pAdd = i;
	}

	PutStr("Reading (1) data from SRAM \n");
	for (i = 0, pAdd = (unsigned int *) SRAM_BASE_ADDRESS; pAdd
			< (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4; ++pAdd, ++i) {
		if (*pAdd != i)
			break;
	}

	if (pAdd >= (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4) {
		PutStr("Veryfied (1) data is OK! \n");
	} else
		PutStr("*** Veryfied (1) is NOT OK!***\n");

	PutStr("Writing (2) data to SRAM \n");
	for (i = 0, pAdd = (unsigned int *) SRAM_BASE_ADDRESS; pAdd
			< (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4; ++pAdd, ++i) {
		*pAdd = ~i;
	}

	PutStr("Reading (2) data from SRAM \n");
	for (i = 0, pAdd = (unsigned int *) SRAM_BASE_ADDRESS; pAdd
			< (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4; ++pAdd, ++i) {
		if (*pAdd != ~i)
			break;
	}

	if (pAdd >= (unsigned int *) SRAM_BASE_ADDRESS + SRAM_SIZE / 4) {
		PutStr("Veryfied (2) data is OK! \n");
	} else
		PutStr("*** Veryfied (2) is NOT OK! ***\n");


}

int main(void) {
	StartOsc();
	PINSEL0 |= 0x50000u;
	UART1_LCR = 0x80;
	UART1_DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
	UART1_DLL = BAUDRATEDIVISOR & 0x00ff;

	UART1_LCR = 3;

	PutStr("TESTE MEMORY\n");

	// Set Data bus functionality
	// 0000 1111 1000 0001 0100 1001 0010 0100
	PINSEL2 = 0x0F814924;
	/*
	 *  0010 0000 0000 0000 0001 1100 1000 0000
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
	//	BCFG1 = 0x20001c80;
	// Initialization of EMC
	// FLASH 16bit; IDCY = F; WST1 = 4; RBLE = 1; WST2 = 6
	//	  BCFG0=0x1000348F;
	// SRAM 32bit; IDCY = F; WST1 = 2; RBLE = 1; WST2 = 1
	BCFG1 = 0x20000c40;

	TestSRAM();

#if 0
	fail = 0;

	ptrSram = (unsigned int *)SRAM_BASE_ADDRESS;

	ptrBase = ptrSram;
	for (j = 0; j < SRAM_SIZE;j++) {
		*ptrSram = j;//0x55555555;
		if (*ptrBase != 0) {
			fail++;
		}
		ptrSram++;
	}

	fail = 0;
	ptrSram = (unsigned int *)SRAM_BASE_ADDRESS;
	for (j = 0; j < SRAM_SIZE;j++) {
		i = *ptrSram;
		if (i != j) { //0x55555555) {
			fail++;
		}
		ptrSram++;
	}

	ptrSram = (unsigned int *)SRAM_BASE_ADDRESS;

	for (j = 0; j < SRAM_SIZE;j++) {
		*ptrSram = ~j;//0x55555555;
		ptrSram++;
	}

	ptrSram = (unsigned int *)SRAM_BASE_ADDRESS;
	for (j = 0; j < SRAM_SIZE;j++) {
		i = *ptrSram;
		if (i != ~j) { //0x55555555) {
			fail++;
		}
		ptrSram++;
	}
	if (fail) {
		PutStr("Memory ERROR\n");
	}
	else {
		PutStr("Memory OK\n");
	}
#endif
	return 0;
}
