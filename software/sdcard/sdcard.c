//#include "fat32.h"
#include "mmc.h"
#include "fat.h"
#include "lpc22xx.h"

void SPI_Configure(void) // \arg baudrate to be programmed
{
	int i;

	PINSEL0 &= ~0xFF00;
	PINSEL0 = 0x5500;

	SPI_SPCCR = 254;//100;
	SPI_SPCR = (1 << 5);// | (1 << CPOL) | (0 << CPHA); // Master CPOL = 0, CCPHA = 0. MSB first

	i = SPI_SPSR;

}

unsigned char Read_Byte_MMC(void)
//############################################################################
{
	unsigned char Byte = 0;
	unsigned char value = 0xff;

#if 0
	while ((SPI_SPSR & (1 << 7)) == 0);
	//	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0);
	SPI_PutChar(value);
	//	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) == 0);
	Byte = (unsigned char)SPI_GetChar();
#endif
	SPI_SPDR = 0xff;
	while (!(SPI_SPSR & (1 << SPIF))) {
	};
	Byte = SPI_SPDR;

	return (Byte);
}

//############################################################################
//Routine zum Senden eines Bytes zur MMC-Karte
void Write_Byte_MMC(unsigned char Byte)
//##############################commands##############################################
{
#if 0
	while ((SPI_SPSR & (1 << 7)) == 0);
	//	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0);
	SPI_PutChar(Byte);
#endif
	SPI_SPDR = Byte; //Sendet ein Byte
	while (!(SPI_SPSR & (1 << SPIF))) {
	}; //Wartet bis Byte gesendet wurde
}

volatile unsigned long startBlock;
volatile unsigned long totalBlocks;
volatile unsigned char buffer[512];
volatile unsigned long firstDataSector, rootCluster, totalClusters;
volatile unsigned int bytesPerSector, sectorPerCluster, reservedSectorCount;

int main(void) {
	unsigned char buffer[513];
	unsigned short int tmp = 0;
	unsigned short int cluster;
	unsigned char Dir_Attrib = 0;
	unsigned long Size = 0;
	char a;

	SPI_Configure();

	while (mmc_init() != 0)
		;

	Cluster_Data_Store();

	for (a = 0; a < 240; a++) {
		cluster = Read_Dir_Ent(0, a, &Size, &Dir_Attrib, buffer);
		if (cluster == 0xffff) {
			break;
		}
		tmp = (Size & 0x0000FFFF);
		//		printf("Cluster = %4x DirA = %2x Size= %8d FileName = ",cluster,Dir_Attrib,tmp);
		//		printf(buffer);
		//		printf("\n\r");
	}
	return 0;
}
