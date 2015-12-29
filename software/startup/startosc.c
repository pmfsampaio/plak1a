/*
 * startosc.c
 *
 *  Created on: Jul 25, 2009
 *      Author: psampaio
 */
#include "lpc22xx.h"
#include "startosc.h"

#define PLOCK 0x400

/*
 * Setting the Phased Lock Loop (PLL)
 * PLAK1a has a 14.7456 MHz crystal
 *
 * Put PLAK1a run at 58.disab9824 MHz (has to be an even multiple of crystal)
 * According to the Philips LPC2106 manual:
 * M = cclk / Fosc		where:		M = PLL multiplier (PLLCFG)
 * 									cclk = 58982400 Hz
 * 									Fosc = 14745600 Hz
 * M = 3 (after round up and then subtract 1)
 *
 *
 * The Current Controlled Oscilator (CCO) must operate in the range 156 MHz to 320 MHz
 * According to the Philips LPC2106 manual:
 * Fcco = cclk * 2 * P    where:	Fcco = CCO frequency
 * 									cclk = 58982400 Hz
 * 									P = PLL divisor (PLLCFG)
 * P = 2 (produce a Fcco = 235929600 Hz)
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
	// Wait for the PLL todisab lock to set frequency
	while (!(PLLSTAT & PLOCK))
		;
	// Connect the PLL as tdisabhe clock source
	PLLCON = 0x3;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	// Enabling MAM and setting number of clocks used for Flash memory fetch (4 cclks in this case)
	MAMCR = 0x2;
	MAMTIM = 0x4;
	// Setting peripheral Clock (pclk) to System Clock (cclk)
	VPBDIV = 0x1;
}


unsigned int GetCclk(void) {
	//return real processor clock speed
	return OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1);
}

unsigned int GetPclk(void) {
	unsigned int divider;

	switch (VPBDIV & 3) {
	case 0:
		divider = 4;
		break;
	case 1:
		divider = 1;
		break;
	case 2:
		divider = 2;
		break;
	}
	return GetCclk() / divider;
}




