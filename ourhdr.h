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
/* constants about folder structure when it pre-allocates */

#define MIN_DSK_SZ	(5 * (1L << 30))

#define VIDEO_SZ	( 1L << 28)

#define VIDEOS_PER_PACK	10L

#define INDEXS_PER_PACK	2


#include "fat32.h"

/* this type describe write position */

typedef struct{
	uint32_t folder;
	uint32_t file;
	uint64_t offset;
}Pos ;

/* allocation information file's content */

typedef struct{
			/* static information about allocation */
	uint32_t folderNum;			/* folder number in this partition */
	uint32_t filesPerFolder;	/* file number predefined per folder */
	uint32_t lastFolderFileNum; /* Properly there isn't a exact division.
								   Last folder contains less files */
			/* dynamic information */
	Pos		writePos;			/* This describes where to write.
								 cyclic written is realised here. */
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

extern void format_fat32(struct filsys_fat32*);
extern void pre_allocation_fat32(struct filsys_fat32*);
extern void clearSectors(int fd, uint64_t start, uint64_t num);
void circle_write(int fd, void* buf, size_t size);



#endif	/* __ourhdr_h */


