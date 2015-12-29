/*
 * c328r.h
 *
 *  Created on: Oct 27, 2009
 *      Author: psampaio
 */

#ifndef C328R_H_
#define C328R_H_

#define GreyScale2 1
#define GreyScale4  2
#define GreyScale8  3
#define Color12  5
#define Color16  6
#define Jpeg  7

#define R80x64  1
#define R160x128  3
#define R320x240  5
#define R640x480  7

#define R_Snapshot  1
#define R_Preview  2
#define R_Jpeg 5

#define Compressed  0
#define Uncompressed  1

#define F50Hz  0
#define F60Hz  1

#define Baud7200	0
#define Baud9600	1
#define Baud14400	2
#define Baud19200	3
#define Baud28800	4
#define Baud38400	5
#define Baud57600	6
#define Baud115200	7

#define CMD_PREFIX  0xAA
#define CMD_INITIAL  0x01
#define CMD_GETPICTURE  0x04
#define CMD_SNAPSHOT  0x05
#define CMD_PACKAGESIZE  0x06

#define CMD_BAUDRATE  0x07
#define CMD_RESET  0x08
#define CMD_POWEROFF  0x09

#define CMD_DATA  0x0A
#define CMD_SYNC  0x0D
#define CMD_ACK  0x0E

#define CMD_NAK  0x0F
#define CMD_LIGHTFREQ  0x13

#define PACKAGE_SIZE	512


void C328RInit(void);

int Sync(void);
int Initial(unsigned char colorType, unsigned char previewResolution,
		unsigned char resolution);
int GetRawPicture(unsigned char pictureType, unsigned char *dataBuffer,
		int processDelay);
int GetJpegPicture(int pictureType, unsigned char *dataBuffer, int processDelay);
int Snapshot(int snapshotType, int skipFrameCounter);
int LightFrequency(int lightFrequency);
int SetBaudRate(int baudRate);
int Reset(int completeReset);
int ResetSpecial(void);
int PowerOff(void);

#endif /* C328R_H_ */
