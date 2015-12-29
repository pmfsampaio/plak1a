/*
 * ili9320.h
 *
 *  Created on: Jun 30, 2009
 *      Author: psampaio
 */

#ifndef ILI9320_H_
#define ILI9320_H_

//RRRRRGGGGGGBBBBB
#define BLACK              0x0000	//0000000000000000
#define BRIGHTBLUE1         0x001F	//0000000000011111
#define BRIGHTBLUE2         0x003F	//0000000000011111
#define BRIGHTGREEN        0x07E0	//0000011111100000
#define BRIGHTCYAN         0x07FF	//0000011111111111
#define BRIGHTRED          0xF800	//1111100000000000
#define BRIGHTMAGENTA      0xF81F	//1111100000011111
#define BRIGHTYELLOW       0xFFE0	//1111111111100000
#define BLUE               0x0010	//0000000000010000
#define GREEN              0x0400	//0000010000000000
#define CYAN               0x0410	//0000010000010000
#define RED                0x8000	//100000000000000
#define WHITE              0xFFFF


//0x8300 0000
//0x8300 0004 - RS = 1
#define LCD_INSTR			(*((volatile unsigned int *) 0x83000000))
#define LCD_DATA			(*((volatile unsigned int *) 0x83000004))

// Horizontal and vertical screen size
#define SCREEN_HOR_SIZE    240
#define SCREEN_VER_SIZE    320

#define LINE_MEM_PITCH     0x100

void GLcdWriteData(unsigned char byte1, unsigned char byte0);
void GLcdWriteCmd(unsigned char cmd);
void GLcdWriteRegister(unsigned int index, unsigned int value);
void GLcdInit(void);
void GLcdSetAddress(unsigned char addr2, unsigned char addr1, unsigned char addr0);
void GLcdClear(unsigned int color);
void GLcdRect(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int color);
void GLcdImage(short unsigned left, short unsigned top, const unsigned short bitmap[], unsigned char stretch);

#endif /* ILI9320_H_ */
