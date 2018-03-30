#include "ourhdr.h"
#include "fat32.h"

void circle_write(int fd, void *buf, size_t size){
	uint64_t sector_size = SEC_SZ;
	uint64_t sectors_per_cluster = SECS_PER_CLUS;
	uint64_t total_size;
	uint64_t total_sectors;
	uint64_t reserved_sectors = RSVD_SECS;
	uint64_t fat_start;
	uint64_t fat_sectors;
	uint64_t fat_backup_start;
	uint64_t data_start;
	uint64_t data_sectors;
	uint64_t cluster_number;

	/* compute basic parameters about the disk */
	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )	
		err_quit("lseek error");

	total_sectors = total_size / SEC_SZ;

	fat_start				= RSVD_SECS;
	fat_sectors	= (total_sectors - RSVD_SECS)  
		/ (FAT_NUM + SEC_SZ * SECS_PER_CLUS / FAT_ENT_SZ);
	fat_backup_start = fat_start + fat_sectors;
	data_start	= fat_start + fat_sectors*FAT_NUM;
	data_sectors = total_sectors - data_start ;
	cluster_number	= data_sectors / SECS_PER_CLUS ;

	uint32_t dirSize = (2 + INDEXS_PER_PACK + VIDEOS_PER_PACK - 1) / 
		(CLUS_SZ/ sizeof(SHORT_FDT)) + 1;

	uint32_t folderNum = 0;
	uint32_t lastFolderFileNum = 0;
	/* compute how many folders and how many video files in last folder */

	if( (cluster_number - ROOT_CLUSTERS - ALLOC_CLUS) % 
			(INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK + 
			 dirSize) == 0)			/* clusters can be divided properly, each folder has equal video files */
	{
		folderNum = 
			(cluster_number - ROOT_CLUSTERS - ALLOC_CLUS) / 
			(dirSize + 
			 INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK);
		lastFolderFileNum = VIDEOS_PER_PACK;
	}
	else
	{
		folderNum = 
			(cluster_number - ROOT_CLUSTERS - ALLOC_CLUS) / 
			(dirSize + 
			INDEXS_PER_PACK * INDEX_CLUS + 
			VIDEO_CLUS * VIDEOS_PER_PACK) + 1;
		lastFolderFileNum = 
			(cluster_number - ROOT_CLUSTERS - ALLOC_CLUS - 
			folderNum * (dirSize + INDEX_CLUS * INDEXS_PER_PACK)- 
			(folderNum - 1) * VIDEO_CLUS * VIDEOS_PER_PACK ) /
			VIDEO_CLUS ;		
	}

#if 1
	allocfile header;
	lseek(fd, clus2off(data_start,ALLOC_FILE_CLUS(folderNum)), SEEK_SET);
	read(fd, &header, sizeof(header) );

	disp(header.writePos.folder);
	disp(header.writePos.file);
	disp(header.writePos.offset);

	uint64_t sum = ((header.folderNum - 1) * VIDEOS_PER_PACK + header.lastFolderFileNum ) * VIDEO_SZ;
	uint64_t cur = (header.writePos.folder * VIDEOS_PER_PACK + header.writePos.file) * VIDEO_SZ + header.writePos.offset;
	disp(cur);
	disp(sum);
	if(cur + size > sum){
		lseek(fd, clus2off(data_start, VIDEO_FILE_CLUS(folderNum, 0, 0)) + cur, SEEK_SET);
		write(fd, buf, sum - cur);
		lseek(fd, clus2off(data_start, VIDEO_FILE_CLUS(folderNum, 0, 0)), SEEK_SET);
		write(fd, buf + sum - cur, cur + size - sum);
	} else {
		lseek(fd, clus2off(data_start, VIDEO_FILE_CLUS(folderNum, 0, 0)) + cur, SEEK_SET);
		write(fd, buf, size);
	}
	cur = (cur + size) % sum;
	header.writePos.offset = cur % VIDEO_SZ;
	cur = cur / VIDEO_SZ;
	header.writePos.file = cur % VIDEOS_PER_PACK;
	cur = cur / VIDEOS_PER_PACK;
	header.writePos.folder = cur;

	lseek(fd, clus2off(data_start,ALLOC_FILE_CLUS(folderNum)), SEEK_SET);
	write(fd, &header, sizeof(header));

#endif

}
