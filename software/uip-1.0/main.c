/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.16 2006/06/11 21:55:03 adam Exp $
 *
 */


#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"

#include "timer.h"
#include "lpc22xx.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

volatile unsigned long __ticks;

void Timer0Handler(void) {
	TIMER0_IR = 1; /* clear interrupt flag */
	__ticks++; // tick counter.
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void Timer0ISRH(void);

void initTimer(void) {
	PCONP |= (1 << 1); // Enable TMR0 clk
	TIMER0_TCR = 0x2; // counting  disable and set reset
	TIMER0_TCR = 0x0; // release reset
	TIMER0_TCR = 0; // Timer Mode: every rising PCLK edge
	TIMER0_MCR = 0x3; // Enable Interrupt on MR0, Enable reset on MR0

	// set timer 0 period
	TIMER0_PR = 2;
	TIMER0_MR0 = 19600; /* 1ms */

	// init timer 0 interrupt
	TIMER0_IR |= (1 << 0); // clear pending interrupt

	// initialize the interrupt vector
	VICIntSelect &= ~VIC_BIT(VIC_TIMER0); // UART1 selected as IRQ
	VICIntEnable = VIC_BIT(VIC_TIMER0); // UART1 interrupt enabled
	VICVectCntl1 = VIC_ENABLE | VIC_TIMER0;
	VICVectAddr1 = (unsigned long) Timer0ISRH; // address of the ISR

	TIMER0_TCR |= (1 << 0); // counting Enable
}

void enableIRQ(void);

#define VECT_ADDR_INDEX	0x100
#define VECT_PRIO_INDEX 0x200

void initAIC(void) {
	unsigned int i = 0;
	unsigned int *vect_addr, *vect_prio;

	/* initialize VIC*/
	VICIntEnClr = 0xffffffff;
	VICVectAddr = 0;
	VICIntSelect = 0;

	/* set all the vector and vector control register to 0 */
	for (i = 0; i < 32; i++) {
		vect_addr = (unsigned int *) (VIC_BASE_ADDR + VECT_ADDR_INDEX + i * 4);
		vect_prio = (unsigned int *) (VIC_BASE_ADDR + VECT_PRIO_INDEX + i * 4);
		*vect_addr = 0x0;
		*vect_prio = 0xF;
	}
}

void Timer0ISRH(void);

/*---------------------------------------------------------------------------*/
int
main(void)
{
  int i;
  uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer;

	PINSEL1 &= ~(0x3 << 10); // set io pins for led P0.23
	IO0DIR |= (1 << 23);
	IO0SET = (1 << 23); // led off
	IO0CLR = (1 << 23); // led on


	initAIC();
	initTimer();

	enableIRQ();

	UART1Initialize(9600);



  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  
  tapdev_init();
  uip_init();

  uip_ipaddr(ipaddr, 197,168,0,30);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 197,168,0,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

  httpd_init();
  
  /*  telnetd_init();*/
  
  /*  hello_world_init();*/

  /*  {
      u8_t mac[6] = {1,2,3,4,5,6};
      dhcpc_init(&mac, 6);
      }*/
  
  /*uip_ipaddr(ipaddr, 127,0,0,1);
  smtp_configure("localhost", ipaddr);
  SMTP_SEND("adam@sics.se", NULL, "uip-testing@example.com",
	    "Testing SMTP from uIP",
	    "Test message sent by uIP\r\n");*/

  /*
    webclient_init();
    resolv_init();
    uip_ipaddr(ipaddr, 195,54,122,204);
    resolv_conf(ipaddr);
    resolv_query("www.sics.se");*/


  
  while(1) {
    uip_len = tapdev_read();
    if(uip_len > 0) {
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  tapdev_send();
	}
      }

    } else if(timer_expired(&periodic_timer)) {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer)) {
	timer_reset(&arp_timer);
	uip_arp_timer();
      }
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
uip_log(char *m)
{
  UART1Printf("uIP log message: %s\n", m);
}
void
resolv_found(char *name, u16_t *ipaddr)
{
  u16_t *ipaddr2;
  
  if(ipaddr == NULL) {
	  UART1Printf("Host '%s' not found.\n", name);
  } else {
	  UART1Printf("Found name '%s' = %d.%d.%d.%d\n", name,
	   htons(ipaddr[0]) >> 8,
	   htons(ipaddr[0]) & 0xff,
	   htons(ipaddr[1]) >> 8,
	   htons(ipaddr[1]) & 0xff);
    /*    webclient_get("www.sics.se", 80, "/~adam/uip");*/
  }
}
#ifdef __DHCPC_H__
void
dhcpc_configured(const struct dhcpc_state *s)
{
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
  resolv_conf(s->dnsaddr);
}
#endif /* __DHCPC_H__ */
void
smtp_done(unsigned char code)
{
	UART1Printf("SMTP done with code %d\n", code);
}
void
webclient_closed(void)
{
	UART1Printf("Webclient: connection closed\n");
}
void
webclient_aborted(void)
{
	UART1Printf("Webclient: connection aborted\n");
}
void
webclient_timedout(void)
{
	UART1Printf("Webclient: connection timed out\n");
}
void
webclient_connected(void)
{
  printf("Webclient: connected, waiting for data...\n");
}
void
webclient_datahandler(char *data, u16_t len)
{
	UART1Printf("Webclient: got %d bytes of data.\n", len);
}
/*---------------------------------------------------------------------------*/
