#include <stdlib.h>
#include <string.h>
#include "mmc.h"
#include "fat.h"
#include "uart.h"

int main(void) {
	unsigned char buffer[513*8];
	unsigned short int tmp = 0;
	unsigned short int cluster;
	unsigned char Dir_Attrib = 0;
	unsigned long Size = 0;
	char a;

	UART1Initialize(9600);
	UART1Printf("\n\r*** START ***\n\r");

	while (mmc_init() != 0)
	{
		UART1Printf("** No Card Present. **\n\r");
	}

	UART1Printf("Card Present\n\r");

	Cluster_Data_Store();

	UART1Printf("\nDirectory\n\n\r");

	for (a = 0; a < 240; a++) {
		cluster = Read_Dir_Ent(0, a, &Size, &Dir_Attrib, buffer);
		if (cluster == 0xffff) {
			break;
		}
		tmp = (Size & 0x0000FFFF);
		UART1Printf("Cluster = %4x DirA = %2x Size= %8d FileName = ", cluster,
				Dir_Attrib, tmp);
		UART1Printf(buffer);
		UART1Printf("\n\r");
	}
	UART1Printf("\nDirectory Ende\n\n\r");

	cluster = 0x0;

	/*
	 //�berschreibt Inhalt von mmc.txt mit "1" <------------------------------------------------
	 if (fat_search_file("MMC.TXT",&cluster,&File_Size,&Dir_Attrib,buffer) == 1)
	 {
	 UART1Printf("Loading \n\r");
	 }
	 else
	 {
	 UART1Printf("File not found!\n\r");
	 while(1)
	 {
	 //ENDE!! File nicht gefunden!!
	 }
	 }

	 UART1Printf("\n\rStart write!!\n\r");
	 while(File_Size>0)
	 {
	 for (a=0;a<512;a++)
	 {
	 buffer[a]= '1';
	 }
	 fat_write_file (cluster,&buffer[0],block_counter);
	 block_counter++;
	 for (a=0;a<512;a++)
	 {
	 File_Size = File_Size - 1;
	 if(File_Size<=0)
	 {
	 break;
	 }
	 }
	 }

	 UART1Printf("\n\rStop write!!\n\r");

	 //Laden eines Files und ausgabe auf der Seriellen Schnitstelle <-------------------
	 block_counter = 0x0;
	 cluster= 0x0;

	 if (fat_search_file("MMC.TXT",&cluster,&File_Size,&Dir_Attrib,buffer) == 1)
	 {
	 UART1Printf("Loading \n\r");
	 }
	 else
	 {
	 UART1Printf("File not found!\n\r");
	 while(1)
	 {
	 //ENDE!! File nicht gefunden!!
	 }
	 }

	 while(File_Size>0)
	 {
	 fat_read_file (cluster,buffer,block_counter);
	 block_counter++;
	 for (a=0;a<512;a=a+1)
	 {
	 UART1Printf("%c",buffer[a]);

	 File_Size = File_Size - 1;
	 if(File_Size<=0)
	 {
	 break;
	 }
	 }
	 }
	 */

	while (1) {
	}
	return (0);
}

