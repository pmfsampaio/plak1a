/*
 * drivers/video/ili932xfb.c
 *
 * Author Pedro Sampaio
 * Copyright (C) 2009
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 */
#include <linux/clk.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/clk.h>
#include <linux/ioport.h>
#include <linux/slab.h>		           /* kmalloc() */
#include <linux/fs.h>		           /* everything... */
#include <linux/types.h>	           /* size_t */
#include <linux/fcntl.h>	           /* O_ACCMODE */
#include <linux/cdev.h>                    /*cdev...*/
#include <linux/interrupt.h>               /*request_irq()...*/
#include <asm/irq.h>                       /*irq no.,etc.*/
#include <linux/irq.h>

#include <asm/io.h>                        /*writel(),readl()...*/
#include <asm/system.h>		           /* cli(), *_flags */
#include <linux/wait.h>                    /*wake_up_interrupkbble()...*/
#include <linux/delay.h>                   /*udelay()*/
#include <linux/device.h>                  /*class_create(),class_device_create()...*/
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <asm/mach/arch.h>

#include <asm/mach/map.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>                 /*proc fs*/
#include <asm/gpio.h>
#include <asm/errno.h>
#include <linux/fb.h>

#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include "ili932x.h"



static struct fb_fix_screeninfo ili932xfb_fix __initdata = {
	.id = "ili932x",
	.type = FB_TYPE_PACKED_PIXELS,
	.visual = FB_VISUAL_TRUECOLOR,
	.xpanstep = 0,
	.ypanstep = 0,
	.ywrapstep = 0,
	
#if ILI9325_BPP		== 16
	.line_length = ILI9325_X_RES * 2,
#elif ILI9325_BPP	== 32
	.line_length = ILI9325_X_RES * 4,
#endif
	.accel = FB_ACCEL_NONE,
};

static struct fb_var_screeninfo ili932xfb_var __initdata = {
	.xres = ILI9325_X_RES,
	.yres = ILI9325_Y_RES,
	.xres_virtual = ILI9325_X_RES,
	.yres_virtual = ILI9325_Y_RES,
	.bits_per_pixel = ILI9325_BPP,
	
#if ILI9325_BPP		== 16
	.blue = { 0, 5, 0 },
    .green = { 5, 6, 0 },
    .red = { 11, 5, 0 },
#elif ILI9325_BPP		== 32
	.blue = { 0, 8, 0 },
    .green = { 8, 8, 0 },
    .red = { 16, 8, 0 },
#endif

	.left_margin = 0,
	.right_margin = 0,
	.upper_margin = 0,
	.lower_margin = 0,
	.vmode = FB_VMODE_NONINTERLACED,
};

static int ili932xfb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
#if 0
	return vm_insert_page(vma, vma->vm_start,
		virt_to_page(ili932x_buffer));
#endif
	unsigned long size = vma->vm_end - vma->vm_start,
		      start = vma->vm_start;
	void *pos = ili932x_buffer;

	
	while (size > 0) {
		if (vm_insert_page(vma, start, vmalloc_to_page(pos)))
			return -EAGAIN;
		start += PAGE_SIZE;
		pos += PAGE_SIZE;
		if (size > PAGE_SIZE)
			size -= PAGE_SIZE;
		else
			size = 0;
	}


	return 0;
}

ssize_t ili932xfb_sys_write(struct fb_info *info, const char __user *buf,
		     size_t count, loff_t *ppos)
{
	ssize_t res;

	res = fb_sys_write(info, buf, count, ppos);
	ili932x_change = 1;
	return res;
}

void ili932xfb_sys_imageblit(struct fb_info *p, const struct fb_image *image)
{
	sys_imageblit(p, image);
	ili932x_change = 1;
}

void ili932xfb_sys_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
	sys_copyarea(p, area);
	ili932x_change = 1;
}

void ili932xfb_sys_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
	sys_fillrect(p, rect);
	ili932x_change = 1;
}

static struct fb_ops ili932xfb_ops = {
	.owner = THIS_MODULE,
	.fb_read = fb_sys_read,
	.fb_write = ili932xfb_sys_write,
	.fb_fillrect = ili932xfb_sys_fillrect,
	.fb_copyarea = ili932xfb_sys_copyarea,
	.fb_imageblit = ili932xfb_sys_imageblit,
	.fb_mmap = ili932xfb_mmap,
};

static int __init ili932xfb_probe(struct platform_device *device)
{
	int ret = -EINVAL;
 	struct fb_info *info = framebuffer_alloc(0, &device->dev);

	if (!info)
		goto none;

	info->screen_base = (char __iomem *) ili932x_buffer;
	info->screen_size =  ILI9325_FB_SIZE;
	info->fbops = &ili932xfb_ops;
	info->fix = ili932xfb_fix;
	info->var = ili932xfb_var;
	info->pseudo_palette = NULL;
	info->par = NULL;
	info->flags = FBINFO_FLAG_DEFAULT;
	
	info->fix.smem_start = ILI9325_BASE_INDEX;
	info->fix.smem_len = ILI9325_IOMEM_SIZE;// ILI9325_FB_SIZE;
	
	if (register_framebuffer(info) < 0)
		goto fballoced;

	platform_set_drvdata(device, info);

	printk(KERN_INFO "fb%d: %s frame buffer device\n", info->node,
		info->fix.id);

#if 0
	if (fb_prepare_logo(info, FB_ROTATE_UR)) {
		/* Start display and show logo on boot */
		fb_set_cmap(&(info->cmap),
				info);

		fb_show_logo(info, FB_ROTATE_UR);
	}
#endif
	return 0;

fballoced:
	framebuffer_release(info);

none:
	return ret;
}

static int ili932xfb_remove(struct platform_device *device)
{
	struct fb_info *info = platform_get_drvdata(device);

	if (info) {
		unregister_framebuffer(info);
		framebuffer_release(info);
	}

	return 0;
}

static struct platform_driver ili932xfb_driver = {
	.probe	= ili932xfb_probe,
	.remove = ili932xfb_remove,
	.driver = {
		.name	= ILI932X_NAME,
	},
};

static struct platform_device *ili932xfb_device;

static int __init ili932xfb_init(void)
{
	int ret = -EINVAL;

	/* ili932x_init() must be called first */
	if (!ili932x_isinited()) {
		printk(KERN_ERR ILI932X_NAME ": ERROR: "
			"ili932x is not initialized\n");
		goto none;
	}

	if (ili932x_enable()) {
		printk(KERN_ERR ILI932X_NAME ": ERROR: "
			"can't enable ili932x refreshing (being used)\n");
		return -ENODEV;
	}
	ret = platform_driver_register(&ili932xfb_driver);

	if (!ret) {
		ili932xfb_device =
			platform_device_alloc(ILI932X_NAME, 0);

		if (ili932xfb_device) {
			ret = platform_device_add(ili932xfb_device);
		} else {
			ret = -ENOMEM;
		}

		if (ret) {
			platform_device_put(ili932xfb_device);
			platform_driver_unregister(&ili932xfb_driver);
		}
	}

none:
	return ret;
}

static void __exit ili932xfb_exit(void)
{
	platform_device_unregister(ili932xfb_device);
	platform_driver_unregister(&ili932xfb_driver);
	ili932x_disable();
}

module_init(ili932xfb_init);
module_exit(ili932xfb_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Pedro Sampaio");
MODULE_DESCRIPTION("ili932x LCD framebuffer driver");

