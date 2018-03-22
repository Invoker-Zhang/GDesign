#ifndef __fat32_h
#define __fat32_h

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <linux/fs.h>
#include <sys/vfs.h>
#include <time.h>

#include "ourhdr.h"

/* constants about fat32 file system. */

#define	SECS_PER_CLUS		16	
#define	CLUS_SZ	(SEC_SZ * SECS_PER_CLUS)

#define	FAT_NUM				2
#define FAT_ENT_SZ			4	/* size of fat entry */

#define	RSVD_SECS		32 /* reserved sectors */
#define FAT_START		RSVD_SECS

#define ROOT_CLUS_NUM	2 /* root directory cluster number */
#define FS_INFO_SEC		1 /* fs information sector number */
#define FS_INFO_BK_SEC  7 /* fs information backup sector number */
#define BOOT_BK_SEC		6 /* boot backup sector number */

#define ROOT_CLUSTERS		1 /* root directory size in cluseter */

#define VIDEO_CLUS	( VIDEO_SZ / CLUS_SZ) /* video file in cluster */

#define INDEX_CLUS	1 /* in cluster */

#define ALLOC_CLUS	1 /* In cluster */

/* special value of fat entry */
#define FAT_ENT_END		0x0fffffff

/* FDT attributes */
						
#define ATTRI_RDONLY	0X01
#define ATTRI_HIDDEN	0X02
#define ATTRI_SYSTEM	0X04
#define ATTRI_VOLUME	0X08
#define ATTRI_DIR		0X10
#define ATTRI_ARCHIVE	0X20
#define ATTRI_LONG_NAM	(ATTRI_RDONLY | ATTRI_HIDDEN \
						ATTRI_SYSTEM | ATTRI_VOLUME)
/* The upper two bits of attribute byte are reserved */


#pragma pack (1)

/* bios parameter block */

typedef struct BIOS_PARAMETER_BLOCK{
	uint16_t	BPB_BytsPerSec;		/* bytes per sector */
	uint8_t		BPB_SecPerClus;		/* sectors per cluster */
	uint16_t	BPB_RsvdSecCnt; 	/* number of reserved sectors */
	uint8_t		BPB_NumFATs;		/* number of fats */
	uint16_t	BPB_RootEntCnt;		/* 0 in fat32 */
	uint16_t	BPB_ToSec16;		/* 0 in fat32 */
	uint8_t		BPB_Media;			/* 0xf8: harddisk, 0xf0: softdisk */
	uint16_t	BPB_FATSz16;		/* sectors per fat */
	uint16_t	BPB_SecPerTrk;		/* sectors per track */
	uint16_t	BPB_NumHeads;		/* head number */
	uint32_t	BPB_HidSec;		/* sectors ahead bootsector */
	uint32_t	BPB_ToSec32;		/* total_sectors  */
	uint32_t	BPB_FATSz32;		/*  */
	uint16_t	BPB_Flags;			/* flags */
	uint16_t	BPB_FSVer;			/* version number */
	uint32_t	BPB_RootClus;		/* root cluster number */
	uint16_t	BPB_FSIfo;			/* FSINFO sector number; */
	uint16_t	BPB_BkBootSec;		/* backup of bootsector */
	uint32_t	BPB_Reserved[3];	/* reserved */
}BPB;

/* externed boot sector */

typedef struct{
	uint8_t		BS_DrvNum;			/* drive number; */
	uint8_t		BS_Reserved1;		/* reserved1 */
	uint8_t		BS_BootSig;			/* externed boot signature */
	uint32_t		BS_VolId;			/* volumn id */
	unsigned char		BS_VolSysType[11];	/* volumn name */
	unsigned char		BS_FilSysType[8];	/* fs name */
}BS;

/* dos boot record .First sector of this partition */

typedef struct{
	unsigned char		DBR_JmpCode[3]; /* jump code. It will be tested by windows */
	unsigned char		DBR_FacCode[8]; /* This code is free if driver program doesn't check it */
	BPB			DBR_BPB;
	BS			DBR_BS;
	unsigned char		DBR_BootCode[420]; 
	unsigned char		DBR_BootSign[2]; /* 0xaa55 */
}DBR;

/* file system information sector */

typedef struct{
	uint32_t	FSINFO_Sym;				/* 0x41615252, indicate this a fsinfo sector */
	unsigned char	FSINFO_Reserved1[480];	/* not used */
	uint32_t	FSINFO_Used;			/* 0x61417272, indicate this sector has been used */
	uint32_t	FSINFO_LastClus;		/* number of clusters remained */
	uint32_t	FSINFO_SrchEnt;			/* where to find remained clusters */
	unsigned char	FSINFO_Reserved2[12];	/* not used */
	uint32_t	FSINFO_EndSign;			/* 0xaa55 */
}FSINFO;

/* date and time structure used in fat file system */

typedef struct{
	uint16_t	day		: 5;			/*  1-31 */
	uint16_t	month	: 4;			/*  1-12 */
	uint16_t	year	: 7;			/*  0-127, plus 1980 */
}DATE;

typedef struct{
	uint16_t	two_sec : 5;			/* 0-29, indicate 0-58 seconds */
	uint16_t	min		: 6;			/* 0-59 */
	uint16_t	hour	: 5;			/* 0-23 */
}TIME;

/* short file directory table.
 * Because all the file name are fixed and short,
 * no long fdt is needed. */

typedef struct{
	unsigned char	FilName[11];		/* file name, 0x20 if not enough, 8-bit filename and 3-bit extended name */
	unsigned char	Attri;				/*  Attribution: 0:rdwr ; 1:rdonly ; 10:hide ; 100:system ; 1000:volumn ; 
								  10000:subdir ; 100000:archive ; 0x0f:long fdt. */
	unsigned char	Reserved;			/*  Used in windows Nt */
	unsigned char	MilliTime;			/* 0.1 ms */
	TIME	CreateTime;			/* create time */
	DATE	CreateDate;			/* create date */
	DATE	LastVisitDate;		/* last visit date */
	uint16_t	HighClus;			/* high 16-bit of cluster number */
	TIME	ChangeTime;			/* last change time */
	DATE	ChangeDate;			/* last change date */
	uint16_t	LowClus;			/* low 16-bit of cluster number */
	uint32_t	FileLength;			/* file length */
}SHORT_FDT;

/* not used as far */
typedef struct{

}LONG_FDT;
#pragma pack ()

/* fillFDT is a macro to initialise a SHORT_FDT variable.
 * Parameters are so many. Terrible! */

#define fillFDT(fdt, name, attri, millTime,			\
		creatime,									\
		visitime,									\
		clusNum,									\
		changetime,									\
		length)										\
{													\
	uint64_t tempClusNum = clusNum;					\
	strncpy( fdt.FilName, name, 11 );				\
	fdt.Attri = attri;								\
	fdt.MilliTime = millTime;						\
	fdt.CreateTime.two_sec = creatime->tm_sec/2;	\
	fdt.CreateTime.min = creatime->tm_min;			\
	fdt.CreateTime.hour = creatime->tm_hour;		\
	fdt.CreateDate.day  = creatime->tm_mday;		\
	fdt.CreateDate.month = creatime->tm_mon;		\
	fdt.CreateDate.year = creatime->tm_year-80;		\
	fdt.LastVisitDate.day = visitime->tm_mday;		\
	fdt.LastVisitDate.month = visitime->tm_mon;		\
	fdt.LastVisitDate.year = visitime->tm_year;		\
	fdt.HighClus = tempClusNum >> 16;				\
	fdt.ChangeTime.two_sec =changetime->tm_sec/2;	\
	fdt.ChangeTime.min = changetime->tm_min;		\
	fdt.ChangeTime.hour = changetime->tm_hour;		\
	fdt.ChangeDate.day = changetime->tm_mday;		\
	fdt.ChangeDate.month = changetime->tm_mon;		\
	fdt.ChangeDate.year = changetime->tm_year-80;	\
	fdt.LowClus = (tempClusNum & 0x0000ffff);		\
	fdt.FileLength = length;						\
}

/* allocInfo file's beginning cluster number
 * folerNum should be a parameter to compute this one */

#define ALLOC_FILE_CLUS(folderNum)		(3+folderNum)

/* Index file's beginning cluster number.
 * folderNum,which foler and which index file should be specified */

#define INDEX_FILE_CLUS(folderNum,folderIndex,index)\
	(ALLOC_FILE_CLUS(folderNum) + (folderIndex * INDEXS_PER_PACK + index) * INDEX_CLUS + 1)

/* Video file's beginning cluster number.
 * FolderNum, which folder and which video file should be specified */

#define VIDEO_FILE_CLUS(folderNum,folderIndex,index) \
	(INDEX_FILE_CLUS(folderNum,folderNum,0) + (folderIndex * VIDEOS_PER_PACK + index) * VIDEO_CLUS)

#pragma pack (1)




#endif
