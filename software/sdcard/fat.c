#include "lpc22xx.h"
#include <stdlib.h>
#include <string.h>

#include "mmc.h"
#include "fat.h"
#include "uart.h"

unsigned char ClusterSize;
unsigned short int FAT_Offset;
unsigned short int ClusterOffset;
unsigned short int Volume_Boot_Record_Addr;

//############################################################################
//Auslesen der Adresse des Volume Boot Record von MBR
unsigned short int FAT_Addr (unsigned char *Buffer)
//############################################################################
{
	unsigned short int Volume_Boot_Record_Addr;
    
	//auslesen des Master Boot Record von der MMC/SD Karte (addr = 0)
	mmc_read_sector (MasterBootRecord,Buffer); //Read Master Boot Record
    Volume_Boot_Record_Addr = Buffer[VBR_Addr] + (Buffer[VBR_Addr+1] << 8);
	//Berechnet Volume Boot Record 
	mmc_read_sector (Volume_Boot_Record_Addr,Buffer); //Read Master Boot Record
    return (Volume_Boot_Record_Addr);
}
//############################################################################
//Auslesen der Adresse des First Root Directory von Volume Boot Record
unsigned short int Root_Dir_Addr (unsigned char *Buffer) 
//############################################################################
{
	struct BootSec *bootp; //Zeiger auf Bootsektor Struktur
	unsigned short int FirstRootDirSecNum;
	//auslesen des Volume Boot Record von der MMC/SD Karte 
	mmc_read_sector (Volume_Boot_Record_Addr,Buffer);
	bootp=(struct BootSec *)Buffer;

	//berechnet den ersten Sector des Root Directory
	FirstRootDirSecNum = ( bootp->BPB_RsvdSecCnt +
	                       (bootp->BPB_NumFATs * bootp->BPB_FATSz16));

	FirstRootDirSecNum+= Volume_Boot_Record_Addr;
	
	return(FirstRootDirSecNum);
}

//############################################################################
//	Ausgabe des angegebenen Directory Eshort intrag in Entry_Count
//	ist kein Eshort intrag vorhanden, ist der Eshort intrag im 
//	R�ckgabe Cluster 0xFFFF. Es wird immer nur ein Eshort intrag ausgegeben
//	um Speicherplatz zu sparen um es auch f�r kleine Atmels zu benutzen
unsigned short int Read_Dir_Ent (unsigned short int dir_cluster, //Angabe Dir Cluster
							unsigned char Entry_Count,	//Angabe welcher Direshort intrag
							unsigned long *Size, 		//R�ckgabe der File Gr��e
							unsigned char *Dir_Attrib,//R�ckgabe des Dir Attributs
							unsigned char *Buffer) 	//Working Buffer
//############################################################################
{
	unsigned char *pointer;
	unsigned short int TMP_Entry_Count = 0;
	unsigned long Block = 0;
	struct DirEntry *dir; //Zeiger auf einen Verzeichniseshort intrag
	unsigned char b;
	unsigned short int blk;
	unsigned short int a;

	pointer = Buffer;

	if (dir_cluster == 0)
		{
		Block = Root_Dir_Addr(Buffer);
		}
	else
		{
		//Berechnung des Blocks aus BlockCount und Cluster aus FATTabelle
		//Berechnung welcher Cluster zu laden ist
		//Auslesen der FAT - Tabelle
		Load_FAT (dir_cluster,&Block,Buffer);			 
		Block = ((Block-2) * ClusterSize) + ClusterOffset;
		}

	//auslesen des gesamten Root Directory
	for (blk = Block;;blk++)
	{
		mmc_read_sector (blk,Buffer);	//Lesen eines Blocks des Root Directory
		for (a=0;a<BlockSize; a = a + 32)
		{
		 dir=(struct DirEntry *)&Buffer[a]; //Zeiger auf aktuellen Verzeichniseshort intrag holen
		 
			if (dir->DIR_Name[0] == 0) //Kein weiterer Eshort intrag wenn erstes Zeichen des Namens 0 ist
			{
			return (0xFFFF);
			}
			
			//Pr�fen ob es ein 8.3 Eshort intrag ist
			//Das ist der Fall wenn es sich nicht um einen Eshort intrag f�r lange Dateinamen
			//oder um einen als gel�scht markierten Eshort intrag handelt.
   			if ((dir->DIR_Attr != ATTR_LONG_NAME) &&
				(dir->DIR_Name[0] != DIR_ENTRY_IS_FREE)) 
			{
				//Ist es der gew�nschte Verzeichniseshort intrag
				if (TMP_Entry_Count == Entry_Count) 
				{
					//Speichern des Verzeichnis Eshort intrages in den R�ckgabe Buffer
					for(b=0;b<11;b++)
					{
					if (dir->DIR_Name[b] != SPACE)
						{
						if (b == 8)
							{
							*pointer++= '.';
							}
						*pointer++=dir->DIR_Name[b];
						}
					}						
					*pointer++='\0';
					*Dir_Attrib = dir->DIR_Attr;

					//Speichern der Filegr��e
					*Size=dir->DIR_FileSize;
					
					//Speichern des Clusters des Verzeichniseshort intrages
					dir_cluster = dir->DIR_FstClusLO;

					//Eshort intrag gefunden R�cksprung mit Cluster File Start
					return(dir_cluster);
				}
			TMP_Entry_Count++;
			}
		}
	}
	return (0xFFFF); //Kein Eintrag mehr gefunden R�cksprung mit 0xFFFF
}

//############################################################################
//	Auslesen der Cluster f�r ein File aus der FAT
//	in den Buffer(512Byte). Bei einer 128MB MMC/SD 
//	Karte ist die Cluster gr��e normalerweise 16KB gro�
//	das bedeutet das File kann max. 4MByte gro� sein.
//	Bei gr��eren Files mu� der Buffer gr��er definiert
//	werden! (Ready)
//	Cluster = Start Clusterangabe aus dem Directory	
void Load_FAT (	unsigned short int Cluster, 		//Angabe Startcluster
				unsigned long *Block,
				unsigned char *TMP_Buffer) 	//Workingbuffer
//############################################################################
{
	//Zum �berpr�fen ob der FAT Block schon geladen wurde
	unsigned short int FAT_Block_Store = 0;	

	//Byte Adresse innerhalb des Fat Blocks
	unsigned short int FAT_Byte_Addresse;	

	//FAT Block Adresse
	unsigned short int FAT_Block_Addresse;
	
	unsigned short int a;

	//Berechnung f�r den ersten FAT Block (FAT Start Addresse)
	for (a = 0;;a++)
	{	
		if (a == *Block)
			{
			*Block = (0x0000FFFF & Cluster);
			return;
			}
		
		if (Cluster == 0xFFFF)
			{
			break; //Ist das Ende des Files erreicht Schleife beenden
			}
		//Berechnung des Bytes innerhalb des FAT Block�s
		FAT_Byte_Addresse = (Cluster*2) % BlockSize;
			
		//Berechnung des Blocks der gelesen werden mu�
		FAT_Block_Addresse = ((Cluster*2) / BlockSize) + 
								Volume_Boot_Record_Addr + FAT_Offset;	
		//Lesen des FAT Blocks
		//�berpr�fung ob dieser Block schon gelesen wurde
		if (FAT_Block_Addresse != FAT_Block_Store)
			{
			FAT_Block_Store = FAT_Block_Addresse;
			//Lesen des FAT Blocks
			mmc_read_sector (FAT_Block_Addresse,TMP_Buffer);	
			}

		//Lesen der n�chsten Clusternummer
		Cluster = (TMP_Buffer[FAT_Byte_Addresse + 1] << 8) + 
					TMP_Buffer[FAT_Byte_Addresse];		
	}
	return;
}

//############################################################################
//Auslesen Cluster Size der MMC/SD Karte und Speichern der gr��e ins EEprom
//Auslesen Cluster Offset der MMC/SD Karte und Speichern der gr��e ins EEprom
void Cluster_Data_Store (void)
//############################################################################
{
	struct BootSec *bootp; //Zeiger auf Bootsektor Struktur

	unsigned char Buffer[BlockSize];

	Volume_Boot_Record_Addr = FAT_Addr (Buffer);	
   
	mmc_read_sector (Volume_Boot_Record_Addr,Buffer);

    bootp=(struct BootSec *)Buffer;

	ClusterSize = bootp->BPB_SecPerClus;

	FAT_Offset = bootp->BPB_RsvdSecCnt;

	ClusterOffset = ((bootp->BPB_BytesPerSec * 32)/BlockSize);	
	ClusterOffset += Root_Dir_Addr(Buffer);
}

//############################################################################
//Lesen eines 512Bytes Blocks von einem File
void Read_File (unsigned short int Cluster,//Angabe des Startclusters vom File
				 unsigned char *Buffer,	  //Workingbuffer
				 unsigned long BlockCount)	  //Angabe welcher Bock vom File geladen 
										      //werden soll a 512 Bytes
//############################################################################
{
	//Berechnung des Blocks aus BlockCount und Cluster aus FATTabelle
	//Berechnung welcher Cluster zu laden ist
	
	unsigned long Block = (BlockCount/ClusterSize);
	
	//Auslesen der FAT - Tabelle
	Load_FAT (Cluster,&Block,Buffer);			 
	Block = ((Block-2) * ClusterSize) + ClusterOffset;
	//Berechnung des Blocks innerhalb des Cluster
	Block += (BlockCount % ClusterSize);
	//Read Data Block from Device
	mmc_read_sector (Block,Buffer);	
	return;
}

//####################################################################################
//Sucht ein File im Directory
unsigned char Search_File (unsigned char *File_Name,	//Name des zu suchenden Files
							unsigned short int *Cluster, 	//Angabe Dir Cluster welches
														//durchsucht werden soll
														//und R�ckgabe des clusters
														//vom File welches gefunden
														//wurde
							unsigned long *Size, 		//R�ckgabe der File Gr��e
							unsigned char *Dir_Attrib,//R�ckgabe des Dir Attributs
							unsigned char *Buffer) 	//Working Buffer
//####################################################################################
{
	unsigned char a ;
	unsigned short int Dir_Cluster_Store = *Cluster;
	for (a = 0;a < 100;a++)
	{
		*Cluster = Read_Dir_Ent(Dir_Cluster_Store,a,Size,Dir_Attrib,Buffer);
		if (*Cluster == 0xffff)
			{
			return(0); //File not Found
			}		
		
		if(strcmp(Buffer,File_Name) == 0)
			{
			return(1); //File Found
			}
	}
	return(2); //Error
}
