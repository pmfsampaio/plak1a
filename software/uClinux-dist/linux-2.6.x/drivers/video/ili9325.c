/*
 * drivers/video/ili932x.c
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
#include <linux/slab.h>
#include <linux/fs.h>
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
#include <asm/errno.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>                 /*proc fs*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/fb.h>

#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>

#include "ili932x.h"

struct ili932x_device 
{
	void __iomem *membase;
};

/*
 * Module Parameters
 */

#define CONFIG_ILI932X_RATE 10

static unsigned int ili932x_rate = CONFIG_ILI932X_RATE;
module_param(ili932x_rate, uint, S_IRUGO);
MODULE_PARM_DESC(ili932x_rate,
	"Refresh rate (hertzs)");

unsigned int ili932x_getrate(void)
{
	return ili932x_rate;
}

/*
 * Update work
 */

struct ili932x_device *lcd_ili932x = NULL;
//static  u16 ili932x_reg_cache[ILI9325_TIMING_CTRL_3 + 1];
unsigned char *ili932x_buffer;
static unsigned char *ili932x_cache;
unsigned char ili932x_change;
static DEFINE_MUTEX(ili932x_mutex);
static unsigned char ili932x_updating;
static void ili932x_update(struct work_struct *delayed_work);
static struct workqueue_struct *ili932x_workqueue;
static DECLARE_DELAYED_WORK(ili932x_work, ili932x_update);

static void ili932x_queue(void)
{
	queue_delayed_work(ili932x_workqueue, &ili932x_work,
		HZ / ili932x_rate);
}

unsigned char ili932x_enable(void)
{
	unsigned char ret;

	mutex_lock(&ili932x_mutex);
	
	if (!ili932x_updating) {
		ili932x_updating = 1;
		ili932x_queue();
		ret = 0;
	} else
		ret = 1;

	mutex_unlock(&ili932x_mutex);

	return ret;
}

void ili932x_disable(void)
{
	mutex_lock(&ili932x_mutex);

	if (ili932x_updating) {
		ili932x_updating = 0;
		cancel_delayed_work(&ili932x_work);
		flush_workqueue(ili932x_workqueue);
	}

	mutex_unlock(&ili932x_mutex);
}

unsigned char ili932x_isenabled(void)
{
	return ili932x_updating;
}

unsigned long Convert16To18(unsigned int val) {
	unsigned int vl, vh;

	vl = (val << 1) & 0x1fe;
	vh = (val << 2) & 0x3fc00;

	return vh | vl;
}

#define LCDTO16Bit(_v) 	(((_v) >> 1) & 0xff) | (((_v) >> 2) & 0xff00)


static inline void ili9325_write_pixel(struct ili932x_device *dev, u32 pixel)
{
#if ILI9325_BPP		== 32
	u8 r, g, b; 
	u32 color = 0;

	/* 24/32-bit colors to 18-bit colors.*/
	b = (pixel & 0xff) >> 2;
	g = ((pixel >> 8 ) & 0xff) >> 2;
	r = ((pixel >> 16) & 0xff ) >> 2;
	color = (r << 12) | (g << 6) | (b << 0);

	writel(color, dev->membase+4);
#elif ILI9325_BPP		== 16
	writel((((pixel & 0xf800) << 2) | ((pixel & 0x7ff) << 1)) | 0x01001, dev->membase+4);
#endif
}


static inline int ili932x_write_reg(struct ili932x_device *dev, u8 reg, u16 data)
{
	void __iomem *index = dev->membase;

	BUG_ON(reg > ILI9325_TIMING_CTRL_3);
	
	writel(Convert16To18(reg), index);
	writel(Convert16To18(data), index+4);

	//ili932x_reg_cache[reg] = data;

	return 0;
}

static inline int ili932x_read_reg(struct ili932x_device *dev, u8 reg)
{
	void __iomem *index = dev->membase;
	void __iomem *data_base = dev->membase;

	if (reg > ILI9325_DRV_CODE) {
		return 0;//ili932x_reg_cache[reg];
	} else {
		writel(reg, index);
		return LCDTO16Bit(readl(data_base+4));
	}
}

/* Power On sequence */
static void ili9325_pow_on(struct ili932x_device *dev)
{
	ili932x_write_reg(dev, ILI9325_POW_CTRL_1, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	ili932x_write_reg(dev, ILI9325_POW_CTRL_2, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0]  */
	ili932x_write_reg(dev, ILI9325_POW_CTRL_3, 0x0000); /* VREG1OUT voltage  */
	ili932x_write_reg(dev, ILI9325_POW_CTRL_4, 0x0000); /* VDV[4:0] for VCOM amplitude  */
	msleep(200); /* Dis-charge capacitor power voltage */

	ili932x_write_reg(dev, ILI9325_POW_CTRL_1, 0x1690); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	ili932x_write_reg(dev, ILI9325_POW_CTRL_2, 0x0227); /* R11h=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]  */
	msleep(50);

	ili932x_write_reg(dev, ILI9325_POW_CTRL_3, 0x001D); /* External reference voltage= Vci  */
	msleep(50);

	ili932x_write_reg(dev, ILI9325_POW_CTRL_4, 0x0800); /* R13=1D00 when R12=009D;VDV[4:0] for VCOM amplitude  */
	ili932x_write_reg(dev, ILI9325_POW_CTRL_7, 0x0012); /* R29=0013 when R12=009D;VCM[5:0] for VCOMH  */
	ili932x_write_reg(dev, ILI9325_FRM_RATE_COLOR, 0x000B); /* Frame Rate = 70Hz  */
	msleep(50);
}

/* Adjust the Gamma Curve  */
static void ili9325_gamma_adjust(struct ili932x_device *dev)
{
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_1, 0x0007);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_2, 0x0707);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_3, 0x0006);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_4, 0x0704);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_5, 0x1F04);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_6, 0x0004);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_7, 0x0000);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_8, 0x0706);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_9, 0x0701);
	ili932x_write_reg(dev, ILI9325_GAMMA_CTRL_10, 0x000F);
}

/*  Partial Display Control */
static void ili9325_par_dis_ctrl(struct ili932x_device *dev)
{
	ili932x_write_reg(dev, ILI9325_PAR_IMG1_POS, 0);
	ili932x_write_reg(dev, ILI9325_PAR_IMG1_START, 0);
	ili932x_write_reg(dev, ILI9325_PAR_IMG1_END, 0);
	ili932x_write_reg(dev, ILI9325_PAR_IMG2_POS, 0);
	ili932x_write_reg(dev, ILI9325_PAR_IMG2_START, 0);
	ili932x_write_reg(dev, ILI9325_PAR_IMG2_END, 0);
}

/* Panel Control */
static void ili9325_pan_ctrl(struct ili932x_device *dev)
{
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_1, 0x0010);
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_2, 0x0000);
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_3, 0x0003);
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_4, 0x0110);
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_5, 0x0000);
	ili932x_write_reg(dev, ILI9325_PAN_CTRL_6, 0x0000);
	ili932x_write_reg(dev, ILI9325_DIS_CTRL_1, 0x0133); /* 262K color and display ON */
}

/* Start Initial Sequence */
static void ili9325_chip_init(struct ili932x_device *dev)
{
	printk("ili9325_chip_init.\n");
	ili932x_write_reg(dev, ILI9325_TIMING_CTRL_1, 0x3008); /* Set internal timing*/
	ili932x_write_reg(dev, ILI9325_TIMING_CTRL_2, 0x0012); /* Set internal timing*/
	ili932x_write_reg(dev, ILI9325_TIMING_CTRL_3, 0x1231); /* Set internal timing*/

	ili932x_write_reg(dev, ILI9325_DRV_OUTPUT_CTRL_1, 0x0100); /* set SS and SM bit */
	ili932x_write_reg(dev, ILI9325_LCD_DRV_CTRL, 0x0700); /* set 1 line inversion */
	
#if ILI9325_BPP		== 16
	printk("ili9325_chip init bpp16.\n");
	ili932x_write_reg(dev, ILI9325_ENTRY_MOD, 0x1030); /* 16bit mode, set GRAM write direction and BGR=1. */
#elif ILI9325_BPP		== 32
	ili932x_write_reg(dev, ILI9325_ENTRY_MOD, 0x9030); /* 18bit mode, set GRAM write direction and BGR=1. */
#endif

	ili932x_write_reg(dev, ILI9325_RESIZE_CTRL, 0x0000); /* Resize register. */
	ili932x_write_reg(dev, ILI9325_DIS_CTRL_2, 0x0207); /* set the back porch and front porch. */
	ili932x_write_reg(dev, ILI9325_DIS_CTRL_3, 0x0000); /* set non-display area refresh cycle ISC[3:0]. */
	ili932x_write_reg(dev, ILI9325_DIS_CTRL_4, 0x0000); /* FMARK function. */
	ili932x_write_reg(dev, ILI9325_RGB_CTRL_1, 0x0000); /* RGB interface setting. */
	ili932x_write_reg(dev, ILI9325_FRAME_MARKER_POS, 0x0000); /* Frame marker Position. */
	ili932x_write_reg(dev, ILI9325_RGB_CTRL_2, 0x0000); /* RGB interface polarity. */

	ili9325_pow_on(dev);
	ili9325_gamma_adjust(dev);

	/* Set GRAM area */
	ili932x_write_reg(dev, ILI9325_HOR_ADDR_START, 0x0000); /* Horizontal GRAM Start Address. */
	ili932x_write_reg(dev, ILI9325_HOR_ADDR_END, 0x00EF); /* Horizontal GRAM End Address. */
	ili932x_write_reg(dev, ILI9325_VET_ADDR_START, 0x0000); /* Vertical GRAM Start Address. */
	ili932x_write_reg(dev, ILI9325_VET_ADDR_END, 0x013F); /* Vertical GRAM End Address. */
	ili932x_write_reg(dev, ILI9325_DRV_OUTPUT_CTRL_2, 0xA700); /* Gate Scan Line. */
	ili932x_write_reg(dev, ILI9325_BASE_IMG_CTRL, 0x0001); /* NDL,VLE, REV. */
	ili932x_write_reg(dev, ILI9325_VSCROLL_CTRL, 0x0000); /* set scrolling line. */

	ili9325_par_dis_ctrl(dev);
	ili9325_pan_ctrl(dev);

	ili932x_write_reg(dev, ILI9325_GRAM_HADDR, 0x0000); /* GRAM horizontal Address. */
	ili932x_write_reg(dev, ILI9325_GRAM_VADDR, 0x0000); /* GRAM Vertical Address. */

	writel(Convert16To18(ILI9325_GRAM_DATA), dev->membase);

	return;
}


static void ili9320_chip_init(struct ili932x_device *dev)
{
	IO0SET = (1 << 30);

	ili932x_write_reg(dev,0x00E5,0x8000);// Set the Vcore voltage and this setting is must.        
	ili932x_write_reg(dev,0x0000,0x0001);// Start internal OSC.                                  

	ili932x_write_reg(dev,0x0001,0x0100);// set SS and SM bit                                    
	ili932x_write_reg(dev,0x0002,0x0700);// set 1 line inversion                                 
	ili932x_write_reg(dev,0x0003,0x92b0);// TRI=0, DFM=0, set GRAM write direction and BGR=1.
	ili932x_write_reg(dev,0x0004,0x0000);// Resize register                                      
	ili932x_write_reg(dev,0x0008,0x0202);// set the back porch and front porch                   
	ili932x_write_reg(dev,0x0009,0x0000);// set non-display area refresh cycle ISC[3:0]          
	ili932x_write_reg(dev,0x000A,0x0000);// FMARK function                                       
	ili932x_write_reg(dev,0x000C,0x0000);// RGB interface setting                                
	ili932x_write_reg(dev,0x000D,0x0000);// Frame marker Position                                
	ili932x_write_reg(dev,0x000F,0x0000);// RGB interface polarity                               

	ili932x_write_reg(dev,0x0010,0x0000);// Power On sequence  //SAP, BT[3:0], AP, DSTB, SLP, STB
	ili932x_write_reg(dev,0x0011,0x0007);// DC1[2:0], DC0[2:0], VC[2:0]
	ili932x_write_reg(dev,0x0012,0x0000);// VREG1OUT voltage
	ili932x_write_reg(dev,0x0013,0x0000);// VDV[4:0] for VCOM amplitude
	msleep(200);
	ili932x_write_reg(dev,0x0010,0x17B0);// SAP, BT[3:0], AP, DSTB, SLP, STB 
	ili932x_write_reg(dev,0x0011,0x0007);// R11h=0x0001 at VCI=3.3V DC1[2:0], DC0[2: 
	msleep(50); 
	ili932x_write_reg(dev,0x0012,0x013E);// R11h=0x0138 at VCI=3.3V VREG1OUT voltage
	msleep(50); 

	ili932x_write_reg(dev,0x0013,0x1F00);// R11h=0x1800 at VCI=2.8V VDV[4:0] for VCO  
	ili932x_write_reg(dev,0x0029,0x0013);// setting VCM for VCOMH  0018-0012          
	msleep(50); 

	ili932x_write_reg(dev,0x0020,0x0000);// GRAM horizontal Address          
	ili932x_write_reg(dev,0x0021,0x0000);// GRAM Vertical Address            

	ili932x_write_reg(dev,0x0030,0x0000);// - Adjust the Gamma Curve -//     
	ili932x_write_reg(dev,0x0031,0x0404);                                    
	ili932x_write_reg(dev,0x0032,0x0404);                                    
	ili932x_write_reg(dev,0x0035,0x0004);                              
	ili932x_write_reg(dev,0x0036,0x0404);                                    
	ili932x_write_reg(dev,0x0037,0x0404);                                    
	ili932x_write_reg(dev,0x0038,0x0404);                                    
	ili932x_write_reg(dev,0x0039,0x0707);                                    
	ili932x_write_reg(dev,0x003C,0x0500);                                    
	ili932x_write_reg(dev,0x003D,0x0607);// - Adjust the Gamma Curve -//     

	//msleep(15); 
	ili932x_write_reg(dev,0x0050,0x0000);// Horizontal GRAM Start Address    
	ili932x_write_reg(dev,0x0051,0x00EF);// Horizontal GRAM End Address      
	ili932x_write_reg(dev,0x0052,0x0000);// Vertical GRAM Start Address      
	ili932x_write_reg(dev,0x0053,0x013F);// Vertical GRAM Start Address      
	ili932x_write_reg(dev,0x0060,0x2700);// Gate Scan Line                   
	ili932x_write_reg(dev,0x0061,0x0001);// NDL,VLE, REV                     
	ili932x_write_reg(dev,0x006A,0x0000);// set scrolling line               

	ili932x_write_reg(dev,0x0080,0x0000);//- Partial Display Control -//     
	ili932x_write_reg(dev,0x0081,0x0000);
	ili932x_write_reg(dev,0x0082,0x0000);
	ili932x_write_reg(dev,0x0083,0x0000);
	ili932x_write_reg(dev,0x0084,0x0000);
	ili932x_write_reg(dev,0x0085,0x0000);

	ili932x_write_reg(dev,0x0090,0x0010);//- Panel Control -//            
	ili932x_write_reg(dev,0x0092,0x0000);                                 
	ili932x_write_reg(dev,0x0093,0x0003);                                 
	ili932x_write_reg(dev,0x0095,0x0110);                                 
	ili932x_write_reg(dev,0x0097,0x0000);                                 
	ili932x_write_reg(dev,0x0098,0x0000);//- Panel Control -//            
	ili932x_write_reg(dev,0x0007,0x0133);//Display Control and display ON
	msleep(50); 
}

static inline void ili932x_write_gram(struct ili932x_device *dev, u8 *buf, size_t size)
{
	size_t i = 0;
	
#if ILI9325_BPP		== 16
	u16 *pixel_buf = (u16 *) buf;
	u8 bpp = sizeof(u16);
#elif ILI9325_BPP	== 32
	u32 *pixel_buf = (u32 *) buf;
	u8 bpp = sizeof(u32);
#endif

	ili932x_write_reg(dev,ILI9325_GRAM_HADDR,0x0000);// GRAM horizontal Address
	ili932x_write_reg(dev,ILI9325_GRAM_HADDR,0x0000);// GRAM Vertical Address
	writel(Convert16To18(ILI9325_GRAM_DATA), dev->membase);

	for (i=0; i<(size/bpp); i++) {
		ili9325_write_pixel(dev, pixel_buf[i]);
	}
}

static void ili932x_update(struct work_struct *work)
{
	if (ili932x_change) {
		ili932x_write_gram(lcd_ili932x, ili932x_buffer,
				ILI9325_FB_SIZE);
		ili932x_change = 0;
	}
/*	if (memcmp(ili932x_cache, ili932x_buffer, ILI9325_FB_SIZE)) {
		printk(".");
		ili932x_write_gram(lcd_ili932x, ili932x_buffer,
				ILI9325_FB_SIZE);	
		memcpy(ili932x_cache, ili932x_buffer, ILI9325_FB_SIZE);
	}
*/

	if (ili932x_updating) {
		ili932x_queue();
	}
}


/*
 * ili932x Exported Symbols
 */
EXPORT_SYMBOL_GPL(ili932x_getrate);
EXPORT_SYMBOL_GPL(ili932x_enable);
EXPORT_SYMBOL_GPL(ili932x_disable);
EXPORT_SYMBOL_GPL(ili932x_isenabled);
EXPORT_SYMBOL_GPL(ili932x_buffer);

/*
 * Is the module inited?
 */

static unsigned char ili932x_inited;
unsigned char ili932x_isinited(void)
{
	return ili932x_inited;
}
EXPORT_SYMBOL_GPL(ili932x_isinited);

static int ili932x_chip_probe(struct ili932x_device *dev)
{
	u16 chip_id = 0;

	/* Get chip ID. */
	chip_id = ili932x_read_reg(dev, ILI9325_DRV_CODE);
	printk("%s: read addr %08x, chip_id = %04x.\n",
			__FUNCTION__, dev->membase, chip_id);

	if (ILI9325_CHIP_ID == (u16) chip_id) {
		ili9325_chip_init(dev);
	} else if (ILI9320_CHIP_ID == (u16) chip_id) {
		ili9320_chip_init(dev);
	} else {
		printk("No Chip ILI9325/9320 found.\n");
		return -1;
	}
	printk("Chip ILI%4x found.\n", chip_id);
	return 0;
}

/*
 * Module Init & Exit
 */

static int __init ili932x_init(void)
{
	int ret = -EINVAL;
	void __iomem *membase = NULL;

	PINSEL1 &= ~(0x3 << 28);
	IO0DIR |= (1 << 30);
	IO0CLR = (1 << 30);
	BCFG3 = 0x200fffe0; // 0x20000C20; //0x200fffe0

	lcd_ili932x = kmalloc(sizeof(struct ili932x_device), GFP_KERNEL);
	if (!lcd_ili932x) {
		ret = -ENOMEM;
		goto alloc_failed; 
	}
	memset(lcd_ili932x, 0, sizeof(struct ili932x_device));	

#if 0
	if (!request_mem_region(ILI9325_BASE_INDEX, ILI9325_IOMEM_SIZE, "ili932x")) {
		printk("%s: request mem region error.\n", __FUNCTION__);
		ret = -1;
		goto region_failed;
	}
#endif

	membase = ioremap(ILI9325_BASE_INDEX, ILI9325_IOMEM_SIZE);
	if (!membase) {
		printk(KERN_ERR "Can NOT remap address 0x%08x\n", 
				(unsigned int)ILI9325_BASE_INDEX);
		ret = -1;
		goto remap_failed;
	}

	lcd_ili932x->membase = membase;
	printk("%s: phy addr %08x ==> virt %08x.\n", 
			__FUNCTION__, (unsigned int)ILI9325_BASE_INDEX, 
			(unsigned int)lcd_ili932x->membase);
	
	ili932x_buffer = (unsigned char *) vmalloc_32(ILI9325_FB_SIZE);
	if (ili932x_buffer == NULL) {
		printk(KERN_ERR ILI932X_NAME": ERROR: "
			"can't get a free page\n");
		ret = -ENOMEM;
		goto page_err;
	}

#if 0
	ili932x_cache = kmalloc(sizeof(unsigned char) *
		ILI9325_FB_SIZE, GFP_KERNEL);
	if (ili932x_cache == NULL) {
		printk(KERN_ERR ILI932X_NAME ": ERROR: "
			"can't alloc cache buffer (%i bytes)\n",
			ILI9325_FB_SIZE);
		ret = -ENOMEM;
		goto bufferalloced;
	}
#endif

	ili932x_workqueue = create_singlethread_workqueue(ILI932X_NAME);
	if (ili932x_workqueue == NULL) {
		goto cachealloced;
	}

	memset(ili932x_buffer, 0, ILI9325_FB_SIZE);
//	memset(ili932x_cache, 0, ILI9325_FB_SIZE);
	ili932x_change = 0;
	
	if (ili932x_chip_probe(lcd_ili932x) < 0) {
		goto cachealloced;
	}

	ili932x_inited = 1;
	return 0;

cachealloced:
//	kfree(ili932x_cache);

//bufferalloced:
	vfree(ili932x_buffer);
	
page_err:
	iounmap(lcd_ili932x->membase);
	lcd_ili932x->membase = NULL;
	
remap_failed:
//	release_mem_region(ILI9325_BASE_INDEX, ILI9325_IOMEM_SIZE);

//region_failed:
	kfree(lcd_ili932x);

alloc_failed:
	return ret;
}

static void __exit ili932x_exit(void)
{
	ili932x_disable();
	destroy_workqueue(ili932x_workqueue);
//	kfree(ili932x_cache);
	free_page ((unsigned long)ili932x_buffer);
	kfree(ili932x_buffer);
	iounmap(lcd_ili932x->membase);
	//release_mem_region(ILI9325_BASE_INDEX, ILI9325_IOMEM_SIZE);
	kfree(lcd_ili932x);
	lcd_ili932x = NULL;
}

module_init(ili932x_init);
module_exit(ili932x_exit);
