#include "fat32.h"
#include "ourhdr.h"



int main(int argc, char** argv){
	struct filsys_fat32 fs_fat;
	init_struct_fat32(argv[1], &fs_fat);
	format_fat32(&fs_fat);
	pre_allocation_fat32(&fs_fat);
	/*
	int fd = open("/dev/sdc1",O_RDWR);
	int buf[100];
	long k = 10000000;
	while(k--){
		for(int i = 0; i < 100; i++)
			buf[i] = i;
		circle_write(fd, buf, 100* sizeof(buf[0]));
	}
	*/
	return 0;
}
