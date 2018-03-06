#include "ourhdr.h"


void pre_allocation(char *device){
	int fd;
	uint64 sector_size = SECTOR_SIZE;
	uint64 sectors_per_cluster = SECTORS_PER_CLUSTER;
	uint64 total_size;
	uint64 total_sectors;
	uint64 reserved_sectors = RESERVED_SECTORS;
	uint64 fat_start;
	uint64 fat_sectors;
	uint64 fat_backup_start;
	uint64 data_start;
	uint64 data_sectors;
	uint64 cluster_number;

	if( (fd = open(device, O_RDWR, FILE_MODE)) < 0)	
		err_quit("open error");

	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )	
		err_quit("lseek error");

	total_sectors = total_size / sector_size;

	fat_start				= reserved_sectors;
	fat_sectors	= 4 * (total_sectors - reserved_sectors)  
		/ (4*FAT_NUMBER + sector_size*sectors_per_cluster);
	fat_backup_start = fat_start + fat_sectors;
	data_start	= fat_start + fat_sectors*FAT_NUMBER;
	data_sectors = total_sectors - data_start -1;
	cluster_number	= data_sectors / SECTORS_PER_CLUSTER ;

	if(data_sectors < MIN_FREE_SPACE / SECTOR_SIZE)	
		err_quit("fail to pre-allocation,\
				there are not enough space");

	//This variable indicates number of clusters every folder occupied.
	uint32 dirSize = (3 + VIDEO_FILE_NUM_PER_PACK) / 
		(sector_size * SECTORS_PER_CLUSTER / 
		 sizeof(SHORT_FDT)) + 1;

	disp16(cluster_number);
	disp(fat_backup_start);
	disp(dirSize);

	uint32 folderNum = 0;
	uint32 lastFolderFileNum = 0;
	if( (cluster_number - 1 - ALLOC_FILE_SIZE) % 
			(INDEX_FILE_NUM_PER_FOLDER * INDEX_FILE_SIZE +
			 VIDEO_FILE_SIZE * VIDEO_FILE_NUM_PER_PACK + 
			 dirSize) == 0)
	{
		folderNum = 
			(cluster_number - 1 - ALLOC_FILE_SIZE) / 
			(dirSize + 
			 INDEX_FILE_NUM_PER_FOLDER * INDEX_FILE_SIZE +
			 VIDEO_FILE_SIZE * VIDEO_FILE_NUM_PER_PACK);
		lastFolderFileNum = VIDEO_FILE_NUM_PER_PACK;
	}
	else
	{
		folderNum = 
			(cluster_number - 1 - ALLOC_FILE_SIZE) / 
			(dirSize + 
			INDEX_FILE_NUM_PER_FOLDER * INDEX_FILE_SIZE + 
			VIDEO_FILE_SIZE * VIDEO_FILE_NUM_PER_PACK) + 1;
		lastFolderFileNum = 
			(cluster_number - 1 - ALLOC_FILE_SIZE - 
			folderNum * 
			(dirSize + 
			INDEX_FILE_SIZE * INDEX_FILE_NUM_PER_FOLDER)- 
			(folderNum - 1) * VIDEO_FILE_SIZE * 
			VIDEO_FILE_NUM_PER_PACK ) /
			VIDEO_FILE_SIZE ;		
	}
	disp(folderNum);
	disp(lastFolderFileNum);
	
	clearSectors( fd, 
			data_start,
			(1+folderNum * dirSize) * SECTORS_PER_CLUSTER);

	//write the root directory's first entry(itself)
	SHORT_FDT rootDir = {0};
	fillFDT(rootDir, "Video      ", 0x08, 0,
			0,0,0,0,0,0,
			0,0,0,
			0,
			0,0,0,0,0,0,
			0);
	addFDT(fd, data_start, 2, &rootDir);
	
	uint32 nextClus			= 3;

	if(folderNum > 100)		
		err_quit("need too many folders,You should enlarge\
			VIDEO_FILE_SIZE or VIDEO_FILE_NUM_PER_PACK");

	SHORT_FDT fdt = {0};
	char name[12] = "folder     ";

	//allocate fat entries for folders
	for(int i = 0; i < folderNum; i++){
		writeFatEntries(fd, fat_start, nextClus, dirSize);
		writeFatEntries(fd, fat_backup_start, nextClus, dirSize);

		name[6] = '0' + i / 10;
		name[7] = '0' + i % 10;
		
		fillFDT(fdt, name, 0x10, 0,
				0,0,0,0,0,0,
				0,0,0,
				nextClus,
				0,0,0,0,0,0,
				0);
		addFDT(fd, data_start, 2, &fdt);
		//add fdt . and .. in subdir
		strncpy(fdt.FilName, ".           ", 11);
		addFDT(fd, data_start, nextClus, &fdt);
		strncpy(rootDir.FilName, "..         ", 11);
		rootDir.Attri = 0x10;
		addFDT(fd, data_start, nextClus, &rootDir);

		nextClus += dirSize;
	}

	//allocate fat entries for allocInfo file
	createFile(fd,
			fat_start,
			fat_backup_start,
			data_start,
			nextClus,
			CLUSTER_SIZE,
			"AllocIfo   ",
			2,
			0x20);
	nextClus += ALLOC_FILE_SIZE;

	//create video files and index files in each folder
	//allocate fat entries and fill the items in Parent directory
	for(int i = 0; i < folderNum - 1; i++){
		for(int j = 0; j < INDEX_FILE_NUM_PER_FOLDER;j++){
			strncpy(name, "Index      ", 11);
			name[6] = '0' + j % 10;
			name[5] = '0' + j / 10;
			createFile(fd,
					fat_start,
					fat_backup_start,
					data_start,
					nextClus,
					INDEX_FILE_SIZE *CLUSTER_SIZE,
					name,
					i + 1 + 2,
					0x20);
			nextClus += INDEX_FILE_SIZE;
		}
		strncpy(name, "ce      mp4", 11);
		for(int j = 0; j < VIDEO_FILE_NUM_PER_PACK; j++){
			int t = j;
			for(int k = 0; k < 6; k++){
				name[7-k] = '0' + t % 10;
				t = t/10;
			}
			createFile(fd,
					fat_start,
					fat_backup_start,
					data_start,
					nextClus,
					VIDEO_FILE_SIZE*CLUSTER_SIZE,
					name,
					i+1+2,
					0x20);
			nextClus += VIDEO_FILE_SIZE;
		}
	}
	for(int j = 0; j < INDEX_FILE_NUM_PER_FOLDER;j++){
		strncpy(name, "Index      ", 11);
		name[6] = '0' + j % 10;
		name[5] = '0' + j / 10;
		createFile(fd,
			fat_start,
			fat_backup_start,
			data_start,
			nextClus,
			INDEX_FILE_SIZE * CLUSTER_SIZE,
			name,
			folderNum + 2,
			0x20);
		nextClus += INDEX_FILE_SIZE;
	}
	strncpy(name, "ce      mp4",11);
	for(int j = 0; j < lastFolderFileNum; j++){
		int t = j;
		for(int k = 0; k < 6; k++){
			name[7-k] = '0' + t % 10;
			t = t/10;
		}
		createFile(fd,
			fat_start,
			fat_backup_start,
			data_start,
			nextClus,
			VIDEO_FILE_SIZE*CLUSTER_SIZE,
			name,
			folderNum + 2,
			0x20);
		nextClus += VIDEO_FILE_SIZE;
	}

	//update the fsinfo sector
	FSINFO fsInfoSec;
	lseek(fd, 1 * SECTOR_SIZE, SEEK_SET);
	read(fd, &fsInfoSec, sizeof(fsInfoSec));
	fsInfoSec.FSINFO_LastClus = cluster_number - nextClus;
	fsInfoSec.FSINFO_SrchEnt = nextClus;

	disp16(fsInfoSec.FSINFO_LastClus);
	disp16(fsInfoSec.FSINFO_SrchEnt);

	lseek(fd, 1*SECTOR_SIZE, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));
	lseek(fd, 7*SECTOR_SIZE, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));

#if 0

	//TODO write allocInfo file.
	uint64 cur = 0;
	cur = lseek(fd, SECTOR_SIZE*(data_start+SECTORS_PER_CLUSTER*(ALLOC_FILE_CLUS(folderNum) - 2)),SEEK_SET);
	disp(cur);
	write(fd, &folderNum, sizeof(folderNum));
	write(fd, &lastFolderFileNum,sizeof(lastFolderFileNum));
	uint32 fileNumPerFolder = VIDEO_FILE_NUM_PER_PACK;
	write(fd, &fileNumPerFolder, sizeof(fileNumPerFolder));
	
#endif
	disp(ALLOC_FILE_CLUS(folderNum));
	disp(INDEX_FILE_CLUS(folderNum,0,0));
	disp(INDEX_FILE_CLUS(folderNum,0,1));
	disp(INDEX_FILE_CLUS(folderNum,1,0));
	disp(INDEX_FILE_CLUS(folderNum,1,1));
	disp(VIDEO_FILE_CLUS(folderNum,0,0));
	disp(VIDEO_FILE_CLUS(folderNum,0,1));
	disp(VIDEO_FILE_CLUS(folderNum,1,0));
	disp16(VIDEO_FILE_CLUS(folderNum,1,1));


	close(fd);
}

//fuction: allocate clusNum clusters for a file and fill the fat entries correspondly.
//fd: device's file descirptor
//fatStart: sector number of fat start
//entryNum: next cluster number available
//clusNum: number of clusters this file needed
void writeFatEntries(int fd,
		uint64 fatStart, 
		uint32 entryNum, 
		uint32 clusNum)
{
	uint32 content = entryNum + 1;

	lseek(fd, fatStart*SECTOR_SIZE+ entryNum * 4, SEEK_SET);
	for(int i  = 0; i < clusNum - 1; i++){
		write(fd, &content, sizeof(content));
		content ++;
	}
	content = 0x0fffffff;
	write(fd, &content, sizeof(content));
}

//add a fdt to a directory
//fd: file descriptor of the device
//data_start: sectors number of the beginning of the data area
//firstClus: cluster number of the directory
//fdt: fdt address
void addFDT(int fd, 
		uint64 data_start, 
		uint64 firstClus, 
		SHORT_FDT* fdt)
{
	SHORT_FDT tempFDT;
	uint64 offset = 
		(data_start + (firstClus - 2)*SECTORS_PER_CLUSTER )
		* SECTOR_SIZE;
	lseek(fd,offset, SEEK_SET);
	do{
		read(fd, &tempFDT, sizeof(tempFDT));
		offset += sizeof(tempFDT);
	}while(tempFDT.FilName[0]);
	offset -= sizeof(tempFDT);
	lseek(fd, offset, SEEK_SET);
	write(fd, fdt, sizeof(SHORT_FDT));
}



