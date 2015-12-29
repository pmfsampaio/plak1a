/*
 * systime.h
 *
 *  Created on: Oct 29, 2009
 *      Author: psampaio
 */

#ifndef SYSTIME_H_
#define SYSTIME_H_

// setup parameters
#define T0_PCLK_DIV     3
#define sysTICSperSEC   (PCLK / T0_PCLK_DIV)

// some helpful times for pause()
#define ONE_MS          (unsigned int)((  1e-3 * sysTICSperSEC) + .5)
#define TWO_MS          (unsigned int)((  2e-3 * sysTICSperSEC) + .5)
#define FIVE_MS         (unsigned int)((  5e-3 * sysTICSperSEC) + .5)
#define TEN_MS          (unsigned int)(( 10e-3 * sysTICSperSEC) + .5)
#define TWENTY_MS       (unsigned int)(( 20e-3 * sysTICSperSEC) + .5)
#define THIRTY_MS       (unsigned int)(( 30e-3 * sysTICSperSEC) + .5)
#define FIFTY_MS        (unsigned int)(( 50e-3 * sysTICSperSEC) + .5)
#define HUNDRED_MS      (unsigned int)((100e-3 * sysTICSperSEC) + .5)
#define ONE_FIFTY_MS    (unsigned int)((150e-3 * sysTICSperSEC) + .5)
#define QUARTER_SEC     (unsigned int)((250e-3 * sysTICSperSEC) + .5)
#define HALF_SEC        (unsigned int)((500e-3 * sysTICSperSEC) + .5)
#define ONE_SEC         (unsigned int)(( 1.0   * sysTICSperSEC) + .5)
#define TWO_SEC         (unsigned int)(( 2.0   * sysTICSperSEC) + .5)
#define FIVE_SEC        (unsigned int)(( 5.0   * sysTICSperSEC) + .5)
#define TEN_SEC         (unsigned int)((10.0   * sysTICSperSEC) + .5)

void initSysTime(void);
unsigned int getSysTICs(void);
unsigned int getElapsedSysTICs(unsigned int startTime);
void pause(unsigned int duration);



#endif /* SYSTIME_H_ */
