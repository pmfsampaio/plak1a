#include <stdarg.h>
#include <stdio.h>
#include "lpc22xx.h"
#include "uart.h"
#include "startosc.h"

void UART0Initialize(unsigned int baud)
{
  unsigned int divisor = GetPclk() / (16 * (baud/4));

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  UART0_LCR = 0x83;
  //divisor
  UART0_DLL = divisor & 0xFF;
  UART0_DLM = (divisor >> 8) & 0xFF;
  UART0_LCR &= ~0x80;
  //set pins:  port0.0 -> TX0,  port0.1 -> RXD0
  PINSEL0 = (PINSEL0 & ~0xF) | 0x5;
}

void UART0WriteChar(unsigned char ch)
{
  //when U0LSR_THRE is 0 - U0THR contains valid data.
  while ((UART0_LSR & 0x20) == 0);
  UART0_THR = ch;
}

unsigned char UART0ReadChar(void)
{
  //when U0LSR_DR is 1 - U0RBR contains valid data
  while ((UART0_LSR & 0x01) == 0);
  return  UART0_RBR;
}

void UART0WriteStr(char *s)
{
	while (*s) {
		UART0WriteChar(*s++);
	}
}

void UART0Printf(char *format, ...) {
	va_list argptr;
	static char buffer[100];
	int len;

	va_start (argptr, format);
	len = vsprintf(buffer, format, argptr);
	UART0WriteStr(buffer);
	va_end(argptr);
}

void UART1Initialize(unsigned int baud)
{
  unsigned int divisor = GetPclk() / (16 * baud);

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  UART1_LCR = 0x83;
  //divisor
  UART1_DLL = divisor & 0xFF;
  UART1_DLM = (divisor >> 8) & 0xFF;
  UART1_LCR &= ~0x80;
  //set pins:  port0.0 -> TX0,  port0.1 -> RXD0
  PINSEL0 = (PINSEL0 & ~0xF) | 0x5;
}

void UART1WriteChar(unsigned char ch)
{
  //when U0LSR_THRE is 0 - U0THR contains valid data.
  while ((UART1_LSR & 0x20) == 0);
  UART1_THR = ch;
}

unsigned char UART1ReadChar(void)
{
  //when U0LSR_DR is 1 - U0RBR contains valid data
  while ((UART1_LSR & 0x01) == 0);
  return  UART1_RBR;
}

void UART1WriteStr(char *s)
{
	while (*s) {
		UART1WriteChar(*s++);
	}
}

void UART1Printf(char *format, ...) {
	va_list argptr;
	static char buffer[100];
	int len;

	va_start (argptr, format);
	len = vsprintf(buffer, format, argptr);
	UART1WriteStr(buffer);
	va_end(argptr);
}

