#include "ourhdr.h"

extern unsigned long		data_start;
extern unsigned long		data_sectors;

void pre_allocation(char *device){
	uint32 folderNum;
	uint32 lastFolderFileNum;

	uint32 nextClus = 3;

	if(data_sectors * sector_size < MIN_FREE_SPACE)		err_exit("fail to pre-allocation, there are not enough space");

	//get the folderNum
	
	for(int i = 0; i < folderNum; i++){
		
	}


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
