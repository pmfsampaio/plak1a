/*
 *  linux/arch/arm/mach-lpc22xx/gpio.c
 *
 *	Copyright (C) 2007 Siemens Building Technologies
 *	                   mailto:philippe.goetz@siemens.com
 *
 *      Based on linux/arch/arm/mach-at91/gpio.c (C) 2005 HP Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <linux/platform_device.h>

#include <linux/serial_8250.h>

#include <asm/arch/gpio.h>
#include <asm/arch/lpc22xx.h>

#define TAB_PINSEL	((volatile unsigned long *) (0xE0028000+0x0000))
#define TAB_PINMODE	((volatile unsigned long *) (0xE0028000+0x0040))

/* APH=XX - General Purpose Input/Output */
static struct lpc22xx_fio_registers {
	struct {
		volatile unsigned long REG_GPIO_FIODIR;             /* 0x0000 */
		volatile unsigned long reserved[3];                 /* 0x0004-0x000C */
		volatile unsigned long REG_GPIO_FIOMASK;            /* 0x0010 */
		volatile unsigned long REG_GPIO_FIOPIN;             /* 0x0014 */
		volatile unsigned long REG_GPIO_FIOSET;             /* 0x0018 */
		volatile unsigned long REG_GPIO_FIOCLR;             /* 0x001C */
	} port[5];
} *lpc22xx_fio_registers = (struct lpc22xx_fio_registers *)0x3FFFC000;

/*
 * Change the pin function and mode to its final functions (0,1,2 or 3).
 * mode = 0 => pullup
 * mode = 2 => no pullup/no pulldown
 * mode = 3 => pulldown
 * The kernel will panic, if you try to change the mode of an pin, which is not in previously in gpio mode.
 */
int __init_or_module lpc22xx_set_periph(unsigned pin, unsigned pinsel, int pinmode)
{
	unsigned reg = pin >> 4;
	unsigned shift = ((pin & 15) * 2);
	unsigned long reg_mask = 3 << shift;
	unsigned long reg_pinsel = (pinsel & 3) << shift;
	unsigned long reg_pinmode = (pinmode & 3) << shift;

//	printk("%s(P%d.%d,%d,%d) =>", __FUNCTION__, pin>>5, pin&0x1F, pinsel, pinmode);
	if((TAB_PINSEL[reg] & reg_mask) == 0) {
		TAB_PINSEL[reg] = TAB_PINSEL[reg] | reg_pinsel;
//		printk(" %08lx\n", reg_pinsel);
 	}
	else if((TAB_PINSEL[reg] & reg_mask) != reg_pinsel) {
		printk("%s(P%d.%d,%d,%d) =>", __FUNCTION__, pin>>5, pin&0x1F, pinsel, pinmode);
		printk(" already used in mode=%ld\n", (TAB_PINSEL[reg] >> shift) & 3);
	}
	else {
//		printk(" already configured\n");
	}
	if(pinmode>=0) {
		TAB_PINMODE[reg] = (TAB_PINMODE[reg] & ~reg_mask) | reg_pinmode;
	}
//	printk("%s SEL%p=%08lx MODE%p=%08lx\n", __FUNCTION__, &TAB_PINSEL[reg], TAB_PINSEL[reg], &TAB_PINMODE[reg], TAB_PINMODE[reg]);
 	return 0;
}
EXPORT_SYMBOL(lpc22xx_set_periph);

int lpc22xx_get_gpio_pinsel(unsigned pin)
{
	unsigned reg = pin >> 4;
	unsigned shift = ((pin & 15) * 2);

	return (TAB_PINSEL[reg] >> shift) & 3;
}

int lpc22xx_get_gpio_pinmode(unsigned pin)
{
	unsigned reg = pin >> 4;
	unsigned shift = ((pin & 15) * 2);

	return (TAB_PINMODE[reg] >> shift) & 3;
}

int __init_or_module lpc22xx_set_gpio_input(unsigned pin, int mode)
{
	unsigned port = pin >> 5;
	unsigned mask = ((pin & 31) * 1);

	lpc22xx_set_periph(pin, 0, mode);

	lpc22xx_fio_registers->port[port].REG_GPIO_FIODIR &= ~mask;
	return 0;
}
EXPORT_SYMBOL(lpc22xx_set_gpio_input);

int __init_or_module lpc22xx_set_gpio_output(unsigned pin, int value)
{
	unsigned port = pin >> 5;
	unsigned mask = ((pin & 31) * 1);

	lpc22xx_set_periph(pin, 0, 2);

	lpc22xx_fio_registers->port[port].REG_GPIO_FIODIR |= mask;

	gpio_set_value(pin, value);

	return 0;
}
EXPORT_SYMBOL(lpc22xx_set_gpio_output);

int lpc22xx_get_gpio_dir(unsigned pin)
{
	unsigned port = pin >> 5;
	unsigned shift = (pin & 31) * 1;

	return (lpc22xx_fio_registers->port[port].REG_GPIO_FIODIR >> shift ) & 1;
}
int lpc22xx_get_gpio_mask(unsigned pin)
{
	unsigned port = pin >> 5;
	unsigned shift = (pin & 31) * 1;

	return (lpc22xx_fio_registers->port[port].REG_GPIO_FIOMASK >> shift ) & 1;
}
/*--------------------------------------------------------------------------*/

int gpio_get_value(unsigned gpio)
{
	unsigned port = gpio >> 5;
	unsigned mask = 1 << ((gpio & 31) * 1);

	return (lpc22xx_fio_registers->port[port].REG_GPIO_FIOPIN & mask) != 0;
}
EXPORT_SYMBOL(gpio_get_value);

void gpio_set_value(unsigned gpio, int value)
{
	unsigned port = gpio >> 5;
	unsigned mask = 1 << ((gpio & 31) * 1);

	if (value)
		lpc22xx_fio_registers->port[port].REG_GPIO_FIOSET = mask;
	else
		lpc22xx_fio_registers->port[port].REG_GPIO_FIOCLR = mask;
}
EXPORT_SYMBOL(gpio_set_value);

