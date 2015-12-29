// ***********************************************************************
//
//  LPC210X.H:  Header file for Philips LPC2104 / LPC2105 / LPC2106
//
// ***********************************************************************

#ifndef __LPC22xx_H
#define __LPC22xx_H

/* Vectored Interrupt Controller (VIC) */
#define VIC_BASE_ADDR	0xFFFFF000
#define VICIRQStatus   (*((volatile unsigned long *) 0xFFFFF000))
#define VICFIQStatus   (*((volatile unsigned long *) 0xFFFFF004))
#define VICRawIntr     (*((volatile unsigned long *) 0xFFFFF008))
#define VICIntSelect   (*((volatile unsigned long *) 0xFFFFF00C))
#define VICIntEnable   (*((volatile unsigned long *) 0xFFFFF010))
#define VICIntEnClr    (*((volatile unsigned long *) 0xFFFFF014))
#define VICSoftInt     (*((volatile unsigned long *) 0xFFFFF018))
#define VICSoftIntClr  (*((volatile unsigned long *) 0xFFFFF01C))
#define VICProtection  (*((volatile unsigned long *) 0xFFFFF020))
#define VICVectAddr    (*((volatile unsigned long *) 0xFFFFF030))
#define VICDefVectAddr (*((volatile unsigned long *) 0xFFFFF034))
#define VICVectAddr0   (*((volatile unsigned long *) 0xFFFFF100))
#define VICVectAddr1   (*((volatile unsigned long *) 0xFFFFF104))
#define VICVectAddr2   (*((volatile unsigned long *) 0xFFFFF108))
#define VICVectAddr3   (*((volatile unsigned long *) 0xFFFFF10C))
#define VICVectAddr4   (*((volatile unsigned long *) 0xFFFFF110))
#define VICVectAddr5   (*((volatile unsigned long *) 0xFFFFF114))
#define VICVectAddr6   (*((volatile unsigned long *) 0xFFFFF118))
#define VICVectAddr7   (*((volatile unsigned long *) 0xFFFFF11C))
#define VICVectAddr8   (*((volatile unsigned long *) 0xFFFFF120))
#define VICVectAddr9   (*((volatile unsigned long *) 0xFFFFF124))
#define VICVectAddr10  (*((volatile unsigned long *) 0xFFFFF128))
#define VICVectAddr11  (*((volatile unsigned long *) 0xFFFFF12C))
#define VICVectAddr12  (*((volatile unsigned long *) 0xFFFFF130))
#define VICVectAddr13  (*((volatile unsigned long *) 0xFFFFF134))
#define VICVectAddr14  (*((volatile unsigned long *) 0xFFFFF138))
#define VICVectAddr15  (*((volatile unsigned long *) 0xFFFFF13C))
#define VICVectCntl0   (*((volatile unsigned long *) 0xFFFFF200))
#define VICVectCntl1   (*((volatile unsigned long *) 0xFFFFF204))
#define VICVectCntl2   (*((volatile unsigned long *) 0xFFFFF208))
#define VICVectCntl3   (*((volatile unsigned long *) 0xFFFFF20C))
#define VICVectCntl4   (*((volatile unsigned long *) 0xFFFFF210))
#define VICVectCntl5   (*((volatile unsigned long *) 0xFFFFF214))
#define VICVectCntl6   (*((volatile unsigned long *) 0xFFFFF218))
#define VICVectCntl7   (*((volatile unsigned long *) 0xFFFFF21C))
#define VICVectCntl8   (*((volatile unsigned long *) 0xFFFFF220))
#define VICVectCntl9   (*((volatile unsigned long *) 0xFFFFF224))
#define VICVectCntl10  (*((volatile unsigned long *) 0xFFFFF228))
#define VICVectCntl11  (*((volatile unsigned long *) 0xFFFFF22C))
#define VICVectCntl12  (*((volatile unsigned long *) 0xFFFFF230))
#define VICVectCntl13  (*((volatile unsigned long *) 0xFFFFF234))
#define VICVectCntl14  (*((volatile unsigned long *) 0xFFFFF238))
#define VICVectCntl15  (*((volatile unsigned long *) 0xFFFFF23C))

/* Pin Connect Block */
#define PINSEL0        (*((volatile unsigned long *) 0xE002C000))
#define PINSEL1        (*((volatile unsigned long *) 0xE002C004))
#define	PINSEL2		   (*((volatile unsigned long *) 0xE002C014))

/* General Purpose Input/Output (GPIO) */
#define IO0PIN          (*((volatile unsigned long *) 0xE0028000))
#define IO0SET          (*((volatile unsigned long *) 0xE0028004))
#define IO0DIR          (*((volatile unsigned long *) 0xE0028008))
#define IO0CLR          (*((volatile unsigned long *) 0xE002800C))

#define IO1PIN          (*((volatile unsigned long *) 0xE0028010))
#define IO1SET          (*((volatile unsigned long *) 0xE0028014))
#define IO1DIR          (*((volatile unsigned long *) 0xE0028018))
#define IO1CLR          (*((volatile unsigned long *) 0xE002801C))

#define IO2PIN          (*((volatile unsigned long *) 0xE0028020))
#define IO2SET          (*((volatile unsigned long *) 0xE0028024))
#define IO2DIR          (*((volatile unsigned long *) 0xE0028028))
#define IO2CLR          (*((volatile unsigned long *) 0xE002802C))

#define IO3PIN          (*((volatile unsigned long *) 0xE0028030))
#define IO3SET          (*((volatile unsigned long *) 0xE0028034))
#define IO3DIR          (*((volatile unsigned long *) 0xE0028038))
#define IO3CLR          (*((volatile unsigned long *) 0xE002803C))

/* Memory Accelerator Module (MAM) */
#define MAMCR          (*((volatile unsigned char *) 0xE01FC000))
#define MAMTIM         (*((volatile unsigned char *) 0xE01FC004))
#define MAMMAP         (*((volatile unsigned char *) 0xE01FC040))

/* Phase Locked Loop (PLL) */
#define PLLCON         (*((volatile unsigned char *) 0xE01FC080))
#define PLLCFG         (*((volatile unsigned char *) 0xE01FC084))
#define PLLSTAT        (*((volatile unsigned short*) 0xE01FC088))
#define PLLFEED        (*((volatile unsigned char *) 0xE01FC08C))

/* VPB Divider */
#define VPBDIV         (*((volatile unsigned char *) 0xE01FC100))

/* Power Control */
#define PCON           (*((volatile unsigned char *) 0xE01FC0C0))
#define PCONP          (*((volatile unsigned long *) 0xE01FC0C4))

/* External Interrupts */
#define EXTINT         (*((volatile unsigned char *) 0xE01FC140))
#define EXTWAKE        (*((volatile unsigned char *) 0xE01FC144))

/* Timer 0 */
#define TIMER0_IR      (*((volatile unsigned long *) 0xE0004000))
#define TIMER0_TCR     (*((volatile unsigned long *) 0xE0004004))
#define TIMER0_TC      (*((volatile unsigned long *) 0xE0004008))
#define TIMER0_PR      (*((volatile unsigned long *) 0xE000400C))
#define TIMER0_PC      (*((volatile unsigned long *) 0xE0004010))
#define TIMER0_MCR     (*((volatile unsigned long *) 0xE0004014))
#define TIMER0_MR0     (*((volatile unsigned long *) 0xE0004018))
#define TIMER0_MR1     (*((volatile unsigned long *) 0xE000401C))
#define TIMER0_MR2     (*((volatile unsigned long *) 0xE0004020))
#define TIMER0_MR3     (*((volatile unsigned long *) 0xE0004024))
#define TIMER0_CCR     (*((volatile unsigned long *) 0xE0004028))
#define TIMER0_CR0     (*((volatile unsigned long *) 0xE000402C))
#define TIMER0_CR1     (*((volatile unsigned long *) 0xE0004030))
#define TIMER0_CR2     (*((volatile unsigned long *) 0xE0004034))
#define TIMER0_CR3     (*((volatile unsigned long *) 0xE0004038))
#define TIMER0_EMR     (*((volatile unsigned long *) 0xE000403C))

/* Timer 1 */
#define TIMER1_IR      (*((volatile unsigned long *) 0xE0008000))
#define TIMER1_TCR     (*((volatile unsigned long *) 0xE0008004))
#define TIMER1_TC      (*((volatile unsigned long *) 0xE0008008))
#define TIMER1_PR      (*((volatile unsigned long *) 0xE000800C))
#define TIMER1_PC      (*((volatile unsigned long *) 0xE0008010))
#define TIMER1_MCR     (*((volatile unsigned long *) 0xE0008014))
#define TIMER1_MR0     (*((volatile unsigned long *) 0xE0008018))
#define TIMER1_MR1     (*((volatile unsigned long *) 0xE000801C))
#define TIMER1_MR2     (*((volatile unsigned long *) 0xE0008020))
#define TIMER1_MR3     (*((volatile unsigned long *) 0xE0008024))
#define TIMER1_CCR     (*((volatile unsigned long *) 0xE0008028))
#define TIMER1_CR0     (*((volatile unsigned long *) 0xE000802C))
#define TIMER1_CR1     (*((volatile unsigned long *) 0xE0008030))
#define TIMER1_CR2     (*((volatile unsigned long *) 0xE0008034))
#define TIMER1_CR3     (*((volatile unsigned long *) 0xE0008038))
#define TIMER1_EMR     (*((volatile unsigned long *) 0xE000803C))

/* Pulse Width Modulator (PWM) */
#define PWM_IR         (*((volatile unsigned long *) 0xE0014000))
#define PWM_TCR        (*((volatile unsigned long *) 0xE0014004))
#define PWM_TC         (*((volatile unsigned long *) 0xE0014008))
#define PWM_PR         (*((volatile unsigned long *) 0xE001400C))
#define PWM_PC         (*((volatile unsigned long *) 0xE0014010))
#define PWM_MCR        (*((volatile unsigned long *) 0xE0014014))
#define PWM_MR0        (*((volatile unsigned long *) 0xE0014018))
#define PWM_MR1        (*((volatile unsigned long *) 0xE001401C))
#define PWM_MR2        (*((volatile unsigned long *) 0xE0014020))
#define PWM_MR3        (*((volatile unsigned long *) 0xE0014024))
#define PWM_MR4        (*((volatile unsigned long *) 0xE0014040))
#define PWM_MR5        (*((volatile unsigned long *) 0xE0014044))
#define PWM_MR6        (*((volatile unsigned long *) 0xE0014048))
#define PWM_CCR        (*((volatile unsigned long *) 0xE0014028))
#define PWM_CR0        (*((volatile unsigned long *) 0xE001402C))
#define PWM_CR1        (*((volatile unsigned long *) 0xE0014030))
#define PWM_CR2        (*((volatile unsigned long *) 0xE0014034))
#define PWM_CR3        (*((volatile unsigned long *) 0xE0014038))
#define PWM_EMR        (*((volatile unsigned long *) 0xE001403C))
#define PWM_PCR        (*((volatile unsigned long *) 0xE001404C))
#define PWM_LER        (*((volatile unsigned long *) 0xE0014050))

/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define UART0_RBR      (*((volatile unsigned char *) 0xE000C000))
#define UART0_THR      (*((volatile unsigned char *) 0xE000C000))
#define UART0_IER      (*((volatile unsigned char *) 0xE000C004))
#define UART0_IIR      (*((volatile unsigned char *) 0xE000C008))
#define UART0_FCR      (*((volatile unsigned char *) 0xE000C008))
#define UART0_LCR      (*((volatile unsigned char *) 0xE000C00C))
#define UART0_MCR      (*((volatile unsigned char *) 0xE000C010))
#define UART0_LSR      (*((volatile unsigned char *) 0xE000C014))
#define UART0_MSR      (*((volatile unsigned char *) 0xE000C018))
#define UART0_SCR      (*((volatile unsigned char *) 0xE000C01C))
#define UART0_DLL      (*((volatile unsigned char *) 0xE000C000))
#define UART0_DLM      (*((volatile unsigned char *) 0xE000C004))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define UART1_RBR      (*((volatile unsigned char *) 0xE0010000))
#define UART1_THR      (*((volatile unsigned char *) 0xE0010000))
#define UART1_IER      (*((volatile unsigned char *) 0xE0010004))
#define UART1_IIR      (*((volatile unsigned char *) 0xE0010008))
#define UART1_FCR      (*((volatile unsigned char *) 0xE0010008))
#define UART1_LCR      (*((volatile unsigned char *) 0xE001000C))
#define UART1_MCR      (*((volatile unsigned char *) 0xE0010010))
#define UART1_LSR      (*((volatile unsigned char *) 0xE0010014))
#define UART1_MSR      (*((volatile unsigned char *) 0xE0010018))
#define UART1_SCR      (*((volatile unsigned char *) 0xE001001C))
#define UART1_DLL      (*((volatile unsigned char *) 0xE0010000))
#define UART1_DLM      (*((volatile unsigned char *) 0xE0010004))

/* I2C Interface */
#define I2C_I2CONSET   (*((volatile unsigned char *) 0xE001C000))
#define I2C_I2STAT     (*((volatile unsigned char *) 0xE001C004))
#define I2C_I2DAT      (*((volatile unsigned char *) 0xE001C008))
#define I2C_I2ADR      (*((volatile unsigned char *) 0xE001C00C))
#define I2C_I2SCLH     (*((volatile unsigned short*) 0xE001C010))
#define I2C_I2SCLL     (*((volatile unsigned short*) 0xE001C014))
#define I2C_I2CONCLR   (*((volatile unsigned char *) 0xE001C018))

/* SPI (Serial Peripheral Interface) */
#define SPI_SPCR       (*((volatile unsigned char *) 0xE0020000))
#define SPI_SPSR       (*((volatile unsigned char *) 0xE0020004))
#define SPI_SPDR       (*((volatile unsigned char *) 0xE0020008))
#define SPI_SPCCR      (*((volatile unsigned char *) 0xE002000C))
#define SPI_SPTCR      (*((volatile unsigned char *) 0xE0020010))
#define SPI_SPTSR      (*((volatile unsigned char *) 0xE0020014))
#define SPI_SPTOR      (*((volatile unsigned char *) 0xE0020018))
#define SPI_SPINT      (*((volatile unsigned char *) 0xE002001C))

/* Real Time Clock */
#define RTC_ILR        (*((volatile unsigned char *) 0xE0024000))
#define RTC_CTC        (*((volatile unsigned short*) 0xE0024004))
#define RTC_CCR        (*((volatile unsigned char *) 0xE0024008))
#define RTC_CIIR       (*((volatile unsigned char *) 0xE002400C))
#define RTC_AMR        (*((volatile unsigned char *) 0xE0024010))
#define RTC_CTIME0     (*((volatile unsigned long *) 0xE0024014))
#define RTC_CTIME1     (*((volatile unsigned long *) 0xE0024018))
#define RTC_CTIME2     (*((volatile unsigned long *) 0xE002401C))
#define RTC_SEC        (*((volatile unsigned char *) 0xE0024020))
#define RTC_MIN        (*((volatile unsigned char *) 0xE0024024))
#define RTC_HOUR       (*((volatile unsigned char *) 0xE0024028))
#define RTC_DOM        (*((volatile unsigned char *) 0xE002402C))
#define RTC_DOW        (*((volatile unsigned char *) 0xE0024030))
#define RTC_DOY        (*((volatile unsigned short*) 0xE0024034))
#define RTC_MONTH      (*((volatile unsigned char *) 0xE0024038))
#define RTC_YEAR       (*((volatile unsigned short*) 0xE002403C))
#define RTC_ALSEC      (*((volatile unsigned char *) 0xE0024060))
#define RTC_ALMIN      (*((volatile unsigned char *) 0xE0024064))
#define RTC_ALHOUR     (*((volatile unsigned char *) 0xE0024068))
#define RTC_ALDOM      (*((volatile unsigned char *) 0xE002406C))
#define RTC_ALDOW      (*((volatile unsigned char *) 0xE0024070))
#define RTC_ALDOY      (*((volatile unsigned short*) 0xE0024074))
#define RTC_ALMON      (*((volatile unsigned char *) 0xE0024078))
#define RTC_ALYEAR     (*((volatile unsigned short*) 0xE002407C))
#define RTC_PREINT     (*((volatile unsigned short*) 0xE0024080))
#define RTC_PREFRAC    (*((volatile unsigned short*) 0xE0024084))

/* Watchdog */
#define WDMOD          (*((volatile unsigned char *) 0xE0000000))
#define WDTC           (*((volatile unsigned long *) 0xE0000004))
#define WDFEED         (*((volatile unsigned char *) 0xE0000008))
#define WDTV           (*((volatile unsigned long *) 0xE000000C))

/*
 *
 */
#define BCFG0			(*((volatile unsigned long *) 0xFFE00000))
#define BCFG1			(*((volatile unsigned long *) 0xFFE00004))
#define BCFG2			(*((volatile unsigned long *) 0xFFE00008))
#define BCFG3			(*((volatile unsigned long *) 0xFFE0000C))

#define WDOG()

// PLL setup values are computed within the LPC include file
// It relies upon the following defines
#define FOSC                (14745000)  // Master Oscillator Freq.
#define PLL_MUL             (4)         // PLL Multiplier
#define CCLK                (FOSC * PLL_MUL) // CPU Clock Freq.

// Pheripheral Bus Speed Divider
#define PBSD                1           // MUST BE 1, 2, or 4
#define PCLK                (CCLK / PBSD) // Pheripheal Bus Clock Freq.


///////////////////////////////////////////////////////////////////////////////
// Universal Asynchronous Receiver Transmitter Registers
typedef struct
{
  union
    {
    volatile unsigned char rbr;                          // Receive Buffer Register
    volatile unsigned char thr;                          // Transmit Holding Register
    volatile unsigned char dll;                          // Divisor Latch Register (LSB)
    volatile unsigned char _pad0[4];
    };

  union
    {
    volatile unsigned char ier;                          // Interrupt Enable Register
    volatile unsigned char dlm;                          // Divisor Latch Register (MSB)
    volatile unsigned char _pad1[4];
    };

  union
    {
    volatile unsigned char iir;                          // Interrupt ID Register
    volatile unsigned char fcr;                          // FIFO Control Register
    volatile unsigned char _pad2[4];
    };

  volatile unsigned char lcr;                            // Line Control Registe
  volatile unsigned char _pad3[3];
  volatile unsigned char mcr;                            // MODEM Control Register
  volatile unsigned char _pad4[3];
  volatile unsigned char lsr;                            // Line Status Register
  volatile unsigned char _pad5[3];
  volatile unsigned char msr;                            // MODEM Status Register
  volatile unsigned char _pad6[3];
  volatile unsigned char scr;                            // Scratch Pad Register
  volatile unsigned char _pad7[3];
} uartRegs_t;

///////////////////////////////////////////////////////////////////////////////
// UART defines

// Interrupt Enable Register bit definitions
#define UIER_ERBFI          (1 << 0)    // Enable Receive Data Available Interrupt
#define UIER_ETBEI          (1 << 1)    // Enable Transmit Holding Register Empty Interrupt
#define UIER_ELSI           (1 << 2)    // Enable Receive Line Status Interrupt
#define UIER_EDSSI          (1 << 3)    // Enable MODEM Status Interrupt

// Interrupt ID Register bit definitions
#define UIIR_NO_INT         (1 << 0)    // NO INTERRUPTS PENDING
#define UIIR_MS_INT         (0 << 1)    // MODEM Status
#define UIIR_THRE_INT       (1 << 1)    // Transmit Holding Register Empty
#define UIIR_RDA_INT        (2 << 1)    // Receive Data Available
#define UIIR_RLS_INT        (3 << 1)    // Receive Line Status
#define UIIR_CTI_INT        (6 << 1)    // Character Timeout Indicator
#define UIIR_ID_MASK        0x0E

// FIFO Control Register bit definitions
#define UFCR_FIFO_ENABLE    (1 << 0)    // FIFO Enable
#define UFCR_RX_FIFO_RESET  (1 << 1)    // Reset Receive FIFO
#define UFCR_TX_FIFO_RESET  (1 << 2)    // Reset Transmit FIFO
#define UFCR_FIFO_TRIG1     (0 << 6)    // Trigger @ 1 character in FIFO
#define UFCR_FIFO_TRIG4     (1 << 6)    // Trigger @ 4 characters in FIFO
#define UFCR_FIFO_TRIG8     (2 << 6)    // Trigger @ 8 characters in FIFO
#define UFCR_FIFO_TRIG14    (3 << 6)    // Trigger @ 14 characters in FIFO

// Line Control Register bit definitions
#define ULCR_CHAR_5         (0 << 0)    // 5-bit character length
#define ULCR_CHAR_6         (1 << 0)    // 6-bit character length
#define ULCR_CHAR_7         (2 << 0)    // 7-bit character length
#define ULCR_CHAR_8         (3 << 0)    // 8-bit character length
#define ULCR_STOP_1         (0 << 2)    // 1 stop bit
#define ULCR_STOP_2         (1 << 2)    // 2 stop bits
#define ULCR_PAR_NO         (0 << 3)    // No Parity
#define ULCR_PAR_ODD        (1 << 3)    // Odd Parity
#define ULCR_PAR_EVEN       (3 << 3)    // Even Parity
#define ULCR_PAR_MARK       (5 << 3)    // MARK "1" Parity
#define ULCR_PAR_SPACE      (7 << 3)    // SPACE "0" Paruty
#define ULCR_BREAK_ENABLE   (1 << 6)    // Output BREAK line condition
#define ULCR_DLAB_ENABLE    (1 << 7)    // Enable Divisor Latch Access

// Modem Control Register bit definitions
#define UMCR_DTR            (1 << 0)    // Data Terminal Ready
#define UMCR_RTS            (1 << 1)    // Request To Send
#define UMCR_LB             (1 << 4)    // Loopback

// Line Status Register bit definitions
#define ULSR_RDR            (1 << 0)    // Receive Data Ready
#define ULSR_OE             (1 << 1)    // Overrun Error
#define ULSR_PE             (1 << 2)    // Parity Error
#define ULSR_FE             (1 << 3)    // Framing Error
#define ULSR_BI             (1 << 4)    // Break Interrupt
#define ULSR_THRE           (1 << 5)    // Transmit Holding Register Empty
#define ULSR_TEMT           (1 << 6)    // Transmitter Empty
#define ULSR_RXFE           (1 << 7)    // Error in Receive FIFO
#define ULSR_ERR_MASK       0x1E

// Modem Status Register bit definitions
#define UMSR_DCTS           (1 << 0)    // Delta Clear To Send
#define UMSR_DDSR           (1 << 1)    // Delta Data Set Ready
#define UMSR_TERI           (1 << 2)    // Trailing Edge Ring Indicator
#define UMSR_DDCD           (1 << 3)    // Delta Data Carrier Detect
#define UMSR_CTS            (1 << 4)    // Clear To Send
#define UMSR_DSR            (1 << 5)    // Data Set Ready
#define UMSR_RI             (1 << 6)    // Ring Indicator
#define UMSR_DCD            (1 << 7)    // Data Carrier Detect

// Timer & PWM Registers
typedef struct
{
  volatile unsigned long ir;                             // Interrupt Register
  volatile unsigned long tcr;                            // Timer Control Register
  volatile unsigned long tc;                             // Timer Counter
  volatile unsigned long pr;                             // Prescale Register
  volatile unsigned long pc;                             // Prescale Counter Register
  volatile unsigned long mcr;                            // Match Control Register
  volatile unsigned long mr0;                            // Match Register 0
  volatile unsigned long mr1;                            // Match Register 1
  volatile unsigned long mr2;                            // Match Register 2
  volatile unsigned long mr3;                            // Match Register 3
  volatile unsigned long ccr;                            // Capture Control Register
  volatile unsigned long cr0;                            // Capture Register 0
  volatile unsigned long cr1;                            // Capture Register 1
  volatile unsigned long cr2;                            // Capture Register 2
  volatile unsigned long cr3;                            // Capture Register 3
  volatile unsigned long emr;                            // External Match Register
  volatile unsigned long mr4;                            // Match Register 4
  volatile unsigned long mr5;                            // Match Register 5
  volatile unsigned long mr6;                            // Match Register 6
  volatile unsigned long pcr;                            // Control Register
  volatile unsigned long ler;                            // Latch Enable Register
} pwmTmrRegs_t;

// Timer Interrupt Register Bit Definitions
#define TIR_MR0I    (1 << 0)            // Interrupt flag for match channel 0
#define TIR_MR1I    (1 << 1)            // Interrupt flag for match channel 1
#define TIR_MR2I    (1 << 2)            // Interrupt flag for match channel 2
#define TIR_MR3I    (1 << 3)            // Interrupt flag for match channel 3
#define TIR_CR0I    (1 << 4)            // Interrupt flag for capture channel 0 event
#define TIR_CR1I    (1 << 5)            // Interrupt flag for capture channel 1 event
#define TIR_CR2I    (1 << 6)            // Interrupt flag for capture channel 2 event
#define TIR_CR3I    (1 << 7)            // Interrupt flag for capture channel 3 event

// PWM Interrupt Register Bit Definitions
#define PWMIR_MR0I  (1 << 0)            // Interrupt flag for match channel 0
#define PWMIR_MR1I  (1 << 1)            // Interrupt flag for match channel 1
#define PWMIR_MR2I  (1 << 2)            // Interrupt flag for match channel 2
#define PWMIR_MR3I  (1 << 3)            // Interrupt flag for match channel 3
#define PWMIR_MR4I  (1 << 8)            // Interrupt flag for match channel 4
#define PWMIR_MR5I  (1 << 9)            // Interrupt flag for match channel 5
#define PWMIR_MR6I  (1 << 10)           // Interrupt flag for match channel 6
#define PWMIR_MASK  (0x070F)

// Timer Control Register Bit Definitions
#define TCR_ENABLE  (1 << 0)
#define TCR_RESET   (1 << 1)

// PWM Control Register Bit Definitions
#define PWMCR_ENABLE (1 << 0)
#define PWMCR_RESET (1 << 1)

// Timer Match Control Register Bit Definitions
#define TMCR_MR0_I  (1 << 0)            // Enable Interrupt when MR0 matches TC
#define TMCR_MR0_R  (1 << 1)            // Enable Reset of TC upon MR0 match
#define TMCR_MR0_S  (1 << 2)            // Enable Stop of TC upon MR0 match
#define TMCR_MR1_I  (1 << 3)            // Enable Interrupt when MR1 matches TC
#define TMCR_MR1_R  (1 << 4)            // Enable Reset of TC upon MR1 match
#define TMCR_MR1_S  (1 << 5)            // Enable Stop of TC upon MR1 match
#define TMCR_MR2_I  (1 << 6)            // Enable Interrupt when MR2 matches TC
#define TMCR_MR2_R  (1 << 7)            // Enable Reset of TC upon MR2 match
#define TMCR_MR2_S  (1 << 8)            // Enable Stop of TC upon MR2 match
#define TMCR_MR3_I  (1 << 9)            // Enable Interrupt when MR3 matches TC
#define TMCR_MR3_R  (1 << 10)           // Enable Reset of TC upon MR3 match
#define TMCR_MR3_S  (1 << 11)           // Enable Stop of TC upon MR3 match

// Timer Capture Control Register Bit Definitions
#define TCCR_CR0_R (1 << 0)            // Enable Rising edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_F (1 << 1)            // Enable Falling edge on CAPn.0 will load TC to CR0
#define TCCR_CR0_I (1 << 2)            // Enable Interrupt on load of CR0
#define TCCR_CR1_R (1 << 3)            // Enable Rising edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_F (1 << 4)            // Enable Falling edge on CAPn.1 will load TC to CR1
#define TCCR_CR1_I (1 << 5)            // Enable Interrupt on load of CR1
#define TCCR_CR2_R (1 << 6)            // Enable Rising edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_F (1 << 7)            // Enable Falling edge on CAPn.2 will load TC to CR2
#define TCCR_CR2_I (1 << 8)            // Enable Interrupt on load of CR2
#define TCCR_CR3_R (1 << 9)            // Enable Rising edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_F (1 << 10)           // Enable Falling edge on CAPn.3 will load TC to CR3
#define TCCR_CR3_I (1 << 11)           // Enable Interrupt on load of CR3

// Vectored Interrupt Controller Registers (VIC)
typedef struct
{
  volatile unsigned long irqStatus;                      // IRQ Status Register
  volatile unsigned long fiqStatus;                      // FIQ Status Register
  volatile unsigned long rawIntr;                        // Raw Interrupt Status Register
  volatile unsigned long intSelect;                      // Interrupt Select Register
  volatile unsigned long intEnable;                      // Interrupt Enable Register
  volatile unsigned long intEnClear;                     // Interrupt Enable Clear Register
  volatile unsigned long softInt;                        // Software Interrupt Register
  volatile unsigned long softIntClear;                   // Software Interrupt Clear Register
  volatile unsigned long protection;                     // Protection Enable Register
  volatile unsigned long _pad0[3];
  volatile unsigned long vectAddr;                       // Vector Address Register
  volatile unsigned long defVectAddr;                    // Default Vector Address Register
  volatile unsigned long _pad1[50];
  volatile unsigned long vectAddr0;                      // Vector Address 0 Register
  volatile unsigned long vectAddr1;                      // Vector Address 1 Register
  volatile unsigned long vectAddr2;                      // Vector Address 2 Register
  volatile unsigned long vectAddr3;                      // Vector Address 3 Register
  volatile unsigned long vectAddr4;                      // Vector Address 4 Register
  volatile unsigned long vectAddr5;                      // Vector Address 5 Register
  volatile unsigned long vectAddr6;                      // Vector Address 6 Register
  volatile unsigned long vectAddr7;                      // Vector Address 7 Register
  volatile unsigned long vectAddr8;                      // Vector Address 8 Register
  volatile unsigned long vectAddr9;                      // Vector Address 9 Register
  volatile unsigned long vectAddr10;                     // Vector Address 10 Register
  volatile unsigned long vectAddr11;                     // Vector Address 11 Register
  volatile unsigned long vectAddr12;                     // Vector Address 12 Register
  volatile unsigned long vectAddr13;                     // Vector Address 13 Register
  volatile unsigned long vectAddr14;                     // Vector Address 14 Register
  volatile unsigned long vectAddr15;                     // Vector Address 15 Register
  volatile unsigned long _pad2[48];
  volatile unsigned long vectCntl0;                      // Vector Control 0 Register
  volatile unsigned long vectCntl1;                      // Vector Control 1 Register
  volatile unsigned long vectCntl2;                      // Vector Control 2 Register
  volatile unsigned long vectCntl3;                      // Vector Control 3 Register
  volatile unsigned long vectCntl4;                      // Vector Control 4 Register
  volatile unsigned long vectCntl5;                      // Vector Control 5 Register
  volatile unsigned long vectCntl6;                      // Vector Control 6 Register
  volatile unsigned long vectCntl7;                      // Vector Control 7 Register
  volatile unsigned long vectCntl8;                      // Vector Control 8 Register
  volatile unsigned long vectCntl9;                      // Vector Control 9 Register
  volatile unsigned long vectCntl10;                     // Vector Control 10 Register
  volatile unsigned long vectCntl11;                     // Vector Control 11 Register
  volatile unsigned long vectCntl12;                     // Vector Control 12 Register
  volatile unsigned long vectCntl13;                     // Vector Control 13 Register
  volatile unsigned long vectCntl14;                     // Vector Control 14 Register
  volatile unsigned long vectCntl15;                     // Vector Control 15 Register
} vicRegs_t;

// VIC Channel Assignments
#define VIC_WDT         0
#define VIC_TIMER0      4
#define VIC_TIMER1      5
#define VIC_UART0       6
#define VIC_UART1       7
#define VIC_PWM         8
#define VIC_PWM0        8
#define VIC_I2C         9
#define VIC_SPI         10
#define VIC_SPI0        10
#define VIC_SPI1        11
#define VIC_PLL         12
#define VIC_RTC         13
#define VIC_EINT0       14
#define VIC_EINT1       15
#define VIC_EINT2       16
#define VIC_EINT3       17
#define VIC_ADC         18

// Vector Control Register bit definitions
#define VIC_ENABLE      (1 << 5)

// Convert Channel Number to Bit Value
#define VIC_BIT(chan)   (1 << (chan))



#endif  // __LPC22xx_H
