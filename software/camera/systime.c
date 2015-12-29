/*
 * systime.c
 *
 *  Created on: Oct 29, 2009
 *      Author: psampaio
 */

#include "lpc22xx.h"
#include "systime.h"

static unsigned int sysTICs;
static unsigned int lastT0TC;

void initSysTime(void)
{
  // setup Timer 1 to count forever
  TIMER0_TCR = TCR_RESET;                    // reset & disable timer 0
  TIMER0_PR  = T0_PCLK_DIV - 1;               // set the prescale divider
  TIMER0_MCR = 0;                            // disable match registers
  TIMER0_CCR = 0;                            // disable compare registers
  TIMER0_EMR = 0;                            // disable external match register
  TIMER0_TCR = TCR_ENABLE;                   // enable timer 0
  sysTICs = 0;
}

unsigned int getSysTICs(void)
{
  unsigned int now = TIMER0_TC;

  sysTICs += (unsigned int)(now - lastT0TC);
  lastT0TC = now;
  return sysTICs;
}

unsigned int getElapsedSysTICs(unsigned int startTime)
{
  return getSysTICs() - startTime;
}

void pause(unsigned int duration)
{
  unsigned int startTime = getSysTICs();

  while (getElapsedSysTICs(startTime) < duration)
    WDOG();
}
