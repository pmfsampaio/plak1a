/*
 * c328r.c
 *
 *  Created on: Oct 27, 2009
 *      Author: psampaio
 */
#include <stdio.h>
#include "lpc22xx.h"
#include "uart.h"
#include "systime.h"
#include "c328r.h"

void SendCommand(unsigned char *commandArray, unsigned int len) {
	uart0Write((char *) commandArray, len);
//	pause(HUNDRED_MS);
}

int WaitForResponse(unsigned char *readBuffer, int len) {
	int ch, res;
	unsigned long startTime;

	res = 0;
	while (len) {
		startTime = getSysTICs();
		do {
			if ((ch = uart0Getch()) >= 0)
				break;
		} while (getElapsedSysTICs(startTime) < ONE_SEC);
		if (ch == -1)
			return res;
		*readBuffer++ = ch;
		res++;
		len--;
	}
	return res;
}

// Error codes:
// Picture Type Error      01h   Parameter Error                   0Bh
// Picture Up Scale        02h   Send Register Timeout             0Ch
// Picture Scale Error     03h   Command ID Error                  0Dh
// Unexpected Reply        04h   Picture Not Ready                 0Fh
// Send Picture Timeout    05h   Transfer Package Number Error     10h
// Unexpected Command      06h   Set Transfer Package Size Wrong   11h
// SRAM JPEG Type Error    07h   Command Header Error              F0h
// SRAM JPEG Size Error    08h   Command Length Error              F1h
// Picture Format Error    09h   Send Picture Error                F5h
// Picture Size Error      0Ah   Send Command Error                FFh
int ReceiveACK(unsigned char expectedACKCommand) {
	unsigned char responseBuffer[6];
	int stat;

	stat = WaitForResponse(responseBuffer, 6);
	if ((stat == 6) && (responseBuffer[1] == CMD_ACK) && (responseBuffer[2]
			== expectedACKCommand))
		return 0;
	return -1;
}

void SendACK(int commandId, int packageId) {
	unsigned char ackCommand[] = { CMD_PREFIX, CMD_ACK, 0, 0, 0, 0 };

	ackCommand[2] = (unsigned char) commandId;
	ackCommand[4] = (unsigned char) packageId;
	ackCommand[5] = (((unsigned char) packageId) >> 8);
	SendCommand(ackCommand, sizeof(ackCommand));
}

int Sync(void) {
	unsigned char recvCommand[6 * 2];
	unsigned char command[] = { CMD_PREFIX, CMD_SYNC, 0, 0, 0, 0 };
	int i;
	int stat = 0;

	for (i = 0; i < 60; i++) {
		SendCommand(command, sizeof(command));
		pause(HUNDRED_MS);
		// Wait for ACK
		stat = WaitForResponse(recvCommand, 6);
		if ((stat == 6) && (recvCommand[1] == CMD_ACK) && (recvCommand[2]
				== CMD_SYNC)) {
			// Wait for SYNC
			pause(HUNDRED_MS);
			stat = WaitForResponse(recvCommand, 6);
			if ((stat == 6) && (recvCommand[1] == CMD_SYNC)) {
				SendACK(CMD_SYNC, 0x00);
				return i;
			}
		}
		uart1Putch('#');
	}
	return -1;
}

int Initial(unsigned char colorType, unsigned char previewResolution,
		unsigned char resolution) {
	unsigned char command[] = { CMD_PREFIX, CMD_INITIAL, 0, 0, 0, 0 };

	command[3] = colorType;
	command[4] = previewResolution;
	command[5] = resolution;

	SendCommand(command, sizeof(command));
	pause(HUNDRED_MS);
	// Wait for ACK
	return ReceiveACK(CMD_INITIAL);
/*	if (ReceiveACK(CMD_INITIAL) != 0)
	return -1;

	// Create package size command
	command[1] = CMD_PACKAGESIZE;
	command[2] = 0x08;
	command[3] = (unsigned char) PACKAGE_SIZE; // PACKAGE_SIZE Low byte
	command[4] = (unsigned char) (PACKAGE_SIZE >> 8); // PACKAGE_SIZE High byte
	command[5] = 0x00;

	// Send 'Set Package Size' command
	SendCommand(command, sizeof(command));

	// Wait for ACK
	return ReceiveACK(CMD_PACKAGESIZE);
*/
}

int GetPictureCommand(unsigned char pictureType, int processDelay,
		int *pictureDataSize) {
	unsigned char command[] = { CMD_PREFIX, CMD_GETPICTURE, 0, 0, 0, 0 };
	unsigned char response[6];
	int stat;

	command[2] = pictureType;
	*pictureDataSize = 0;
	SendCommand(command, sizeof(command));
	pause(processDelay);
	if (ReceiveACK(CMD_GETPICTURE) != 0)
		return -1;
	// Receive DATA command, with informations about image
	stat = WaitForResponse(response, 6); //500
	if ((stat != 6) || (response[1] != CMD_DATA))
		return -1;
	// Get dataSize from three bytes

	stat = response[5] << 8;
	stat |= response[4] << 8;
	stat |= response[3];
	*pictureDataSize = stat;
	return 0;
}

int GetRawPicture(unsigned char pictureType, unsigned char *dataBuffer,
		int processDelay) {
	int dataSize, res, res2;
	char s[80];

	if (GetPictureCommand(pictureType, processDelay, &dataSize) != 0) {
		uart1Puts("GetPicture: Error\n\r");
		return -1;
	}
	sprintf(s, "Data size:%d\n\r", dataSize);
	uart1Puts(s);
	if ((res = WaitForResponse(dataBuffer, dataSize)) != dataSize) {
		res2 = WaitForResponse(dataBuffer + res, dataSize - res);
		sprintf(s, "Data size not match (%d, %d)\n\r", res, res2);
		SendACK(CMD_DATA, 0x00);
		uart1Puts(s);
		return res;
	}
	SendACK(CMD_DATA, 0x00);
	return dataSize;
}

int GetJpegPicture(int pictureType, unsigned char *dataBuffer, int processDelay) {
	int dataSize, stat;
	int bufferPosition = 0;
	int packageCounter = 0;
	int errorCounter = 0;
	char response[PACKAGE_SIZE];
	char s[80];

	// Send 'Get Picture' command
	if (GetPictureCommand(pictureType, processDelay, &dataSize) != 0) {
		uart1Puts("GetPicture: Error\n\r");
		return -1;
	}
	sprintf(s, "Data size:%d\n\r", dataSize);
	uart1Puts(s);
	// Loop to read all data
	while (bufferPosition < dataSize && errorCounter <= 15) {
		SendACK(0, packageCounter);
		pause(TEN_MS);
		//                Thread.Sleep(40);

		// Wait for data package
		stat = WaitForResponse(&response, PACKAGE_SIZE); //1000

		// If data package received process it else increase error counter
		if (stat == PACKAGE_SIZE) {
			// Get data size in packet
			int packetSize = response[3] << 8;
			packetSize |= response[2];

			// Copy data from packet into data buffer
			memcpy(&dataBuffer[bufferPosition], &response[4], packetSize);
			//                    		Array.Copy(response, 4, dataBuffer, bufferPosition, packetSize);

			// Move buffer position and get ready for next package
			bufferPosition += packetSize;
			packageCounter++;
		} else
			errorCounter++;
	}

	// Send final package ACK
	SendACK(0, packageCounter);

	return dataSize;
}

int Snapshot(int snapshotType, int skipFrameCounter) {
	unsigned char command[] = { CMD_PREFIX, CMD_SNAPSHOT, 0x00, 0x00, 0x00,
			0x00 };

	command[2] = (unsigned char) snapshotType;
	command[3] = (unsigned char) (skipFrameCounter);
	command[4] = (unsigned char) (skipFrameCounter >> 8);
	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_SNAPSHOT);
}

int LightFrequency(int lightFrequency) {
	unsigned char command[] = { CMD_PREFIX, CMD_LIGHTFREQ, 0x00, 0x00, 0x00,
			0x00 };

	command[2] = lightFrequency;
	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_LIGHTFREQ);
}

int SetBaudRate(int baudRate) {
	int c;
	unsigned char command[] = { CMD_PREFIX, CMD_BAUDRATE, 0x00, 0x01, 0x00,
			0x00 };

	switch (baudRate) {
	case Baud7200:
		command[2] = 0xFF;
		break;
	case Baud9600:
		command[2] = 0xBF;
		break;
	case Baud14400:
		command[2] = 0x7F;
		break;
	case Baud19200:
		command[2] = 0x5F;
		break;
	case Baud28800:
		command[2] = 0x3F;
		break;
	case Baud38400:
		command[2] = 0x2F;
		break;
	case Baud57600:
		command[2] = 0x1F;
		break;
	case Baud115200:
		command[2] = 0x0F;
		break;
	default:
		command[2] = 0xBF;
		break;
	}
	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_BAUDRATE);
}

int Reset(int completeReset) {
	unsigned char command[] = { CMD_PREFIX, CMD_RESET, 0x00, 0x00, 0x00, 0x00 };

	command[2] = (completeReset) ? 0x00 : 0x01;
	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_RESET);
}

int ResetSpecial(void) {
	unsigned char command[] = { CMD_PREFIX, CMD_RESET, 0x00, 0x00, 0x00, 0xff };

	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_RESET);
}

int PowerOff(void) {
	unsigned char command[] = { CMD_PREFIX, CMD_POWEROFF, 0x00, 0x00, 0x00,
			0x00 };

	SendCommand(command, sizeof(command));
	return ReceiveACK(CMD_POWEROFF);
}

