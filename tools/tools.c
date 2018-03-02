#include "ourhdr.h"

void displaySector(char* device, unsigned offset){
	int fd;
	unsigned char x = 0;

	if( (fd = open(device, O_RDONLY)) < 0)	err_sys("open error");
	if( lseek(fd, offset * 512, SEEK_SET) < 0)	err_sys("lseek error");

	for(int i = 0; i < 32; i++){

		for(int j = 0 ; j < 16; j++){
			if( read(fd, &x, sizeof(x)) < 0)	err_sys("read error");
			printf("%2x ",x);
		}

		printf("\n");
	}

	close(fd);
}

#define BUF_SIZE	4096
void clearAll(char *device){
	int fd;
	int writeNum;

	if( (fd = open(device, O_WRONLY)) < 0)	err_sys("open error");
	
	char buf[BUF_SIZE] = {0};

	do{
		writeNum = write( fd, buf, BUF_SIZE);
	}while(writeNum == BUF_SIZE);

	close(fd);
}
