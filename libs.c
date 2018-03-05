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
		uint64 start,
		uint64 number)
{
	lseek(fd, start * SECTOR_SIZE, SEEK_SET);
	char buf[SECTOR_SIZE] = {0};
	for(int i = 0; i < number; i++){
		write(fd, buf, SECTOR_SIZE);
	}
}

void createFile(int fd,
		uint64 fatStart, 
		uint64 fatBakStart,
		uint64 dataStart,
		uint32 nextClus, 
		uint32 fileSize,
		const char* fileName, 
		uint32 PDClus,
		uchar attri)
{
	SHORT_FDT fdt;
	if(attri == 0x20){
		writeFatEntries(fd, fatStart, nextClus, (fileSize - 1) / CLUSTER_SIZE + 1);
		writeFatEntries(fd, fatBakStart, nextClus, (fileSize - 1)/ CLUSTER_SIZE + 1);
		fillFDT(fdt, fileName, attri, 0,
				0,0,0,0,0,0,
				0,0,0,
				nextClus,
				0,0,0,0,0,0,
				fileSize);
		addFDT(fd, dataStart, PDClus, &fdt);
	}
	
}
