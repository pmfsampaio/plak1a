#include "lpc22xx.h"
#include "touchscreen.h"

#define	X_DEFAULT_RESOLUTION	320
#define	Y_DEFAULT_RESOLUTION	240
#define	P_DEFAULT_RESOLUTION	100

#define	TS_NUM_SAMPLES		10
#define	TS_SETTLING_DELAY	10000

#define	TS_RESOLUTION_BITS	12

#define TS_Y_CHANNEL	1
#define TS_X_CHANNEL	3

#define YU		(1 << 20)
#define XL		(1 << 21)
#define YD		(1 << 22)
#define XR		(1 << 23)

#define ENABLE_I2C_AA ( I2C_I2CONSET |= (0x1 << 2))
#define DISABLE_I2C_AA ( I2C_I2CONCLR = (0x1 << 2))
//#define I2C_BIT_RATE(rate) (I2SCLH = I2SCLL = ((unsigned int)(PERIPH_FREQ/2/rate)+10))
#define I2C_SEND_START (I2C_I2CONSET |= (0x1 << 5))
#define I2C_SEND_STOP (I2C_I2CONSET = (0x1 << 4))
#define I2C_CLR_START (I2C_I2CONCLR = (0x1 << 5))
#define I2C_CLR_SI (I2C_I2CONCLR = (0x1 << 3))

int i2c_bytesRemaining;
int i2c_pos;
unsigned char i2c_buffer[10];
unsigned char i2c_rxBuffer[19];
unsigned char i2c_RdnWr;
int i2c_rxWrPos, i2c_rxComplete;

void i2c_transfer(void) {
	unsigned char code;
	int i2c_rxBytesRemaining = 2* 4 ;
	unsigned char transferComplete = 0;
	i2c_pos = 0;
	i2c_rxWrPos = 0;
	i2c_bytesRemaining = 1;

	I2C_SEND_START;

	while (!transferComplete) {
		code = I2C_I2STAT;
		switch (code) {
		/***** MASTER TX MODE *****/
		case 0x08: // First Start Transmitted
		case 0x10: // Repeated Start
			// Write address and tell if it is read or write
			I2C_I2DAT = 0x28 << 1 | 0x01; //(i2c_address << 1) | i2c_RdnWr;
			I2C_I2CONCLR = (0x1 << 3); // Clear SI
			break;
		case 0x18: // Start + Wr sent, ACK received
		case 0x20: // Start + Wr sent, Not ACK received
		case 0x28: // Data byte sent, ACK recieved
		case 0x30: // Data byte sent, Not Ack received
			if (i2c_bytesRemaining > 0) {
				I2C_I2DAT = i2c_buffer[i2c_pos++];
				i2c_bytesRemaining--;
				I2C_I2CONCLR = (0x1 << 5);
			} else if (i2c_rxBytesRemaining == 0) {
				I2C_SEND_STOP; // Send Stop
				I2C_I2CONCLR = (0x1 << 5);
				transferComplete = 1;
			} else { // Change from master_tx to master_rx
				i2c_RdnWr = (~i2c_RdnWr) & 0x1;
				i2c_pos = 0;
				i2c_rxWrPos = 0;
				i2c_rxComplete = 0;
				I2C_SEND_START;
			}
			I2C_I2CONCLR = (0x1 << 3); // Clear SI
			break;
		case 0x38: // Arbitration Lost, try again
			I2C_SEND_START;
			I2C_I2CONCLR = (0x1 << 3); // Clear SI
			break;
			/***** MASTER RX MODE *****/
		case 0x40: // SLA+R HAS BEEN SENT; ACK RECEIVED
			if (i2c_rxBytesRemaining != 0)
				ENABLE_I2C_AA;
			else
				DISABLE_I2C_AA;
			I2C_I2CONCLR = (0x1 << 3) | (0x1 << 5); // Clear SI and START
			break;
		case 0x48: // SLA+R has been send; not ack received
			// Try again
			I2C_SEND_START;
			I2C_CLR_SI;
			break;

		case 0x50: // DATA BYTE RECEIVED, ACK RETURNED
			i2c_rxBuffer[i2c_rxWrPos] = I2C_I2DAT;
			i2c_rxWrPos++;
			i2c_rxBytesRemaining--;
			if (i2c_rxBytesRemaining != 0)
				ENABLE_I2C_AA;
			else
				DISABLE_I2C_AA;
			I2C_I2CONCLR = (0x1 << 3) | (0x1 << 5); // Clear SI and START
			break;
		case 0x58: // DATA BYTE RECEIVED; NOT ACT RETURNED
			i2c_rxComplete = 1; // Done receiving all requested data
			transferComplete = 1;
			I2C_CLR_START;
			I2C_SEND_STOP;
			I2C_CLR_SI;
			break;
		}
	}
}

unsigned int GetADC(int channel) {
	int i, j, ch;

	i2c_transfer();
	for (i = 0; i < 8; i += 2) {
		j = i2c_rxBuffer[i] << 8 | i2c_rxBuffer[i + 1];
		ch = (0x3000 & j) >> 12;
		if (ch == channel) {
			return (j & 0x0FFC);
		}
	}
	return -1;
}

void polarizeXx(void) {
	IO1DIR |= (XL | XR);
	IO1DIR &= ~(YU | YD);

	IO1SET |= XL;
	IO1CLR |= XR;
}

void polarizexX(void) {
	IO1DIR |= (XL | XR);
	IO1DIR &= ~(YU | YD);

	IO1CLR |= XL;
	IO1SET |= XR;
}

void polarizeYy(void) {
	IO1DIR &= (XL | XR);
	IO1DIR |= ~(YU | YD);

	IO1SET |= YU;
	IO1CLR |= YD;
}

void polarizeyY(void) {
	IO1DIR &= (XL | XR);
	IO1DIR |= ~(YU | YD);

	IO1CLR |= YU;
	IO1SET |= YD;
}


touchscreen_data res_data;

void InitTS(touchscreen_data ts_resolution) {
	PINSEL0 |= 0x0050;
	I2C_I2CONSET |= 0x40;
	I2C_I2SCLH = 25;
	I2C_I2SCLL = 25;

	if (ts_resolution.xvalue != 0) {
		res_data.xvalue = ts_resolution.xvalue;
	} else {
		res_data.xvalue = X_DEFAULT_RESOLUTION;
	}

	if (ts_resolution.yvalue != 0) {
		res_data.yvalue = ts_resolution.yvalue;
	} else {
		res_data.yvalue = Y_DEFAULT_RESOLUTION;
	}

	if (ts_resolution.pvalue != 0) {
		res_data.pvalue = ts_resolution.pvalue;
	} else {
		res_data.pvalue = P_DEFAULT_RESOLUTION;
	}

}

void ts_settling_delay(void) {
	int i;
	for (i = TS_SETTLING_DELAY; i; i--)
		;
}

touchscreen_data GetTS(void) {

#define XDELTA_MAX	100
#define	YDELTA_MAX	100
	touchscreen_data samples[TS_NUM_SAMPLES * 2];
	/*static*//*touchscreen_data ts_result_p, ts_result_n, ts_result;*/
	touchscreen_data ts_result;
	int i;
	int yvalue_min = 4095;//1023;
	int yvalue_max = 0;
	int xvalue_min = 4095;//1023;
	int xvalue_max = 0;
	int xdelta, ydelta;
	int ts_max_value = 2 << (TS_RESOLUTION_BITS - 1);

	ts_settling_delay();
	ts_settling_delay();
	ts_settling_delay();

	for (i = 0; i < TS_NUM_SAMPLES; i++) {
		polarizeXx();
//		InitADC(TS_Y_CHANNEL);
		ts_settling_delay();
		samples[i * 2].xvalue = GetADC(TS_Y_CHANNEL);
		/*samples[i*2+1].xvalue = ts_max_value - samples[i*2].xvalue;*/

		polarizeYy();
//		InitADC(TS_X_CHANNEL);
		ts_settling_delay();
		samples[i * 2].yvalue = GetADC(TS_X_CHANNEL);

		samples[i * 2].pvalue = 0;

		polarizexX();
//		InitADC(TS_Y_CHANNEL);
		ts_settling_delay();
		samples[(i * 2) + 1].xvalue = GetADC(TS_Y_CHANNEL);
		/*samples[i*2].xvalue = ts_max_value - samples[i*2+1].xvalue;*/

		polarizeyY();
//		InitADC(TS_X_CHANNEL);
		ts_settling_delay();
		samples[(i * 2) + 1].yvalue = GetADC(TS_X_CHANNEL);

		samples[(i * 2) + 1].pvalue = 0;

	}

	/*	ts_result_p.xvalue = 0;
	 ts_result_p.yvalue = 0;
	 ts_result_p.pvalue = 0;
	 ts_result_n.xvalue = 0;
	 ts_result_n.yvalue = 0;
	 ts_result_n.pvalue = 0;
	 */

	ts_result.xvalue = 0;
	ts_result.yvalue = 0;
	ts_result.pvalue = 0;

	for (i = 0; i < (TS_NUM_SAMPLES); i++) {
		int tempyval = ts_max_value - samples[i * 2 + 1].yvalue;
		int tempxval = ts_max_value - samples[i * 2 + 1].xvalue;

		ts_result.xvalue += samples[i * 2].xvalue;
		ts_result.yvalue += samples[i * 2].yvalue;
		/*ts_result.pvalue += samples[i*2].pvalue;*/

		ts_result.xvalue += ((ts_max_value) - samples[(i * 2) + 1].xvalue);
		ts_result.yvalue += ((ts_max_value) - samples[(i * 2) + 1].yvalue);

		if (yvalue_min > samples[i * 2].yvalue)
			yvalue_min = samples[i * 2].yvalue;
		if (yvalue_max < samples[i * 2].yvalue)
			yvalue_max = samples[i * 2].yvalue;

		if (yvalue_min > tempyval)
			yvalue_min = tempyval;
		if (yvalue_min < tempyval)
			yvalue_max = tempyval;

		if (xvalue_min > samples[i * 2].xvalue)
			xvalue_min = samples[i * 2].xvalue;
		if (xvalue_max < samples[i * 2].xvalue)
			xvalue_max = samples[i * 2].xvalue;

		if (xvalue_min > tempxval)
			xvalue_min = tempxval;
		if (xvalue_min < tempxval)
			xvalue_max = tempxval;
	}

	ts_result.xvalue = ts_result.xvalue / (TS_NUM_SAMPLES * 2);
	ts_result.yvalue = ts_result.yvalue / (TS_NUM_SAMPLES * 2);

	xdelta = xvalue_max - xvalue_min;
	ydelta = yvalue_max - yvalue_min;

	if ((ts_result.xvalue < 910) && (ts_result.yvalue < 910) && (xdelta
			< XDELTA_MAX) && (ydelta < YDELTA_MAX)) {
		ts_result.pvalue = 1024;
	} else {
		ts_result.pvalue = 0;
	}

	ts_result.yvalue = ts_max_value - ts_result.yvalue;

	return ts_result;

}
