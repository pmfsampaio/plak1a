/* include/video/s1d13xxxfb.h
 *
 * (c) 2004 Simtec Electronics
 * (c) 2005 Thibaut VARENE <varenet@parisc-linux.org>
 *
 * Header file for Epson S1D13XXX driver code
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#ifndef	S1D13XXXFB_H
#define	S1D13XXXFB_H

#define ET0320_PALETTE_SIZE		256
//#define S1D_CHIP_REV			7	/* expected chip revision number for s1d13806 */
//#define S1D_FBID			"S1D13806"
#define ET0320_DEVICENAME			"et0320fb"

#define ILI9320_CHIP_ID				0x9320

/* ILI9325 control registers */

#define ILI9325_DRV_CODE			0x00  /* Driver code read, RO.*/

#define LINE_MEM_PITCH     0x100
/* Note: all above defines should go in separate header files
   when implementing other et0320 chip support. */

struct et0320fb_regval {
	u16	addr;
	u8	value;
};


struct et0320fb_par {
	void __iomem	*regs;

	unsigned int	pseudo_palette[16];
#ifdef CONFIG_PM
	void		*regs_save;	/* pm saves all registers here */
	void		*disp_save;	/* pm saves entire screen here */
#endif
};

struct et0320fb_pdata {
	const struct et0320fb_regval	*initregs;
	const unsigned int		initregssize;
	void				(*platform_init_video)(void);
#ifdef CONFIG_PM
	int				(*platform_suspend_video)(void);
	int				(*platform_resume_video)(void);
#endif
};

#endif

