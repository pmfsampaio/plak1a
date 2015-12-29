/*
 * (C) Copyright 2004
 * DAVE Srl
 *
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * Configuation settings for the lpc board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * If we are developing, we might want to start armboot from ram
 * so we MUST NOT initialize critical regs like mem-timing ...
 */


/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM7			1	/* This is a ARM7 CPU	*/
#define CONFIG_LPC			1	/* on an lpc Board      */
#define CONFIG_ARM_THUMB	1	/* this is an ARM7TDMI */
#undef  CONFIG_ARM7_REVD	 	/* disable ARM720 REV.D Workarounds */



#undef CONFIG_USE_IRQ			/* don't need them anymore */


/*
 * Size of malloc() pool
 */
#define CFG_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for Monitor	*/
#define CFG_ENV_SIZE		1024	/* 1024 bytes may be used for env vars*/
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 128*1024 )
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
//#define CONFIG_DRIVER_RTL8019
//#define RTL8019_BASE              0x82000600
//#define CONFIG_NET_RETRY_COUNT		10	   /* # of retries          */

#define CONFIG_DRIVER_SMSC9118
#define CONFIG_SMSC9118_BASE		0x82000000

/*
 * select serial console configuration
 */
//#define CONFIG_SERIAL1		1	/* we use Serial line 1 */
#define CONFIG_SERIAL2		1	/* we use Serial line 1 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		9600

#define CONFIG_BOOTP_MASK       (CONFIG_BOOTP_DEFAULT|CONFIG_BOOTP_BOOTFILESIZE)

#define CONFIG_COMMANDS		( CONFIG_CMD_DFL | \
								CFG_CMD_ELF | \
								CFG_CMD_FLASH | \
								CFG_CMD_NET  | \
								CFG_CMD_PING | \
								CFG_CMD_ENV)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	5
#define CONFIG_ETHADDR	00:50:c2:1e:af:fb
#define CONFIG_BOOTARGS  "setenv bootargs root=/dev/ram0 ip=192.168.0.70:::::eth0:off \
							 ether=25,0,0,0,eth0 ethaddr=00:50:c2:1e:af:fb"
#define CONFIG_NETMASK  255.255.255.0
#define CONFIG_IPADDR   197.168.0.70
#define CONFIG_SERVERIP	197.168.0.1
#define CONFIG_BOOTFILE	"linux.bin"
#define CONFIG_BOOTCOMMAND	"bootm 20000 f0000"

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory		*/
#define	CFG_PROMPT		"=>  "	/* Monitor Command Prompt	*/
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x81000000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x81300000	/* 4 ... 8 MB in DRAM	*/

#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR		0x81008000	/* default load address	*/

#define	CFG_HZ				10000		/* 10 kHz,since the timer0 is set to 10kHz */

						/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1	   /* we have 1 banks of DRAM */
#define PHYS_SDRAM_1		0x81000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x00400000 /* 4 MB */

#define PHYS_FLASH_1		0x80000000 /* Flash Bank #1 */
#define PHYS_FLASH_SIZE		0x00800000 /* 8 MB */

#define CFG_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	135 //256	/* max number of sectors on one chip	*/

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	1000	/* Timeout for Flash Write (in ms)	*/

#define CFG_FLASH_WORD_SIZE	unsigned short	/* flash word size (width)	*/
#define CFG_FLASH_ADDR0		0x555 	/* 1st address for flash config cycles	*/
#define CFG_FLASH_ADDR1		0xaaa	/* 2nd address for flash config cycles	*/
/*
 * The following defines are added for buggy IOP480 byte interface.
 * All other boards should use the standard values (CPCI405 etc.)
 */
#define CFG_FLASH_READ0		0x0000	/* 0 is standard			*/
#define CFG_FLASH_READ1		0x0001	/* 1 is standard			*/
#define CFG_FLASH_READ2		0x0002	/* 2 is standard			*/

#define CFG_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */

/*-----------------------------------------------------------------------
 * Environment Variable setup
 */
#define CFG_ENV_IS_IN_FLASH	1	/* use FLASH for environment vars */
#undef CFG_ENV_IS_NOWHERE
#define CFG_ENV_ADDR (CFG_FLASH_BASE + 0x7FE000) /* environment start address */
#define CFG_ENV_SECT_SIZE 0x2000 /* Total Size of Environment Sector */

/*-----------------------------------------------------------------------
 * I2C EEPROM (STM24C02W6) for environment
 */


/* Flash banks JFFS2 should use */

#define CFG_JFFS2_FIRST_BANK    0
#define CFG_JFFS2_FIRST_SECTOR	2
#define CFG_JFFS2_NUM_BANKS     1

/*
	Linux TAGs (see lib_arm/armlinux.c)
*/
#define CONFIG_CMDLINE_TAG
#undef CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

//#define ENV_IS_EMBEDDED		1
//#define CFG_RAMBOOT
//#define CONFIG_SKIP_RELOCATE_UBOOT
//#define CONFIG_SKIP_LOWLEVEL_INIT


#endif	/* __CONFIG_H */
