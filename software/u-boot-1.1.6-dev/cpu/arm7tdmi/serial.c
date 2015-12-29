/*
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2002-2004
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/hardware.h>

#ifdef CONFIG_SERIAL1

#define URBR U0RBR
#define UTHR U0THR
#define UIER U0IER
#define UIIR U0IIR
#define UFCR U0FCR
#define ULCR U0LCR
#define UMCR U0MCR
#define ULSR U0LSR
#define UMSR U0MSR
#define USCR U0SCR
#define UDLL U0DLL
#define UDLM U0DLM

#endif

#if CONFIG_SERIAL2

#define URBR U1RBR
#define UTHR U1THR
#define UIER U1IER
#define UIIR U1IIR
#define UFCR U1FCR
#define ULCR U1LCR
#define UMCR U1MCR
#define ULSR U1LSR
#define UMSR U1MSR
#define USCR U1SCR
#define UDLL U1DLL
#define UDLM U1DLM

#endif

#define	USRRxData      	(1 << 0)
#define	USRTxHoldEmpty 	(1 << 6)
#define GET_CHAR(p)	p
#define PUT_CHAR(p,c)  	(p= (unsigned )(c))
#define TX_READY(s)    	((s) & USRTxHoldEmpty)
#define RX_DATA(s)     	((s) & USRRxData)

/* flush serial input queue. returns 0 on success or negative error
 * number otherwise
 */
static int serial_flush_input(void) {
	volatile u32 tmp;

	/* keep on reading as long as the receiver is not empty */
	while (RX_DATA(ULSR)) {
		tmp = GET_CHAR(URBR);
	}

	return 0;
}

/* flush output queue. returns 0 on success or negative error number
 * otherwise
 */
static int serial_flush_output(void) {
	/* wait until the transmitter is no longer busy */
	while (TX_READY(ULSR) == 0) {
	}

	return 0;
}

void serial_setbrg(void) {
	DECLARE_GLOBAL_DATA_PTR;

	u32 divisor = 0;

	/* get correct divisor */
	switch (gd->baudrate) {

	case 9600:
		divisor = 96;
		break;

	case 19200:
		divisor = 48;
		break;

	case 38400:
		divisor = 24;

	case 57600:
		divisor = 16;

	case 115200:
		divisor = 8;

	}
	ULCR = 0x83;
	UDLM = (divisor >> 8) & 0x00ff;
	UDLL = divisor & 0x00ff;

	ULCR = 0x03;
	UIER = 0x00000007;
	UFCR = 0x00000081;
	serial_flush_output();
	serial_flush_input();

	for (divisor = 0; divisor < 100; divisor++) {
		/* NOP */
	}
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
int serial_init(void) {
	serial_setbrg();

	return (0);
}

/*
 * Output a single byte to the serial port.
 */
void serial_putc(const char c) {
	/* wait for room in the transmit FIFO */
	while (TX_READY(ULSR) == 0)
		;

	PUT_CHAR(UTHR,c);

	/*
	 to be polite with serial console add a line feed
	 to the carriage return character
	 */
	if (c == '\n')
		serial_putc('\r');
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc(void) {
	return (RX_DATA(ULSR));
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc(void) {
	int rv;

	for (;;) {
		rv = serial_tstc();

		if (rv > 0)
			return (GET_CHAR(URBR));
	}
}

void serial_puts(const char *s) {
	while (*s != 0) {
		serial_putc(*s++);
	}
}
