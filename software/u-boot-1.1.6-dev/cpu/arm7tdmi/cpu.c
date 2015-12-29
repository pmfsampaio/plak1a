/*
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * S3C44B0 CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/hardware.h>




int cpu_init (void)
{
	/*we have nothing to do here,cause we have no cache 
	 *icache_enable();
   */
	return 0;
}

int cleanup_before_linux (void)
{
	/*
		cache memory should be enabled before calling
		Linux to make the kernel uncompression faster
	*/
	/*we have nothing to do here,cause we have no cache
	 *icache_enable();
   */
	disable_interrupts ();

	return 0;
}

void reset_cpu (ulong addr)
{
	/*
		reset the cpu using watchdog
	*/
	
	/* set the timeout value ,the minimum value is 0xff */
  WDTC=0x000000ff;
  /* setup the watchdog mode */
  WDMOD=0x03;
  /* start the watchdog */
  WDFEED=0xaa;
  WDFEED=0x55;
  
  /*wait ...*/
	while(1) {
		/*NOP*/
	}
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void reset_cpu (ulong addr);

	disable_interrupts ();
	reset_cpu (0);

	/*NOTREACHED*/
	return (0);
}

void icache_enable (void)
{
}

void icache_disable (void)
{
}

int icache_status (void)
{
	return 0;
}

void dcache_enable (void)
{
	icache_enable();
}

void dcache_disable (void)
{
	icache_disable();
}

int dcache_status (void)
{
	return icache_status();
}

/*
	RTC stuff
*/
#include <rtc.h>
#ifndef BCD2HEX
	#define BCD2HEX(n)  ((n>>4)*10+(n&0x0f))
#endif
#ifndef HEX2BCD
	#define HEX2BCD(x) ((((x) / 10) << 4) + (x) % 10)
#endif

void rtc_get (struct rtc_time* tm)
{
	
}

void rtc_set (struct rtc_time* tm)
{
	
}

void rtc_reset (void)
{
	
}


