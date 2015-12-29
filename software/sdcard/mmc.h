/*#######################################################################################
Connect AVR to MMC/SD 
ARM modifications by Ingo Busker 2005

Copyright (C) 2004 Ulrich Radig
#######################################################################################*/

#ifndef _MMC_H_
 #define _MMC_H_

#define BlockSize			512

#define CPOL    4
#define CPHA    3
#define SPIF	7

#if 0
//SPI bits
// SPSR 
#define SPIF	7
#define WCOL	6
#define ROVR	5
#define MODF	4
#define ABRT	3
// bit 2- 0 reserved 

// SPCR 
#define SPIE    7
#define LSBF    6
#define MSTR    5
#define CPOL    4
#define CPHA    3
// bit 2- 0 reserved 

#define MMC_Write			IO0PIN	//Port an der die MMC/SD-Karte angeschlossen ist also des SPI
#define MMC_Read			IO0PIN
#define MMC_Direction_REG	IO0DIR

#define SPI_DI				5		//Port Pin an dem Data Output der MMC/SD-Karte angeschlossen ist 
#define SPI_DO				6		//Port Pin an dem Data Input der MMC/SD-Karte angeschlossen ist
#define SPI_Clock			4		//Port Pin an dem die Clock der MMC/SD-Karte angeschlossen ist (clk)
#define MMC_Chip_Select	   10		//Port Pin an dem Chip Select der MMC/SD-Karte angeschlossen ist
#define SPI_SS				7		//Nicht benutzt

#endif

//Prototypes
inline unsigned char Read_Byte_MMC(void);

inline void Write_Byte_MMC(unsigned char);

extern void MMC_Read_Block(unsigned char *,unsigned char *,unsigned short int);

extern unsigned char mmc_init(void);

extern unsigned char mmc_read_sector (unsigned long,unsigned char *);

extern unsigned char mmc_write_sector (unsigned long,unsigned char *);

extern unsigned char Write_Command_MMC (unsigned char *);

extern unsigned char Read_CSD_MMC (unsigned char *);

extern unsigned char Read_CID_MMC (unsigned char *);

//set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
#define MMC_Disable()
//MMC_Write|= (1<<MMC_Chip_Select);

//set MMC_Chip_Select to low (MMC/SD-Karte Aktiv)
#define MMC_Enable()
//MMC_Write&=~(1<<MMC_Chip_Select);

#define nop()  __asm__ __volatile__ ("nop" ::)

#endif //_MMC_H_


