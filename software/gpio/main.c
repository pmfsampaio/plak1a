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

#define LCD_INSTR			(*((volatile unsigned int *) 0x83000000))
#define LCD_DATA			(*((volatile unsigned int *) 0x83000004))

int main(void)
{
	int j;

	PINSEL2 = 0x0F810924;
	BCFG3 = 0x20001c80;

	LCD_INSTR = 0;
	j = LCD_DATA;



	// set io pins for led P0.23
	IO0DIR |= (1 << 23);
	IO0SET =  (1 << 23);	// led off
	IO0CLR =  (1 << 23);	// led on

	while (1)
	{
		if (IO1PIN & (1 << 24)) {
			IO0SET = (1 << 23);
		}
		else {
			IO0CLR =(1 << 23);
		}
	}
	return 0;
}
