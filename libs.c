#include <errno.h>
#include <stdarg.h>
#include "ourhdr.h"

static void err_doit(int, const char *, va_list);

char * pname = NULL;

void err_ret(const char * fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	return;
}

void err_sys(const char * fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_dump(const char* fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}

void err_msg(const char* fmt, ...){
	va_list		ap;
	va_start(ap,fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	return;
}

void err_quit(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(1);
}

static void err_doit(int errnoflag, const char *fmt, va_list ap){
	int errno_save;
	char buf[MAXLINE];

	errno_save = errno;
	vsprintf(buf, fmt, ap);
	if(errnoflag)	sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
	strcat(buf,"\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
	return ;
}

void clearSectors(int fd,
		uint64_t start,
		uint64_t number)
{
	lseek(fd, start * SEC_SZ, SEEK_SET);
	char buf[SEC_SZ] = {0};
	for(int i = 0; i < number; i++){
		write(fd, buf, SEC_SZ);
		printf("clearing sectors... %d%% completed\r",100*i/number);
	}
	printf("                                \r");
	printf("clearing sectors finished\n");
}

/* createFile: create file in a directory.This need two steps.First, allocate fat entries for this file.Second, write file descriptor table.
 * fd: file descriptor of the device
 * fatStart: beginning sector of fat
 * fatBakStart: beginning sector of backup fat
 * nextClus: next cluster available
 * fileSize: size of file to be allocated
 * fileName: file name
 * PDClus: parent directory's cluster number
 * attri: file attribution
 */
void createFile(int fd,
		uint64_t fatStart, 
		uint64_t fatBakStart,
		uint64_t dataStart,
		uint32_t nextClus, 
		uint32_t fileSize,
		const char* fileName, 
		uint32_t PDClus,
		unsigned char attri)
{
	SHORT_FDT fdt = {0};
	time_t t;
	struct tm * curTime = NULL;
	time(&t);
	curTime = gmtime(&t);

	if(attri == 0x20){
		writeFatEntries(fd, fatStart, nextClus, (fileSize - 1) / CLUS_SZ + 1);
		writeFatEntries(fd, fatBakStart, nextClus, (fileSize - 1)/ CLUS_SZ + 1);
		fillFDT(fdt, fileName, attri, 0,
				curTime->tm_sec/2,
				curTime->tm_min,
				curTime->tm_hour,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year,
				nextClus,
				curTime->tm_sec/2,
				curTime->tm_min,
				curTime->tm_hour,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year,
				fileSize);
		addFDT(fd, dataStart, PDClus, &fdt);
	}
	
}

#if 0
void initIndexes(int fd, int folderNum, uint64_t dataStart){
	uint64_t startSec = dataStart + (INDEX_FILE_CLUS(folderNum, 0, 0) - 2) * SECS_PER_CLUS;
	uint64_t num = folderNum * INDEXS_PER_PACK * INDEX_CLUS * SECS_PER_CLUS;
	clear(fd, startSec, num);
	
	for(int i = 0; i < folderNum - 1; i++){
		index1_file file1;
		for(int j =0 ; j < VIDEOS_PER_PACK; j++){
			file1.entries[j].fileLength = VIDEO_CLUS * CLUS_SZ;
		}
		lseek(fd, SEC_SZ*(dataStart+SECS_PER_CLUS*(INDEX_FILE_CLUS(folderNum,i,0) - 2)),SEEK_SET);
		write(fd, &file1,sizeof(file1));
	}
}

#endif
