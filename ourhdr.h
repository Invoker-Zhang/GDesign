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
#include <ctype.h>
#include <stdint.h>
#include <linux/fs.h>
#include <sys/vfs.h>
#include <time.h>

#define MAXLINE 4096

/* default file access permissions for new files */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


/* default permissions for new directories */
#define DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/* Appendix B */
void	err_dump(const char *, ...);
void	err_msg(const char *, ...);
void	err_quit(const char*, ...);
void	err_ret(const char*, ...);
void	err_sys(const char*, ...);


#define DEBUG	1

#if DEBUG
	#define disp(x) { printf(""#x": %lld\n",x); }
	#define disp16(x) { printf(""#x": 0x%x\n",x); }
#else
	#define disp(x) {}
	#define disp16(x) {}
#endif

/* constants about disk structure */

#define	SEC_SZ				512
#define	SECS_PER_CLUS		16	
#define	CLUS_SZ	(SEC_SZ * SECS_PER_CLUS)

#define	FAT_NUM				2
#define FAT_ENT_SZ			4

#define	RSVD_SECS		32

#define ROOT_CLUSTERS		1

/* constants about folder structure when it pre-allocates */

#define MIN_DSK_SZ	(5 * (1 << 30))

#define VIDEO_SZ	( 1 << 28)
#define VIDEO_CLUS	( VIDEO_SZ / CLUS_SZ)

#define VIDEOS_PER_PACK	10

#define INDEXS_PER_PACK	2

#define INDEX_CLUS	1 /* in cluster */

#define ALLOC_CLUS	1 /* In cluster */



#pragma pack (1)
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

typedef struct{
	uint8_t		BS_DrvNum;			/* drive number; */
	uint8_t		BS_Reserved1;		/* reserved1 */
	uint8_t		BS_BootSig;			/* externed boot signature */
	uint32_t		BS_VolId;			/* volumn id */
	unsigned char		BS_VolSysType[11];	/* volumn name */
	unsigned char		BS_FilSysType[8];	/* fs name */
}BS;

typedef struct{
	unsigned char		DBR_JmpCode[3]; /* jump code. It will be tested by windows */
	unsigned char		DBR_FacCode[8]; /* This code is free if driver program doesn't check it */
	BPB			DBR_BPB;
	BS			DBR_BS;
	unsigned char		DBR_BootCode[420]; 
	unsigned char		DBR_BootSign[2]; /* 0xaa55 */
}DBR;

typedef struct{
	uint32_t	FSINFO_Sym;				/* 0x41615252, indicate this a fsinfo sector */
	unsigned char	FSINFO_Reserved1[480];	/* not used */
	uint32_t	FSINFO_Used;			/* 0x61417272, indicate this sector has been used */
	uint32_t	FSINFO_LastClus;		/* number of clusters remained */
	uint32_t	FSINFO_SrchEnt;			/* where to find remained clusters */
	unsigned char	FSINFO_Reserved2[12];	/* not used */
	uint32_t	FSINFO_EndSign;			/* 0xaa55 */
}FSINFO;

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

typedef struct{

}LONG_FDT;
#pragma pack ()

/* fillFDT is a macro to initialise a SHORT_FDT variable */

#define fillFDT(fdt, name, attri, millTime, \
		creSec, creMin, creHour,creDay,creMonth,creYear, \
		visitDay, visitMonth, visitYear, \
		clusNum, \
		chgSec, chgMin, chgHour, chgDay, chgMonth, chgYear,\
		length) \
{ \
	uint64_t tempClusNum = clusNum;		\
	strncpy( fdt.FilName, name, 11 );	\
	fdt.Attri = attri;		\
	fdt.MilliTime = millTime; \
	fdt.CreateTime.two_sec  = creSec / 2;	\
	fdt.CreateTime.min = creMin;	\
	fdt.CreateTime.hour = creHour;	\
	fdt.CreateDate.day  = creDay;	\
	fdt.CreateDate.month = creMonth; \
	fdt.CreateDate.year = creYear;	\
	fdt.LastVisitDate.day = visitDay; \
	fdt.LastVisitDate.month = visitMonth; \
	fdt.LastVisitDate.year = visitYear ; \
	fdt.HighClus = tempClusNum >> 16;	\
	fdt.ChangeTime.two_sec = chgSec / 2; \
	fdt.ChangeTime.min = chgMin;	\
	fdt.ChangeTime.hour = chgHour; \
	fdt.ChangeDate.day = chgDay;	\
	fdt.ChangeDate.year = chgYear;	\
	fdt.LowClus = (tempClusNum & 0x0000ffff);	\
	fdt.FileLength = length;	\
}

/* allocInfo file's beginning cluster number
 * folerNum should be a parameter to compute this one */
#define ALLOC_FILE_CLUS(folderNum)		(3+folderNum)

/* Index file's beginning cluster number.
 * folderNum,which foler and which index file should be specified */
#define INDEX_FILE_CLUS(folderNum,folderIndex,index)\
	(ALLOC_FILE_CLUS(folderNum) + (folderIndex * INDEXS_PER_PACK + index) * INDEX_CLUS + 1)

/* Video file's beginning cluster number.
 * folderNum, which folder and which video file should be specified */
#define VIDEO_FILE_CLUS(folderNum,folderIndex,index) \
	(INDEX_FILE_CLUS(folderNum,folderNum,0) + (folderIndex * VIDEOS_PER_PACK + index) * VIDEO_CLUS)

#pragma pack (1)
/* this type describe write position */
typedef struct{
	uint32_t folder;
	uint32_t file;
	uint64_t offset;
}Pos ;
typedef struct{
	uint32_t folderNum;
	uint32_t filesPerFolder;
	uint32_t lastFolderFileNum;
	Pos		writePos;
}allocfile;

#define pos2off(pos) ((pos.folder * VIDEOS_PER_PACK + pos.file) * VIDEO_CLUS * CLUS_SZ + pos.offset)
#define clus2off(data_start,clus) (SEC_SZ * (data_start + (clus - 2) * SECS_PER_CLUS) )
/* index file contents */
typedef struct{
	uint64_t fileSize; /* video file size */
	uint32_t fileNum; /* video file number */
	uint32_t file;		/* current file to be written */
	uint32_t offset; /* current offset in the file */
	uint32_t last32bit; /* last 32 bit of video block */
	uint32_t crc32; /* crc32 of video block */
}index_file;
#pragma pack ()

extern void format(char* device);
extern void pre_allocation(char *device);
extern void clearSectors(int fd, uint64_t start, uint64_t num);
extern void createFile(int fd,
		uint64_t fatStart, 
		uint64_t fatBakStart,
		uint64_t dataStart,
		uint32_t nextClus, 
		uint32_t fileSize,
		const char* name, 
		uint32_t PDClus,
		unsigned char attri);
void writeFatEntries(int fd,uint64_t fatStart, uint32_t entryNum, uint32_t clusNum);
void addFDT(int fd,uint64_t data_start,  uint64_t dirClus, SHORT_FDT* fdt);

void circle_write(int fd, void* buf, size_t size);



#endif	/* __ourhdr_h */


