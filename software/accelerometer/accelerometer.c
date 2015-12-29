#include "lpc22xx.h"
#include "accelerometer.h"

#define		LIS3L					0x3A
#define		LIS3L_AUTOINC			0x80
#define 	LIS3L_CTRL_REG1			0x20
#define 	LIS3L_WHO_AM_I			0x0F
#define 	LIS3L_CTRL_REG1			0x20
#define 	LIS3L_CTRL_REG2			0x21
#define 	LIS3L_CTRL_REG3			0x22

#define		LIS3L_OUTX_L			0x28
#define		LIS3L_OUTX_H			0x29
#define		LIS3L_OUTY_L			0x2A
#define		LIS3L_OUTY_H			0x2B
#define		LIS3L_OUTZ_L			0x2C
#define		LIS3L_OUTZ_H			0x2D

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

int i2c_rxWrPos, i2c_rxComplete;

void i2c_transfer(int len, int rx) {
	unsigned char code;
	int i2c_rxBytesRemaining = rx;
	unsigned char transferComplete = 0;
	unsigned char i2c_RdnWr = 0;
	i2c_pos = 0;
	i2c_rxWrPos = 0;
	i2c_bytesRemaining = len;

	I2C_SEND_START;

	while (!transferComplete) {
		code = I2C_I2STAT;
		switch (code) {
		/***** MASTER TX MODE *****/
		case 0x08: // First Start Transmitted
		case 0x10: // Repeated Start
			// Write address and tell if it is read or write
			I2C_I2DAT = LIS3L | i2c_RdnWr;
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

void LIS3LEnable() {
	i2c_buffer[0] = LIS3L_CTRL_REG1;
	i2c_buffer[1] = 0xC7;
	i2c_transfer(2, 0);
}

void LIS3LWhoAmI() {
	int j;

	i2c_buffer[0] = LIS3L_WHO_AM_I;
	i2c_transfer(1, 1);
	j = i2c_rxBuffer[0];
}

void LIS3LGetXYZ (short * xvalue, short * yvalue, short  * zvalue)
{
	i2c_buffer[0] = LIS3L_OUTX_L | LIS3L_AUTOINC;
	i2c_transfer(1,6);
	*xvalue = i2c_rxBuffer[0];  				// read low byte from slave
	*xvalue += i2c_rxBuffer[1] << 8;			// read high byte from slave
	*yvalue = i2c_rxBuffer[2];					// read low byte from slave
	*yvalue += (i2c_rxBuffer[3] << 8);			// read high byte from slave
	*zvalue = i2c_rxBuffer[4];					// read low byte from slave
	*zvalue += (i2c_rxBuffer[5] << 8);			// read high byte from slave
}


void InitAccelerometer(void) {
	PINSEL0 |= 0x0050;
	I2C_I2CONSET |= 0x40;
	I2C_I2SCLH = 25;
	I2C_I2SCLL = 25;
	LIS3LEnable();
	LIS3LWhoAmI();
}

