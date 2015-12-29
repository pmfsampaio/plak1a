/*
 * uart.c
 *
 *  Created on: Oct 30, 2009
 *      Author: psampaio
 */
#include "lpc22xx.h"
#include "uart.h"
#include "isr.h"

void uart0ISRH(void);
void uart1ISRH(void);

#define UART0_PINSEL       (0x00000005)    /* PINSEL0 Value for UART0 */
#define UART0_PINMASK      (0x0000000F)    /* PINSEL0 Mask for UART0 */

#define UART1_PINSEL       (0x00050000)    /* PINSEL0 Value for UART1 */
#define UART1_PINMASK      (0x000F0000)    /* PINSEL0 Mask for UART1 */


unsigned char  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
unsigned short uart0_rx_insert_idx, uart0_rx_extract_idx;

unsigned char  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
unsigned short uart0_tx_insert_idx, uart0_tx_extract_idx;
int      uart0_tx_running;

unsigned char  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
unsigned short uart1_rx_insert_idx, uart1_rx_extract_idx;

unsigned char  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
unsigned short uart1_tx_insert_idx, uart1_tx_extract_idx;
int      uart1_tx_running;

void uart0Init(unsigned short baud, unsigned char mode, unsigned char fmode)
{
  // set port pins for UART0
  PINSEL0 = (PINSEL0 & ~UART0_PINMASK) | UART0_PINSEL;

  UART0_IER = 0x00;                         // disable all interrupts
  UART0_IIR;                                // clear interrupt ID
  UART0_RBR;                                // clear receive register
  UART0_LSR;                                // clear line status register

  // set the baudrate
  UART0_LCR = ULCR_DLAB_ENABLE;             // select divisor latches
  UART0_DLL = (unsigned char)baud;                // set for baud low byte
  UART0_DLM = (unsigned char)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  UART0_LCR = (mode & ~ULCR_DLAB_ENABLE);
  UART0_FCR = fmode;

  // initialize the interrupt vector
  VICIntSelect &= ~VIC_BIT(VIC_UART0);  // UART0 selected as IRQ
  VICIntEnable = VIC_BIT(VIC_UART0);    // UART0 interrupt enabled
  VICVectCntl0 = VIC_ENABLE | VIC_UART0;
  VICVectAddr0 = (unsigned long)uart0ISRH;    // address of the ISR

  // initialize the transmit data queue
  uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
  uart0_tx_running = 0;

  // initialize the receive data queue
  uart0_rx_extract_idx = uart0_rx_insert_idx = 0;

  // enable receiver interrupts
  UART0_IER = UIER_ERBFI | UIER_ELSI;
}

int uart0Putch(int ch)
{
  unsigned short temp;
  unsigned cpsr;

  temp = (uart0_tx_insert_idx + 1) % UART0_TX_BUFFER_SIZE;

  if (temp == uart0_tx_extract_idx)
    return -1;                          // no room

  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart0_tx_running)
    {
    // add to queue
    uart0_tx_buffer[uart0_tx_insert_idx] = (unsigned char)ch;
    uart0_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart0_tx_running = 1;
    UART0_THR = (unsigned char)ch;
    }

  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER |= UIER_ETBEI;                  // enable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  return (unsigned char)ch;
}

unsigned short uart0Space(void)
{
  int space;

  if ((space = (uart0_tx_extract_idx - uart0_tx_insert_idx)) <= 0)
    space += UART0_TX_BUFFER_SIZE;

  return (unsigned short)(space - 1);
}

const char *uart0Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart0Putch(ch) >= 0))
    string++;

  return string;
}

int uart0Write(const char *buffer, unsigned short count)
{
  if (count > uart0Space())
    return -1;
  while (count && (uart0Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}

int uart0TxEmpty(void)
{
  return (UART0_LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

void uart0TxFlush(void)
{
  unsigned cpsr;

  UART0_FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  // "Empty" the transmit buffer.
  cpsr = disableIRQ();                  // disable global interrupts
  UART0_IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  uart0_tx_insert_idx = uart0_tx_extract_idx = 0;
}

int uart0Getch(void)
{
  unsigned char ch;

  if (uart0_rx_insert_idx == uart0_rx_extract_idx) // check if character is available
    return -1;

  ch = uart0_rx_buffer[uart0_rx_extract_idx++]; // get character, bump pointer
  uart0_rx_extract_idx %= UART0_RX_BUFFER_SIZE; // limit the pointer
  return ch;
}

void uart1Init(unsigned short baud, unsigned char mode, unsigned char fmode)
{
  // set port pins for UART1
  PINSEL0 = (PINSEL0 & ~UART1_PINMASK) | UART1_PINSEL;

  UART1_IER = 0x00;                         // disable all interrupts
  UART1_IIR;                                // clear interrupt ID
  UART1_RBR;                                // clear receive register
  UART1_LSR;                                // clear line status register

  // set the baudrate
  UART1_LCR = ULCR_DLAB_ENABLE;             // select divisor latches
  UART1_DLL = (unsigned char)baud;                // set for baud low byte
  UART1_DLM = (unsigned char)(baud >> 8);         // set for baud high byte

  // set the number of characters and other
  // user specified operating parameters
  UART1_LCR = (mode & ~ULCR_DLAB_ENABLE);
  UART1_FCR = fmode;

  // initialize the interrupt vector
  VICIntSelect &= ~VIC_BIT(VIC_UART1);  // UART1 selected as IRQ
  VICIntEnable = VIC_BIT(VIC_UART1);    // UART1 interrupt enabled
  VICVectCntl1 = VIC_ENABLE | VIC_UART1;
  VICVectAddr1 = (unsigned long)uart1ISRH;    // address of the ISR

  uart1_tx_extract_idx = uart1_tx_insert_idx = 0;
  uart1_tx_running = 0;

  // initialize data queues
  uart1_rx_extract_idx = uart1_rx_insert_idx = 0;

  // enable receiver interrupts
  UART1_IER |= UIER_ERBFI;
}

int uart1Putch(int ch)
{
  unsigned short temp;
  unsigned cpsr;

  temp = (uart1_tx_insert_idx + 1) % UART1_TX_BUFFER_SIZE;

  if (temp == uart1_tx_extract_idx)
    return -1;                          // no room

//  cpsr = disableIRQ();                  // disable global interrupts
  UART1_IER &= ~UIER_ETBEI;                 // disable TX interrupts
//  restoreIRQ(cpsr);                     // restore global interrupts

  // check if in process of sending data
  if (uart1_tx_running)
    {
    // add to queue
    uart1_tx_buffer[uart1_tx_insert_idx] = (unsigned char)ch;
    uart1_tx_insert_idx = temp;
    }
  else
    {
    // set running flag and write to output register
    uart1_tx_running = 1;
    UART1_THR = (unsigned char)ch;
    }

//  cpsr = disableIRQ();                  // disable global interrupts
  UART1_IER |= UIER_ETBEI;                  // enable TX interrupts
//  restoreIRQ(cpsr);                     // restore global interrupts
  return (unsigned char)ch;
}

unsigned short uart1Space(void)
{
  int space;

  if ((space = (uart1_tx_extract_idx - uart1_tx_insert_idx)) <= 0)
    space += UART1_TX_BUFFER_SIZE;

  return (unsigned short)(space - 1);
}

const char *uart1Puts(const char *string)
{
  register char ch;

  while ((ch = *string) && (uart1Putch(ch) >= 0))
    string++;

  return string;
}

int uart1Write(const char *buffer, unsigned short count)
{
  if (count > uart1Space())
    return -1;
  while (count && (uart1Putch(*buffer++) >= 0))
    count--;

  return (count ? -2 : 0);
}

int uart1TxEmpty(void)
{
  return (UART1_LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

void uart1TxFlush(void)
{
  unsigned cpsr;

  UART1_FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo

  // "Empty" the transmit buffer.
  cpsr = disableIRQ();                  // disable global interrupts
  UART1_IER &= ~UIER_ETBEI;                 // disable TX interrupts
  restoreIRQ(cpsr);                     // restore global interrupts
  uart1_tx_insert_idx = uart1_tx_extract_idx = 0;
}

int uart1Getch(void)
{
	unsigned char ch;

  if (uart1_rx_insert_idx == uart1_rx_extract_idx) // check if character is available
    return -1;

  ch = uart1_rx_buffer[uart1_rx_extract_idx++]; // get character, bump pointer
  uart1_rx_extract_idx %= UART1_RX_BUFFER_SIZE; // limit the pointer
  return ch;
}
