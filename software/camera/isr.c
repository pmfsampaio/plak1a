/*
 * isr.c
 *
 *  Created on: Oct 29, 2009
 *      Author: psampaio
 */

#include "lpc22xx.h"
#include "uart.h"
#include "vic.h"

extern unsigned char  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
extern unsigned short uart0_rx_insert_idx, uart0_rx_extract_idx;

extern unsigned char  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
extern unsigned short uart0_tx_insert_idx, uart0_tx_extract_idx;
extern int      uart0_tx_running;
int error;

void uart0ISR(void)
{
  unsigned char iid;

  // loop until not more interrupt sources
  while (((iid = UART0_IIR) & UIIR_NO_INT) == 0)
    {
    // identify & process the highest priority interrupt
    switch (iid & UIIR_ID_MASK)
      {
      case UIIR_RLS_INT:                // Receive Line Status
        UART0_LSR;                          // read LSR to clear
        break;

      case UIIR_CTI_INT:                // Character Timeout Indicator
      case UIIR_RDA_INT:                // Receive Data Available
        do
          {
          unsigned short temp;

          // calc next insert index & store character
          temp = (uart0_rx_insert_idx + 1) % UART0_RX_BUFFER_SIZE;
          uart0_rx_buffer[uart0_rx_insert_idx] = UART0_RBR;

          // check for more room in queue
          if (temp != uart0_rx_extract_idx)
            uart0_rx_insert_idx = temp; // update insert index
          }
        while (UART0_LSR & ULSR_RDR);

        break;

      case UIIR_THRE_INT:               // Transmit Holding Register Empty
        while (UART0_LSR & ULSR_THRE)
          {
          // check if more data to send
          if (uart0_tx_insert_idx != uart0_tx_extract_idx)
            {
            UART0_THR = uart0_tx_buffer[uart0_tx_extract_idx++];
            uart0_tx_extract_idx %= UART0_TX_BUFFER_SIZE;
            }
          else
            {
            // no
            uart0_tx_running = 0;       // clear running flag
            break;
            }
          }

        break;

      default:                          // Unknown
    	  error++;
    	  UART0_LSR;
        UART0_RBR;
        break;
      }
    }

  VICVectAddr = 0x00000000;             // clear this interrupt from the VIC
}

extern unsigned char  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
extern unsigned short uart1_rx_insert_idx, uart1_rx_extract_idx;

extern unsigned char  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
extern unsigned short uart1_tx_insert_idx, uart1_tx_extract_idx;
extern int      uart1_tx_running;

void uart1ISR(void)
{
  unsigned char iid;

  // loop until not more interrupt sources
  while (((iid = UART1_IIR) & UIIR_NO_INT) == 0)
    {
    // identify & process the highest priority interrupt
    switch (iid & UIIR_ID_MASK)
      {
      case UIIR_RLS_INT:                // Receive Line Status
        UART1_LSR;                          // read LSR to clear
        break;

      case UIIR_CTI_INT:                // Character Timeout Indicator
      case UIIR_RDA_INT:                // Receive Data Available
        do
          {
          unsigned short temp;

          // calc next insert index & store character
          temp = (uart1_rx_insert_idx + 1) % UART1_RX_BUFFER_SIZE;
          uart1_rx_buffer[uart1_rx_insert_idx] = UART1_RBR;

          // check for more room in queue
          if (temp != uart1_rx_extract_idx)
            uart1_rx_insert_idx = temp; // update insert index
          }
        while (UART1_LSR & ULSR_RDR);

        break;

      case UIIR_THRE_INT:               // Transmit Holding Register Empty
        while (UART1_LSR & ULSR_THRE)
          {
          // check if more data to send
          if (uart1_tx_insert_idx != uart1_tx_extract_idx)
            {
            UART1_THR = uart1_tx_buffer[uart1_tx_extract_idx++];
            uart1_tx_extract_idx %= UART1_TX_BUFFER_SIZE;
            }
          else
            {
            // no
            uart1_tx_running = 0;       // clear running flag
            break;
            }
          }

        break;

      case UIIR_MS_INT:                 // MODEM Status
        UART1_MSR;                          // read MSR to clear
        break;

      default:                          // Unknown
        UART1_LSR;
        UART1_RBR;
        UART1_MSR;
        break;
      }
    }

  VICVectAddr = 0x00000000;             // clear this interrupt from the VIC
}
