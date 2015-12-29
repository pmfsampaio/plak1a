#include <stdio.h>
#include <string.h>
#include "lpc22xx.h"
#include "startosc.h"
#include "ili9320.h"

void Delay(int j) {
	int i;
	for (i = 0; i < 4000 * j; i++)
		;
}

extern const unsigned short Subaru_logo_320x240[];
//extern const unsigned short filhos[];
//extern const unsigned short len_full[];

	// Set Data bus functionality
	// 0000 1111 1000 0001 0100 1001 0010 0100
//	PINSEL2 = 0x0F810924;

int main()
{
	GLcdInit();

	for (;;) {
		GLcdClear(WHITE | 0x30000);
		GLcdClearx(WHITE | 0x30000);
//		GLcdImage(0, 0, xx, 1);

//		GLcdImage(0, 0, len_full, 1);
//		Delay(1000);
		GLcdClear(WHITE & 0xffff);
//		GLcdClearx(WHITE & 0xffff);

		GLcdClear(0xF800 & 0xffff);
//		GLcdClearx(0xF800 & 0xffff);

		GLcdClear(0x001F & 0xffff);
//		GLcdClearx(0x001F & 0xffff);

		GLcdClear(0x07e0 & 0xffff);
//		GLcdClearx(0x07e0 & 0xffff);

		GLcdClear(0 & 0xffff);
//		GLcdClearx(0 & 0xffff);


		GLcdImage(0, 0, Subaru_logo_320x240, 1);
		Delay(10);
		Delay(1000);
		GLcdClear(BRIGHTBLUE1);
		Delay(1000);
		GLcdClear(BRIGHTBLUE2);
		Delay(1000);
		GLcdClear(BLUE);
		Delay(1000);
		GLcdClear(BRIGHTGREEN);
		Delay(1000);
		GLcdClear(BRIGHTCYAN);
		Delay(1000);
		GLcdClear(BRIGHTRED);
		Delay(1000);
		GLcdClear(BRIGHTMAGENTA);
		Delay(1000);
		GLcdClear(BRIGHTYELLOW);
		Delay(1000);
		GLcdClear(WHITE);
		Delay(1000);
		GLcdRect(10, 10, 100, 100, BRIGHTYELLOW);
		Delay(1000);
		GLcdClear(BLACK);
		Delay(1000);
	}
}
