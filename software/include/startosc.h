/*
 * startosc.h
 *
 *  Created on: Jul 25, 2009
 *      Author: psampaio
 */

#ifndef STARTOSC_H_
#define STARTOSC_H_

#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz

void StartOsc(void) __attribute__((section(".startup")));
unsigned int GetCclk(void);
unsigned int GetPclk(void);

#endif /* STARTOSC_H_ */
