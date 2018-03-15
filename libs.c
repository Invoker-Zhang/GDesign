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
