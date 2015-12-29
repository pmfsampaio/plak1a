/* drivers/video/s1d13xxxfb.c
 *
 * (c) 2004 Simtec Electronics
 * (c) 2005 Thibaut VARENE <varenet@parisc-linux.org>
 *
 * Driver for Epson S1D13xxx series framebuffer chips
 *
 * Adapted from
 *  linux/drivers/video/skeletonfb.c
 *  linux/drivers/video/epson1355fb.c
 *  linux/drivers/video/epson/s1d13xxxfb.c (2.4 driver by Epson)
 *
 * Note, currently only tested on S1D13806 with 16bit CRT.
 * As such, this driver might still contain some hardcoded bits relating to
 * S1D13806.
 * Making it work on other S1D13XXX chips should merely be a matter of adding
 * a few switch()s, some missing glue here and there maybe, and split header
 * files.
 *
 * TODO: - handle dual screen display (CRT and LCD at the same time).
 *	 - check_var(), mode change, etc.
 *	 - PM untested.
 *	 - Accelerated interfaces.
 *	 - Probably not SMP safe :)
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/fb.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/hardware.h>

#include <video/et0320fb.h>

#define PFX "et0320fb: "

#if 1
#define dbg(fmt, args...) do { printk(KERN_INFO fmt, ## args); } while(0)
#else
#define dbg(fmt, args...) do { } while (0)
#endif

/*
 * Here we define the default struct fb_fix_screeninfo
 */
static struct fb_fix_screeninfo __devinitdata et0320fb_fix = {
		.id = "et0320",
		.type = FB_TYPE_PACKED_PIXELS,
		.visual = FB_VISUAL_TRUECOLOR,
		.xpanstep = 0,
		.ypanstep = 0,
		.ywrapstep = 0,
		.line_length = 240 * 2,
		.accel = FB_ACCEL_NONE,
};

unsigned long Convert16To18(unsigned int val) {
	unsigned int vl, vh;

	vl = (val << 1) & 0x1fe;
	vh = (val << 2) & 0x3fc00;

	return vh | vl;
}

#define LCDTO16Bit(_v) 	(((_v) >> 1) & 0xff) | (((_v) >> 2) & 0xff00)

static int et0320fb_writereg(struct et0320fb_par *par, u16 regno, u32 value) {
	void __iomem *index = par->regs;

	writel(Convert16To18(regno), index);
	writel(Convert16To18(value), index + 4);
	return 0;
}

static int et0320fb_readreg(struct et0320fb_par *par, u16 regno) {
	u32 aux;

	void __iomem *index = par->regs;//dev->membase;
	void __iomem *data_base = par->regs + 4;

	udelay(50);
	writel(regno, index);
	udelay(50);
	aux = readl(data_base);
	return LCDTO16Bit(aux);
}

static inline void et0320fb_cfghw(void) {
	PINSEL1 &= ~(0x3 << 28);
	IO0DIR |= (1 << 30);
	//	IO0SET = (1 << 30);
	IO0CLR = (1 << 30);

	/*
	 *  0010 0000 0000 0000 0000 1100 0010 0000
	 * |    |    |    |    |    |    |    |    |
	 *                       WST2   WST1   IDLE
	 * |00xx|RWP0|----|----|wwww|wBww|www-|iiii|
	 *    |  ||||                 |
	 *    |  |||+-- BUSERR        +-- RBLE
	 *    |  ||+--- WPERR
	 *    |  |+---- WRITE PROTECT
	 *    |  +----- Burst-ROM
	 *    +-------- 00 - 8 / 01 - 16 / 10 - 32 bits
	 */
	BCFG3 = 0x200fffe0;// 0x20000C20;
}

static void ili9320_chip_init(struct fb_info *info) {
	IO0SET = (1 << 30);

	/*** Setup display for ILI9320 driver IC ***/
	et0320fb_writereg(info->par, 0x00, 0x0001);
	et0320fb_writereg(info->par, 0xe5, 0x8000); //start osc
	et0320fb_writereg(info->par, 0x00, 0x0001);
	msleep(10);

	et0320fb_writereg(info->par, 0x01, 0x0100); //Driver output control (1)
	et0320fb_writereg(info->par, 0x02, 0x0700); //LCD driving control
	et0320fb_writereg(info->par, 0x03, 0x1030); //Entry mode
	et0320fb_writereg(info->par, 0x04, 0x0000); //Resize control
	et0320fb_writereg(info->par, 0x08, 0x0202); //Display control (2)
	et0320fb_writereg(info->par, 0x09, 0x0000); //Display control (3)

	/*** Power Control ***/
	// et0320fb_writereg(info->par, 0x07, 0x0101);			//power control 1 BT, AP
	et0320fb_writereg(info->par, 0x17, 0x0001);
	et0320fb_writereg(info->par, 0x10, 0x0000);
	et0320fb_writereg(info->par, 0x11, 0x0007); //power control 2 DC, VC
	et0320fb_writereg(info->par, 0x12, 0x0000); //power control 3 VRH
	et0320fb_writereg(info->par, 0x13, 0x0000); //power control 4 VCOM amplitude
	msleep(20);

	// 0x17B0				//power control 1 BT,AP
	et0320fb_writereg(info->par, 0x10, 0x16B0);          //power control 1 BT,AP
	et0320fb_writereg(info->par, 0x11, 0x0037);          //power control 2 DC,VC
	//et0320fb_writereg(info->par, 0x11, 0x0007); //power control 2 DC,VC
	msleep(50);
	// 0x013E   	      	//power control 3 VRH
	// 0x013B	    	  	//power control 3 VRH
	et0320fb_writereg(info->par, 0x12, 0x013A); 			      //power control 3 VRH
	msleep(50);
	// 0x1800 				//power control 4 vcom amplitude
	et0320fb_writereg(info->par, 0x13, 0x1A00);          //power control 4 vcom amplitude

	// 0x000F  		        //power control 7 VCOMH
	et0320fb_writereg(info->par, 0x29, 0x0018); //power control 7 VCOMH
	msleep(50);
	et0320fb_writereg(info->par, 0x20, 0x0000); //Horizontal GRAM Address Set
	et0320fb_writereg(info->par, 0x21, 0x0000); //Vertical GRAM Address Set

	et0320fb_writereg(info->par, 0x50, 0x0000); //Horizontal Address Start Position
	et0320fb_writereg(info->par, 0x51, 0x00EF); //Horizontal Address end Position (239)
	et0320fb_writereg(info->par, 0x52, 0x0000); //Vertical Address Start Position
	et0320fb_writereg(info->par, 0x53, 0x013F); //Vertical Address end Position (319)

	et0320fb_writereg(info->par, 0x60, 0x2700); //Driver Output Control 2
	et0320fb_writereg(info->par, 0x61, 0x0001); //Base Image Display Control
	et0320fb_writereg(info->par, 0x6a, 0x0000); //Base Image Display Control

	et0320fb_writereg(info->par, 0x90, 0x0010); //Panel Interface Control 1
	et0320fb_writereg(info->par, 0x92, 0x0000); //Panel Interface Control 2
	et0320fb_writereg(info->par, 0x93, 0x0000); //Panel Interface Control 3

	/*** GAMMA Control ***/
	et0320fb_writereg(info->par, 0x30, 0x0007);
	et0320fb_writereg(info->par, 0x31, 0x0403);
	et0320fb_writereg(info->par, 0x32, 0x0404);
	et0320fb_writereg(info->par, 0x35, 0x0002);
	et0320fb_writereg(info->par, 0x36, 0x0707);
	et0320fb_writereg(info->par, 0x37, 0x0606);
	et0320fb_writereg(info->par, 0x38, 0x0106);
	et0320fb_writereg(info->par, 0x39, 0x0007);
	et0320fb_writereg(info->par, 0x3c, 0x0700);
	et0320fb_writereg(info->par, 0x3d, 0x0707);

	et0320fb_writereg(info->par, 0x07, 0x0173);
	msleep(50);

	return;
}

static void et0320fb_dpy_update(struct fb_info *info);

static void et0320fb_fillrect(struct fb_info *info,
				   const struct fb_fillrect *rect)
{
	sys_fillrect(info, rect);

	et0320fb_dpy_update(info);
}

static void et0320fb_copyarea(struct fb_info *info,
				   const struct fb_copyarea *area)
{
	sys_copyarea(info, area);

	et0320fb_dpy_update(info);
}

static void et0320fb_imageblit(struct fb_info *info,
				const struct fb_image *image)
{
	sys_imageblit(info, image);

	et0320fb_dpy_update(info);
}

/*
 * this is the slow path from userspace. they can seek and write to
 * the fb. it's inefficient to do anything less than a full screen draw
 */
static ssize_t et0320fb_write(struct fb_info *info, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned long p;
	int err=-EINVAL;
	struct et0320fb_par *par;
	unsigned int xres;
	unsigned int fbmemlength;

	p = *ppos;
	par = info->par;
	xres = info->var.xres;
	fbmemlength = (xres * info->var.yres)/8;

	if (p > fbmemlength)
		return -ENOSPC;

	err = 0;
	if ((count + p) > fbmemlength) {
		count = fbmemlength - p;
		err = -ENOSPC;
	}

	if (count) {
		char *base_addr;

		base_addr = (char __force *)info->screen_base;
		count -= copy_from_user(base_addr + p, buf, count);
		*ppos += count;
		err = -EFAULT;
	}

	et0320fb_dpy_update(info);

	if (count)
		return count;

	return err;
}

void GLcdWriteData(struct et0320fb_par *par, unsigned char byte1, unsigned char byte0) {
	void __iomem *index = par->regs;

	writel(Convert16To18(byte1 << 8 | byte0), index + 4);
}

void GLcdWriteCmd(struct et0320fb_par *par, unsigned char cmd) {
	void __iomem *index = par->regs;

	writel(Convert16To18(cmd), index);
}

static void et0320fb_dpy_update(struct fb_info *info)
{
	unsigned char *buf = (unsigned char __force *)info->screen_base;
	unsigned address = 0;
	register long counter;
	unsigned int color, temp;
	struct et0320fb_par *par = info->par;
	int x, y;
	unsigned char *flashAddress;

	flashAddress = buf;
	for (y = 0; y < 320; y++) {
		//            SetAddress(address.v[2],address.v[1],address.v[0]);
		GLcdWriteCmd(par, 0x20);
		GLcdWriteData(par, 0, (address & 0xff));
		GLcdWriteCmd(par, 0x21);
		GLcdWriteData(par, (address & 0xff0000) >> 16, (address & 0xff00) >> 8);
		GLcdWriteCmd(par, 0x22);
		for (x = 0; x < 240; x++) {
			// Read pixels from flash
			temp = *flashAddress;
//			temp = ((temp >> 8) & 0xff) | ((temp << 8) & 0xff00);
			flashAddress++;
			color = temp << 1 | ((temp & 0x10)>> 4);
			color = (color & 0x0fff) | ((color << 1) & 0x3e000);
			color = color | ((color & 0x2000) >> 5);
			writel(color, par->regs + 4);
		}
		address += LINE_MEM_PITCH;
	}
}

/* We implement our own mmap to set MAY_SHARE and add the correct size */
static int et0320fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	vma->vm_flags |= VM_MAYSHARE| VM_SHARED;

	vma->vm_start = info->fix.smem_start;
	vma->vm_end   = info->fix.smem_start + info->fix.smem_len;
	return 0;
}

/* framebuffer information structures */

static struct fb_ops et0320fb_fbops = {
		.owner		= THIS_MODULE,
//		.fb_read        = fb_sys_read,
		.fb_write	= et0320fb_write,
		.fb_fillrect	= et0320fb_fillrect,
		.fb_copyarea	= et0320fb_copyarea,
		.fb_imageblit	= et0320fb_imageblit,
		.fb_mmap	= et0320fb_mmap,
};

#if 0
		.owner = THIS_MODULE,
		.fb_set_par = et0320fb_set_par,
		.fb_setcolreg = et0320fb_setcolreg,
		.fb_blank = et0320fb_blank,

//		.fb_pan_display = et0320fb_pan_display,

		/* to be replaced by any acceleration we can */
		.fb_fillrect = cfb_fillrect,
		.fb_copyarea = cfb_copyarea,
		.fb_imageblit = cfb_imageblit,
};
#endif

/**
 *      et0320fb_fetch_hw_state - Configure the framebuffer according to
 *	hardware setup.
 *      @info: frame buffer structure
 *
 *	We setup the framebuffer structures according to the current
 *	hardware setup. On some machines, the BIOS will have filled
 *	the chip registers with such info, on others, these values will
 *	have been written in some init procedure. In any case, the
 *	software values needs to match the hardware ones. This is what
 *	this function ensures.
 *
 *	Note: some of the hardcoded values here might need some love to
 *	work on various chips, and might need to no longer be hardcoded.
 */
static void __devinit
et0320fb_hwinit(struct fb_info *info) {
	struct fb_var_screeninfo *var = &info->var;
	struct fb_fix_screeninfo *fix = &info->fix;
	u32 xres, yres;
	u32 xres_virtual, yres_virtual;

//	fix->type = FB_TYPE_PACKED_PIXELS;

//	info->fix.visual = FB_VISUAL_TRUECOLOR;
	info->var.bits_per_pixel = 16;

	info->var.red.length = 5;
	info->var.red.offset = 11;

	info->var.green.length = 6;
	info->var.green.offset = 5;

	info->var.blue.length = 5;
	info->var.blue.offset = 0;
	fb_alloc_cmap(&info->cmap, 256, 0);
	xres = 240;
	yres = 320;
	xres_virtual = 240;
	yres_virtual = 320;

	var->xres = xres;
	var->yres = yres;
	var->xres_virtual = xres_virtual;
	var->yres_virtual = yres_virtual;
	var->xoffset = var->yoffset = 0;

//	fix->line_length = 240 * 2;

	var->grayscale = 0;

	var->activate = FB_ACTIVATE_NOW;

	dbg(PFX "bpp=%d, xres=%d, yres=%d, vxres=%d, vyres=%d\n",
			var->bits_per_pixel, xres, yres, xres_virtual, yres_virtual);

	ili9320_chip_init(info);
}

static int et0320fb_remove(struct platform_device *pdev) {
	struct fb_info *info = platform_get_drvdata(pdev);
	struct et0320fb_par *par = NULL;

	if (info) {
		par = info->par;
		if (par && par->regs) {
			/* disable output & enable powersave */
			iounmap(par->regs);
		}

		fb_dealloc_cmap(&info->cmap);

		if (info->screen_base)
			iounmap(info->screen_base);

		framebuffer_release(info);
	}

	release_mem_region(pdev->resource[0].start,
			pdev->resource[0].end - pdev->resource[0].start +1);
	return 0;
}

static int __devinit
et0320fb_probe(struct platform_device *pdev) {
	struct et0320fb_par *default_par;
	struct fb_info *info;
	int ret = 0;
	u16 code;
	char *p;

	printk(KERN_INFO "ET0320 Framebuffer Driver\n");

	if (pdev->num_resources != 1) {
		dev_err(&pdev->dev, "invalid num_resources: %i\n",
				pdev->num_resources);
		ret = -ENODEV;
		goto bail;
	}

	/* resource[0] is registers */
	if (pdev->resource[0].flags != IORESOURCE_MEM) {
		dev_err(&pdev->dev, "invalid resource type\n");
		ret = -ENODEV;
		goto bail;
	}

	if (!request_mem_region(pdev->resource[0].start,
			pdev->resource[0].end - pdev->resource[0].start +1, "et0320fb regs")) {
		dev_dbg(&pdev->dev, "request_mem_region failed\n");
		ret = -EBUSY;
		goto bail;
	}
	info = framebuffer_alloc(sizeof(struct et0320fb_par) + sizeof(u32) * 256,
			&pdev->dev);
	if (!info) {
		ret = -ENOMEM;
		goto bail;
	}

	platform_set_drvdata(pdev, info);
	default_par = info->par;
	default_par->regs = ioremap_nocache(pdev->resource[0].start,
			pdev->resource[0].end - pdev->resource[0].start + 1);
	if (!default_par->regs) {
		printk(KERN_ERR PFX "unable to map registers\n");
		ret = -ENOMEM;
		goto bail;
	}
//	info->pseudo_palette = 0;//default_par->pseudo_palette;

	if (!(info->screen_base = (char __iomem *) vmalloc(240 * 320 * 2))) {
		printk(KERN_ERR PFX "unable to alloc video memory\n");
		ret = -ENOMEM;
		goto bail;
	}
	if (!info->screen_base) {
		printk(KERN_ERR PFX "unable to map framebuffer\n");
		ret = -ENOMEM;
		goto bail;
	}

	memset(info->screen_base, 0, 240 * 320 * 2);
	info->screen_size = 240 * 320;

	et0320fb_cfghw();

	code = et0320fb_readreg(default_par, ILI9325_DRV_CODE);
	if (code != ILI9320_CHIP_ID) {
		printk(KERN_INFO PFX "chip not found: %i\n", code);
		ret = -ENODEV;
		goto bail;
	}

	info->fix = et0320fb_fix;
	info->fix.mmio_start = pdev->resource[0].start;
	info->fix.mmio_len = pdev->resource[0].end - pdev->resource[0].start + 1;
	info->fix.smem_start = (u32 )&info->screen_base;
	info->fix.smem_len = 240 * 320 * 2;

	printk(KERN_INFO PFX "regs mapped at 0x%p, fb %d bytes mapped at 0x%p\n",
			default_par->regs, info->fix.smem_len , info->screen_base);

	info->par = default_par;
	info->fbops = &et0320fb_fbops;
	info->flags = FBINFO_FLAG_DEFAULT;

	et0320fb_hwinit(info);

	if (register_framebuffer(info) < 0) {
		ret = -EINVAL;
		goto bail;
	}

	printk(KERN_INFO "fb%d: %s frame buffer device\n",
			info->node, info->fix.id);

	if (fb_prepare_logo(info, FB_ROTATE_UR)) {
		/* Start display and show logo on boot */
		fb_set_cmap(&(info->cmap),
				info);

		fb_show_logo(info, FB_ROTATE_UR);
	}

	return 0;

	bail: et0320fb_remove(pdev);
	return ret;

}

#ifdef CONFIG_PM
static int et0320fb_suspend(struct platform_device *dev, pm_message_t state)
{
	struct fb_info *info = platform_get_drvdata(dev);
	struct et0320fb_par *s1dfb = info->par;
	struct et0320fb_pdata *pdata = NULL;

	/* disable display */
	lcd_enable(s1dfb, 0);
	crt_enable(s1dfb, 0);

	if (dev->dev.platform_data)
	pdata = dev->dev.platform_data;

#if 0
	if (!s1dfb->disp_save)
	s1dfb->disp_save = kmalloc(info->fix.smem_len, GFP_KERNEL);

	if (!s1dfb->disp_save) {
		printk(KERN_ERR PFX "no memory to save screen");
		return -ENOMEM;
	}

	memcpy_fromio(s1dfb->disp_save, info->screen_base, info->fix.smem_len);
#else
	s1dfb->disp_save = NULL;
#endif

	if (!s1dfb->regs_save)
	s1dfb->regs_save = kmalloc(info->fix.mmio_len, GFP_KERNEL);

	if (!s1dfb->regs_save) {
		printk(KERN_ERR PFX "no memory to save registers");
		return -ENOMEM;
	}

	/* backup all registers */
	memcpy_fromio(s1dfb->regs_save, s1dfb->regs, info->fix.mmio_len);

	/* now activate power save mode */
	et0320fb_writereg(s1dfb, S1DREG_PS_CNF, 0x11);

	if (pdata && pdata->platform_suspend_video)
	return pdata->platform_suspend_video();
	else
	return 0;
}

static int et0320fb_resume(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);
	struct et0320fb_par *s1dfb = info->par;
	struct et0320fb_pdata *pdata = NULL;

	/* awaken the chip */
	et0320fb_writereg(s1dfb, S1DREG_PS_CNF, 0x10);

	/* do not let go until SDRAM "wakes up" */
	while ((et0320fb_readreg(s1dfb, S1DREG_PS_STATUS) & 0x01))
	udelay(10);

	if (dev->dev.platform_data)
	pdata = dev->dev.platform_data;

	if (s1dfb->regs_save) {
		/* will write RO regs, *should* get away with it :) */
		memcpy_toio(s1dfb->regs, s1dfb->regs_save, info->fix.mmio_len);
		kfree(s1dfb->regs_save);
	}

	if (s1dfb->disp_save) {
		memcpy_toio(info->screen_base, s1dfb->disp_save,
				info->fix.smem_len);
		kfree(s1dfb->disp_save); /* XXX kmalloc()'d when? */
	}

	if ((s1dfb->display & 0x01) != 0)
	lcd_enable(s1dfb, 1);
	if ((s1dfb->display & 0x02) != 0)
	crt_enable(s1dfb, 1);

	if (pdata && pdata->platform_resume_video)
	return pdata->platform_resume_video();
	else
	return 0;
}
#endif /* CONFIG_PM */

static struct platform_driver et0320fb_driver = {
		.probe = et0320fb_probe,
		.remove = et0320fb_remove,
#ifdef CONFIG_PM
		.suspend = et0320fb_suspend,
		.resume = et0320fb_resume,
#endif
		.driver = {
			.name = ET0320_DEVICENAME,
		}, };

static int __init et0320fb_init(void) {
	if (fb_get_options("et0320fb", NULL))
		return -ENODEV;

	return platform_driver_register(&et0320fb_driver);
}

static void __exit
et0320fb_exit(void) {
	platform_driver_unregister(&et0320fb_driver);
}

module_init(et0320fb_init);
module_exit(et0320fb_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Framebuffer driver for et0320 devices");
MODULE_AUTHOR("Pedro Sampaio");
