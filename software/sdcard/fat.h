/*#######################################################################################
Connect AVR to MMC/SD 

Copyright (C) 2004 Ulrich Radig
ARM modifications by Ingo Busker 2005
#######################################################################################*/

#ifndef _FAT_H_
 #define _FAT_H_

//Prototypes
extern unsigned short int Root_Dir_Addr (unsigned char *);
extern unsigned short int Read_Dir_Ent (unsigned short int,unsigned char,unsigned long*,unsigned char *,unsigned char *);
extern unsigned short int FAT_Addr (unsigned char *);
extern void Load_FAT (unsigned short int,unsigned long *,unsigned char *);
extern void Read_File (unsigned short int,unsigned char *,unsigned long);
extern void Cluster_Data_Store (void);
extern unsigned char Search_File (unsigned char *,unsigned short int *,unsigned long *,unsigned char *,unsigned char *);

//Block Size in Bytes
#define BlockSize			512

//Master Boot Record
#define MasterBootRecord	0

//Volume Boot Record location in Master Boot Record
#define VBR_Addr 			0x1C6

//define ASCII
#define SPACE 				0x20
#define DIR_ENTRY_IS_FREE   0xE5
#define FIRST_LONG_ENTRY	0x01
#define SECOND_LONG_ENTRY	0x42

//define DIR_Attr
#define ATTR_LONG_NAME		0x0F
#define ATTR_READ_ONLY		0x01
#define ATTR_HIDDEN			0x02
#define ATTR_SYSTEM			0x04
#define ATTR_VOLUME_ID		0x08
#define ATTR_DIRECTORY		0x10
#define ATTR_ARCHIVE		0x20

struct BootSec 
{
	unsigned char  		BS_jmpBoot[3];
	char           			BS_OEMName[8];
	unsigned short int   	BPB_BytesPerSec; //2 bytes
	unsigned char			BPB_SecPerClus;
	unsigned short int	BPB_RsvdSecCnt; //2 bytes
	unsigned char			BPB_NumFATs;
	unsigned short int	BPB_RootEntCnt; //2 bytes
	unsigned short int	BPB_TotSec16; //2 bytes
	unsigned char			BPB_Media;
	unsigned short int	BPB_FATSz16; //2 bytes
	unsigned short int	BPB_SecPerTrk; //2 bytes
	unsigned short int	BPB_NumHeads; //2 bytes
	unsigned long			BPB_HiddSec; //4 bytes
	unsigned long			BPB_TotSec32; //4 bytes
};

//FAT12 and FAT16 Structure Starting at Offset 36
#define BS_DrvNum			36
#define BS_Reserved1		37
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54

//FAT32 Structure Starting at Offset 36
#define BPB_FatSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BPB_Reserved		52

#define FAT32_BS_DrvNum		64
#define FAT32_BS_Reserved1	65
#define FAT32_BS_BootSig	66
#define FAT32_BS_VolID		67
#define FAT32_BS_VolLab		71
#define FAT32_BS_FilSysType	82
//End of Boot Sctor and BPB Structure

struct DirEntry {
	unsigned char			DIR_Name[11];      //8 chars filename
	unsigned char			DIR_Attr;         //file attributes RSHA, Longname, Drive Label, Directory
	unsigned char			DIR_NTRes;        //set to zero
	unsigned char			DIR_CrtTimeTenth; //creation time part in milliseconds
	unsigned short int	DIR_CrtTime;      //creation time
	unsigned short int	DIR_CrtDate;      //creation date
	unsigned short int	DIR_LastAccDate;  //last access date
	unsigned short int	DIR_FstClusHI;  //first cluster high word                 
	unsigned short int	DIR_WrtTime;      //last write time
	unsigned short int	DIR_WrtDate;      //last write date
	unsigned short int	DIR_FstClusLO;  //first cluster low word                 
	unsigned long			DIR_FileSize;     
	};

#endif //_FAT_H_
