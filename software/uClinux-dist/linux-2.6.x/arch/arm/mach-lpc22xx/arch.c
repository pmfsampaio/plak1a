/*
 *  linux/arch/arm/mach-lpc22xx/arch.c
 *
 *  Copyright (C) 2004 Philips Semiconductors
 *
 *  Architecture specific fixups.  This is where any
 *  parameters in the params struct are fixed up, or
 *  any additional architecture specific information
 *  is pulled from the params struct.
 */
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysdev.h>
#include <linux/i2c.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>

#include <linux/tty.h>
#include <asm/elf.h>
#include <linux/root_dev.h>
#include <linux/initrd.h>

#include <linux/platform_device.h>
#include <linux/smsc911x.h>


/* LAN9118 */

static struct resource plak1a_smc911x_resource[] = {
		[0] = {
			.start = 0x82000000,
			.end   = 0x820000ff,
			.flags = IORESOURCE_MEM,
		},
		[1] = {
			.start = 0,
			.end   = 0,
			.flags = IORESOURCE_IRQ,
		}
};

static struct platform_device plak1a_device_smc911x = {
	.name		= "smc911x",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(plak1a_smc911x_resource),
	.resource	= plak1a_smc911x_resource
};

/* ET032001DH9 */

static struct resource plak1a_et032001_resource[] = {
	[0] = {
		.start = 0x83000000,
		.end   = 0x83000042,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device plak1a_device_et032001 = {
	.name		= "et032001fb",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(plak1a_et032001_resource),
	.resource	= plak1a_et032001_resource
};



static struct platform_device *plak1a_devices[] __initdata = {
	&plak1a_device_smc911x,
//	&plak1a_device_et032001,

};

static struct i2c_board_info __initdata plak1a_i2c_board_info[] = {
 {
  .type = "ad7991",
  .addr = 0x28,
  .irq = LPC22xx_INTERRUPT_I2C,
 },
};


inline void ioport_unmap(void __iomem *p) {
}

EXPORT_SYMBOL(ioport_unmap);

void __iomem *ioport_map(unsigned long port, unsigned int nr)
{
	printk("*********** ERROR ****************");
	return NULL;
}
EXPORT_SYMBOL(ioport_map);


extern void __init lpc22xx_init_irq(void);

void __init lpc22xx_init_machine(void)
{
	platform_add_devices(plak1a_devices, ARRAY_SIZE(plak1a_devices));
	i2c_register_board_info(0, plak1a_i2c_board_info, ARRAY_SIZE(plak1a_i2c_board_info));
}

extern struct sys_timer lpc22xx_timer;

MACHINE_START(LPC22XX, "LPC22xx, PHILIPS ELECTRONICS Co., Ltd.")
/*	MAINTAINER(" Lucy Wang <mcu.china@philips.com>")    */
	.init_irq = lpc22xx_init_irq,
	.timer	= &lpc22xx_timer,
	.init_machine = lpc22xx_init_machine
MACHINE_END
