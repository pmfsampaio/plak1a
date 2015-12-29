/*
 * accelerometer.h
 *
 *  Created on: Jun 25, 2009
 *      Author: psampaio
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

void InitAccelerometer(void);
void LIS3LGetXYZ (short * xvalue, short * yvalue, short  * zvalue);

#endif /* ACCELEROMETER_H_ */
