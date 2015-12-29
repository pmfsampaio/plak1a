#ifndef _UART_H
#define _UART_H

#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz

//get real processor clock frequency
unsigned int processorClockFrequency(void);
//get peripheral clock frequency
unsigned int peripheralClockFrequency(void);

/**** UART0 ****/
//initialize UART0 interface
void UART0Initialize(unsigned int baud);
//write char to UART0 (RS232);
void UART0WriteChar(unsigned char ch0);
//read char from RS232
unsigned char UART0ReadChar(void);

// write formated to UART0;
void UART0Printf(char *format, ...);

//this function read/write char from RS232,
//but they not wait to read/write
unsigned char UART0ReadChar_nostop(void);
void UART0WriteChar_nostop(unsigned char ch0);


/**** UART1 ****/
//initialize UART1 interface
void UART1Initialize(unsigned int baud);
//write char to UART1 (RS232);
void UART1WriteChar(unsigned char ch0);
//read char from RS232
unsigned char UART1ReadChar(void);

//this function read/write char from RS232,
//but they not wait to read/write
unsigned char UART1ReadChar_nostop(void);
void UART1WriteChar_nostop(unsigned char ch0);

// write formated to UART0;
void UART1Printf(char *format, ...);

#define ConsoleWriteChar(_c)	UART1WriteChar(_c)
#define ConsolePrintf			UART1Printf

#endif
