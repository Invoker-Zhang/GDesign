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
#include <time.h>

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
#define disp16(x) { printf(""#x": 0x%x\n",x); }

typedef unsigned char		uchar;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

//macros about disk structure
#define	SECTOR_SIZE				512
#define	SECTORS_PER_CLUSTER		16	
#define	CLUSTER_SIZE	(SECTOR_SIZE * SECTORS_PER_CLUSTER)
#define	FAT_NUMBER				2
#define FAT_ENTRY_SIZE			4
#define	RESERVED_SECTORS		32
#define ROOT_CLUSTERS		1



//macros about folder structure when it pre-allocates
#define MIN_FREE_SPACE		(5 * (1 << 30))
#define VIDEO_CLUS		((1 << 28) / CLUSTER_SIZE)
#define VIDEOS_PER_PACK		10
#define INDEXS_PER_PACK	2
#define INDEX_CLUS				1 //in cluster
#define ALLOC_CLUS				1 //In cluster



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
	uint16		BPB_NumHeads;		//head number
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
	uchar	FilName[11];			//file name, 0x20 if not enough, 8-bit filename and 3-bit extended name
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

typedef struct{

}LONG_FDT;
#pragma pack ()

#define fillFDT(fdt, name, attri, millTime, \
		creSec, creMin, creHour,creDay,creMonth,creYear, \
		visitDay, visitMonth, visitYear, \
		clusNum, \
		chgSec, chgMin, chgHour, chgDay, chgMonth, chgYear,\
		length) \
{ \
	uint64 tempClusNum = clusNum;		\
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

#define ALLOC_FILE_CLUS(folderNum)		(3+folderNum)
#define INDEX_FILE_CLUS(folderNum,folderIndex,index)\
	(ALLOC_FILE_CLUS(folderNum) + folderIndex*(VIDEO_CLUS*VIDEOS_PER_PACK+INDEX_CLUS*INDEXS_PER_PACK) + index * INDEX_CLUS + 1)
#define VIDEO_FILE_CLUS(folderNum,folderIndex,index) \
	(INDEX_FILE_CLUS(folderNum,folderIndex,INDEXS_PER_PACK) + VIDEO_CLUS*index)

extern void format(char* device);
extern void pre_allocation(char *device);
extern void clearSectors(int fd, uint64 start, uint64 num);
extern void createFile(int fd,
		uint64 fatStart, 
		uint64 fatBakStart,
		uint64 dataStart,
		uint32 nextClus, 
		uint32 fileSize,
		const char* name, 
		uint32 PDClus,
		uchar attri);
void writeFatEntries(int fd,uint64 fatStart, uint32 entryNum, uint32 clusNum);
void addFDT(int fd,uint64 data_start,  uint64 dirClus, SHORT_FDT* fdt);




#endif	/* __ourhdr_h */

