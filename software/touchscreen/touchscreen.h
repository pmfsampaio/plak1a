/*
 * touchscreen.h
 *
 *  Created on: Jun 25, 2009
 *      Author: psampaio
 */

#ifndef TOUCHSCREEN_H_
#define TOUCHSCREEN_H_

typedef struct touchscreen_data_t {
	unsigned long xvalue;
	unsigned long yvalue;
	unsigned long pvalue;
} touchscreen_data;

touchscreen_data GetTS(void);
void InitTS(touchscreen_data ts_resolution);

#endif /* TOUCHSCREEN_H_ */
