#include "ourhdr.h"

void pre_allocation(char *device){
	int fd;
	uint64 sector_size = SECTOR_SIZE;
	uint64 sector_size = SECTORS_PER_CLUSTER;
	uint64 total_size;
	uint64 total_sectors;
	uint64 reserved_sectors = RESERVED_SECTORS;
	uint64 fat_start;
	uint64 fat_sectors;
	uint64 data_start;
	uint64 data_sectors;
	uint64 cluster_number;

	if( (fd = open(device, O_RDWR, FILE_MODE)) < 0)		err_quit("open error");

	disp(sector_size);
	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )		err_quit("lseek error");

	disp(total_size);
	total_sectors = total_size / sector_size;
	disp(total_sectors);

	fat_start				= reserved_sectors;
	fat_sectors				= 4 * (total_sectors - reserved_sectors) / (4*FAT_NUMBER + sector_size*sectors_per_cluster);
	data_start				= fat_start + fat_sectors*FAT_NUMBER;
	data_sectors			= total_sectors - data_start -1;
	cluster_number			= data_sectors / SECTORS_PER_CLUSTER ;


	uint32 folderNum;
	uint32 lastFolderFileNum;
	uint32 nextClus			= 3;

	if(data_sectors * sector_size < MIN_FREE_SPACE)	err_quit("fail to pre-allocation, there are not enough space");
	//TODO get the folderNum and lastFolderFileNum
	folderNum = 20;

	for(int i = 0; i < folderNum; i++){
		writeFatEntries(fd, fat_start, nextClus, 1);

		SHORT_FDT fdt = {0};
		char name[12] = "folder     ";
		name[6] = '0' + i / 10;
		name[7] = '0' + i % 10;
		fillFDT(fdt, name, 0x10, 
				0, curTime->tm_sec/2, curTime->tm_min, curTime->tm_hour, curTime->tm_mday, curTime->tm_mon, curTime->tm_year, 
				curTime->tm_mday, curTime->tm_mon, curTime->tm_year, 
				nextClus ,
				curTime->tm_sec/2, curTime->tm_min, curTime->tm_hour, curTime->tm_mday, curTime->tm_mon, curTime->tm_year, 0);
		addFDT(fd, 2, &fdt);
		//add fdt , and ,, in subdir
		strncpy(fdt.FilName, ".           ", 11);
		addFDT(fd, nextClus, &fdt);
		strncpy(rootDir.FilName, "..         ", 11);
		addFDT(fd, nextClus, &rootDir);
		nextClus++;
	}

	close(fd);
}



	close(fd);

}

void writeFatEntries(int fd,uint64 fatStart, uint32 entryNum, uint32 clusNum){
	uint32 content = entryNum;

	lseek(fd, fatStart + entryNum * 4, SEEK_SET);
	for(int i  = 0; i < clusNum - 1; i++){
		write(fd, &content, sizeof(content));
		content ++;
	}
	content = 0x0fffffff;
	write(fd, &content, sizeof(content));
}
