#include "fat32.h"
#include "ourhdr.h"

int init_struct_fat32(const char* device, struct filsys_fat32* fs){
	int fd;
	if( (fd = open(device, O_RDONLY)) < 0){
		fprintf(stderr, "fail to open %s\n", device);
		return -1;
	}
	fs->device = device;
	fs->tot_sz = lseek(fd, 0, SEEK_END);
	fs->tot_secs = fs->tot_sz / SEC_SZ;
	fs->clus_sz = CLUS_SZ;
	fs->fat_start = FAT_START;
	fs->fat_secs = FAT_ENT_SZ * (fs->tot_secs - RSVD_SECS) / (FAT_ENT_SZ * FAT_NUM + CLUS_SZ);
	fs->data_start = FAT_START + fs->fat_secs * FAT_NUM;
	fs->data_secs = fs->tot_secs - fs->data_start - 1;
	fs->clus_num = fs->data_secs / SECS_PER_CLUS;

	close(fd);
}
