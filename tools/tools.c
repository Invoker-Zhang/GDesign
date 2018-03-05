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

	unsigned long long end = 0;
	end = lseek(fd, 0, SEEK_END);

	unsigned long	blockNum = (end - 1) / BUF_SIZE + 1;
	
	char buf[BUF_SIZE] = {0};
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < blockNum; i++){
		write(fd, buf, BUF_SIZE);
		fprintf(stderr,"%d%% completed\n",100 * i / blockNum);
	}

	close(fd);
}
