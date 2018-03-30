#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#define BLK_SZ	4096ul

static void displaySector(const unsigned char *buf){
	int i;
	for(i = 0; i < 32; i++){
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++)
				printf("%02x ", buf[i*16+j*4 + k]);
			printf("  ");
		}
		for(int j = 0; j < 16;j++)
			printf("%c",buf[i*16+j]);
		printf("\n");
	}
}

int main(int argc, char**argv){
	if (argc != 3) {
		fprintf(stderr, "usage: displayBlock device blockNumber\n");
		exit(1);
	}
	int fd;
	if ( (fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "fail to open %s\n", argv[1]);
		exit(1);
	}
	uint32_t block_offset = atoi(argv[2]);
	if (lseek(fd, BLK_SZ * block_offset, SEEK_SET) < 0){
		fprintf(stderr, "fail to lseek\n");
		exit(1);
	}
	char buf[BLK_SZ / 512][512];
	read(fd, buf, BLK_SZ);

	char c = 0;
	int i = 0;
	do{
		displaySector(buf[i]);
		i = (i+1) % (BLK_SZ / 512);
		printf("%d\n",i);
	}while((c = getchar()) != 'q');
	return 0;
}
