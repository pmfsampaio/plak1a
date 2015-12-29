#include "lpc22xx.h"
#include "mmc.h"
unsigned char mmc_init ()
{
	unsigned short int Timeout = 0;
	unsigned long a;
	unsigned char b;

#if 0
	MMC_Direction_REG &=~(1<<SPI_DI);			//Setzen von Pin MMC_DI auf Input
	MMC_Direction_REG |= (1<<SPI_Clock);		//Setzen von Pin MMC_Clock auf Output
	MMC_Direction_REG |= (1<<SPI_DO);			//Setzen von Pin MMC_DO auf Output
	MMC_Direction_REG |= (1<<MMC_Chip_Select);	//Setzen von Pin MMC_Chip_Select auf Output
	MMC_Direction_REG |= (1<<SPI_SS);	
	MMC_Write |= (1<<MMC_Chip_Select);			//Setzt den Pin MMC_Chip_Select auf High Pegel

	IO0DIR &= ~(1 << SPI_DI);

	// setup SCK pin P04
	PINSEL0 &= ~(3<<8);
	PINSEL0 |= (1<<8);
	  
	// setup MISO pin P05
	PINSEL0 &= ~(3<<10);
	PINSEL0 |= (1<<10);
	// setup MOSI pin P06
	PINSEL0 &= ~(3<<12);
	PINSEL0 |= (1<<12);
	// setup SSEL pin P07
	PINSEL0 &= ~(3<<14);
	PINSEL0 |= 1<<14;
	  
	//Aktiviren des SPI - Bus, Master, Clock = Active LOW
	SPI_SPCCR = 254;//248; // low speed
	SPI_SPCR = (1<<MSTR)|(0<<CPOL); //Enable SPI, SPI in Master Mode
#endif

	// setup SCK pin P04
	PINSEL0 &= ~(3<<8);
	PINSEL0 |= (1<<8);

	// setup MISO pin P05
	PINSEL0 &= ~(3<<10);
	PINSEL0 |= (1<<10);
	// setup MOSI pin P06
	PINSEL0 &= ~(3<<12);
	PINSEL0 |= (1<<12);
	// setup SSEL pin P07
	PINSEL0 &= ~(3<<14);
	PINSEL0 |= 1<<14;

	SPI_SPCCR = 254;//100;
	// Master CPOL = 0, CCPHA = 0. MSB first
	SPI_SPCR = (1 << 5) | (0 << CPOL) | (0 << CPHA);
	
	for(a=0;a<20000;a++){
	nop();
	};
	
	//Initialisiere MMC/SD-Karte in den SPI-Mode
	for (b = 0;b<10;b++) //Sendet min 74+ Clocks an die MMC/SD-Karte
	{
		Write_Byte_MMC(0xff);
	}
	
	unsigned char CMD[] = {0x40,0x00,0x00,0x00,0x00,0x95};
	
	//Sendet Commando CMD0 an MMC/SD-Karte
	while(Write_Command_MMC (CMD) !=1)
	{
		if (Timeout++ > 200)
			{
			return(1); //Abbruch bei Commando1 (Return Code1)
			}
	}
	
	
	//Sendet Commando CMD1 an MMC/SD-Karte
	Timeout = 0;
	CMD[0] = 0x41;//Commando 1
	CMD[5] = 0xFF;
	
	while( Write_Command_MMC (CMD) !=0)
	{
		if (Timeout++ > 200)
			{
			return(2); //Abbruch bei Commando2 (Return Code2)
			}
	}
	
    SPI_SPCCR = 8; // max speed
	
	//set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
	MMC_Disable();
	return(0);
}

//############################################################################
//Sendet ein Commando an die MMC/SD-Karte
unsigned char Write_Command_MMC (unsigned char *CMD)
//############################################################################
{
	unsigned char tmp = 0xff;
	unsigned short int Timeout = 0;
	unsigned char a;

	//set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv) 
	MMC_Disable();

	//sendet 8 Clock Impulse
	Write_Byte_MMC(0xFF);

	//set MMC_Chip_Select to low (MMC/SD-Karte Aktiv)
	MMC_Enable();

	//sendet 6 Byte Commando
	for (a = 0;a<0x06;a++) //sendet 6 Byte Commando zur MMC/SD-Karte
		{
		Write_Byte_MMC(*CMD++);
		}

	//Wartet auf ein g�ltige Antwort von der MMC/SD-Karte
	while (tmp == 0xff)	
		{
		tmp = Read_Byte_MMC();
		if (Timeout++ > 500)
			{
			break; //Abbruch da die MMC/SD-Karte nicht Antwortet
			}
		}
  // printf("#R: %02X #\n\r",tmp);
	return(tmp);
}

inline unsigned char Read_Byte_MMC (void)
{
	unsigned char value = 0;

	SPI_SPDR = 0xff;
	while (!(SPI_SPSR & (1 << SPIF)));
	value = SPI_SPDR;
	return value;
}

inline void Write_Byte_MMC (unsigned char Byte)
{
	SPI_SPDR = Byte;
	while (!(SPI_SPSR & (1 << SPIF)));
}

//############################################################################
//Routine zum schreiben eines Blocks(512Byte) auf die MMC/SD-Karte
unsigned char mmc_write_sector (unsigned long addr,unsigned char *Buffer)
//############################################################################
{
	unsigned short int a;
	unsigned char tmp;
	//Commando 24 zum schreiben eines Blocks auf die MMC/SD - Karte
	unsigned char CMD[] = {0x58,0x00,0x00,0x00,0x00,0xFF}; 
	
	/*Die Adressierung der MMC/SD-Karte wird in Bytes angegeben,
	  addr wird von Blocks zu Bytes umgerechnet danach werden 
	  diese in das Commando eingef�gt*/
	  
	//addr = addr << 9; //addr = addr * 512
	
	CMD[1] = ((addr & 0xFF000000) >>24 );
	CMD[2] = ((addr & 0x00FF0000) >>16 );
	CMD[3] = ((addr & 0x0000FF00) >>8 );

	//Sendet Commando CMD24 an MMC/SD-Karte (Write 1 Block/512 Bytes)
	tmp = Write_Command_MMC (CMD);
	if (tmp != 0)
		{
		return(tmp);
		}
			
	//Wartet einen Moment und sendet einen Clock an die MMC/SD-Karte
	for (a=0;a<100;a++)
		{
		Read_Byte_MMC();
		}
	
	//Sendet Start Byte an MMC/SD-Karte
	Write_Byte_MMC(0xFE);	
	
	//Schreiben des Bolcks (512Bytes) auf MMC/SD-Karte
	for (a=0;a<BlockSize;a++)
		{
		Write_Byte_MMC(*Buffer++);
		}
	
	//CRC-Byte schreiben
	Write_Byte_MMC(0xFF); //Schreibt Dummy CRC
	Write_Byte_MMC(0xFF); //CRC Code wird nicht benutzt
	
	//Wartet auf MMC/SD-Karte Bussy
	while (Read_Byte_MMC() != 0xff){};
	
	//set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
	MMC_Disable();
	
return(0);
}

//############################################################################
//Routine zum lesen des CID Registers von der MMC/SD-Karte (16Bytes)
void MMC_Read_Block(unsigned char *CMD,unsigned char *Buffer,unsigned short int Bytes)
//############################################################################
{	
	unsigned short int a;
	//Sendet Commando CMD an MMC/SD-Karte
	if (Write_Command_MMC (CMD) != 0)
			{
			 return;
			}

	//Wartet auf Start Byte von der MMC/SD-Karte (FEh/Start Byte)
	
	while (Read_Byte_MMC() != 0xfe){};

	//Lesen des Bolcks (normal 512Bytes) von MMC/SD-Karte
	for (a=0;a<Bytes;a++)
		{
		*Buffer++ = Read_Byte_MMC();
		}
	//CRC-Byte auslesen
	Read_Byte_MMC();//CRC - Byte wird nicht ausgewertet
	Read_Byte_MMC();//CRC - Byte wird nicht ausgewertet
	
	//set MMC_Chip_Select to high (MMC/SD-Karte Inaktiv)
	MMC_Disable();
	
	return;
}

//############################################################################
//Routine zum lesen eines Blocks(512Byte) von der MMC/SD-Karte
unsigned char mmc_read_sector (unsigned long addr,unsigned char *Buffer)
//############################################################################
{	
	//Commando 16 zum lesen eines Blocks von der MMC/SD - Karte
	unsigned char CMD[] = {0x51,0x00,0x00,0x00,0x00,0xFF}; 
	
	/*Die Adressierung der MMC/SD-Karte wird in Bytes angegeben,
	  addr wird von Blocks zu Bytes umgerechnet danach werden 
	  diese in das Commando eingef�gt*/
	  
	addr = addr << 9; //addr = addr * 512

	CMD[1] = ((addr & 0xFF000000) >>24 );
	CMD[2] = ((addr & 0x00FF0000) >>16 );
	CMD[3] = ((addr & 0x0000FF00) >>8 );

    MMC_Read_Block(CMD,Buffer,BlockSize);

	return(0);
}

//############################################################################
//Routine zum lesen des CID Registers von der MMC/SD-Karte (16Bytes)
unsigned char Read_CID_MMC (unsigned char *Buffer)
//############################################################################
{
	//Commando zum lesen des CID Registers
	unsigned char CMD[] = {0x4A,0x00,0x00,0x00,0x00,0xFF}; 
	
	MMC_Read_Block(CMD,Buffer,16);

	return(0);
}

//############################################################################
//Routine zum lesen des CSD Registers von der MMC/SD-Karte (16Bytes)
unsigned char Read_CSD_MMC (unsigned char *Buffer)
//############################################################################
{	
	//Commando zum lesen des CSD Registers
	unsigned char CMD[] = {0x49,0x00,0x00,0x00,0x00,0xFF};
	
	MMC_Read_Block(CMD,Buffer,16);

	return(0);
}
