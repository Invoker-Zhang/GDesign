#ifndef __ourhdr_h
#define __ourhdr_h

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/vfs.h>

#define MAXLINE 4096

//default file access permissions for new files
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


//default permissions for new directories
#define DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/* Appendix B */
void	err_dump(const char *, ...);
void	err_msg(const char *, ...);
void	err_quit(const char*, ...);
void	err_ret(const char*, ...);
void	err_sys(const char*, ...);
#define disp(x) { printf(""#x": %lld\n",x); }

typedef unsigned char		uchar;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

#define MIN_FREE_SPACE		5 * 1024 * 1024 * 1024
#define VIDEO_FILE_SIZE		256 * 1024 * 1024
#define VIDEO_FILE_NUM_PER_PACK		10

extern unsigned long long	totalSize;			//disk size in byte
extern unsigned int			sectorSize;			//sector size
extern unsigned int			fatSize;			//fat size int sector
extern unsigned long		reservedOff;		//
extern unsigned long		fatOff;				//fat offset
extern unsigned long		dataOff;			//data offset
extern unsigned long		dataSize;			//data size in sector
extern unsigned long		cluster_number;


extern void format(char* device);

#pragma pack (1)
typedef struct BIOS_PARAMETER_BLOCK{
	uint16		BPB_BytePerSec;		//Bytes per sector
	uint8		BPB_SecPerClus;		//sectors per cluster
	uint16		BPB_RsvdSecCnt; 	//number of reserved sectors
	uint8		BPB_NumFATs;		//number of fats
	uint16		BPB_RootEntCnt;		// 0 in fat32
	uint16		BPB_ToSec16;		// 0 in fat32
	uint8		BPB_Media;	//0xf8: harddisk, 0xf0: softdisk.
	uint16		BPB_FATSz16;		//sectors per fat
	uint16		BPB_SecPerTrk;		//sectors per track
	uint16		BPB_NumHeads;				//head number
	uint32		BPB_HidSec;		//sectors ahead bootsector
	uint32		BPB_ToSec32;		//total_sectors 
	uint32		BPB_FATSz32;		//
	uint16		BPB_Flags;			//flags
	uint16		BPB_FSVer;			//version number
	uint32		BPB_RootClus;		//root cluster number
	uint16		BPB_FSIfo;			//FSINFO sector number;
	uint16		BPB_BkBootSec;		//backup of bootsector
	uint32		BPB_Reserved[3];	//reserved
}BPB;

typedef struct{
	uint8		BS_DrvNum;			//drive number;
	uint8		BS_Reserved1;		//reserved1
	uint8		BS_BootSig;			//externed boot signature
	uint32		BS_VolId;			//volumn id
	uchar		BS_VolSysType[11];	//volumn name
	uchar		BS_FilSysType[8];	//fs name
}BS;

typedef struct{
	uchar		DBR_JmpCode[3];
	uchar		DBR_FacCode[8];
	BPB			DBR_BPB;
	BS			DBR_BS;
	uchar		DBR_BootCode[420];
	uchar		DBR_BootSign[2];
}DBR;

typedef struct{
	uint32	FSINFO_Sym;				//0x41615252, indicate this a fsinfo sector
	uchar	FSINFO_Reserved1[480];	//not used
	uint32	FSINFO_Used;			//0x61417272, indicate this sector has been used
	uint32	FSINFO_LastClus;		//number of clusters remained
	uint32	FSINFO_SrchEnt;			//where to find remained clusters
	uchar	FSINFO_Reserved2[12];	//not used
	uint32	FSINFO_EndSign;			//0xaa5500
}FSINFO;

typedef struct{
	uint16	day		: 5;			// 1-31
	uint16	month	: 4;			// 1-12
	uint16	year	: 7;			// 0-127, plus 1980
}DATE;

typedef struct{
	uint16	two_sec : 5;			//0-29, indicate 0-58 seconds
	uint16	min		: 6;			//0-59
	uint16	hour	: 5;			//0-23
}TIME;

typedef struct{
	uchar	FilName[8];			//file name, 0x20 if not enough
	uchar	ExtName[3];			//extended name, filled with 0x20 if not enough
	uchar	Attri;				//Attribution: 0:rdwr ; 1:rdonly ; 10:hide ; 100:system ; 1000:volumn ; 10000:subdir ; 100000:archive ; 0x0f:long fdt.
	uchar	Reserved;
	uchar	MilliTime;			//10 ms
	TIME	CreateTime;			//create time
	DATE	CreateDate;			//create date
	DATE	LastVisitDate;		//last visit date
	uint16	HighClus;			//high 16-bit of cluster number
	TIME	ChangeTime;			//last change time
	DATE	ChangeDate;			//last change date
	uint16	LowClus;			//low 16-bit of cluster number
	uint32	FileLength;			//file length
}SHORT_FDT;
#pragma pack ()


#endif	/* __ourhdr_h */

