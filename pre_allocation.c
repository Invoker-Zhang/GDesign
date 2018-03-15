#include "ourhdr.h"

static void createFile(int fd,
		uint64_t fatStart, 
		uint64_t fatBakStart,
		uint64_t dataStart,
		uint32_t nextClus, 
		uint32_t fileSize,
		const char* fileName, 
		uint32_t PDClus,
		unsigned char attri);

static void writeFatEntries(int fd,
		uint64_t fatStart, 
		uint32_t entryNum, 
		uint32_t clusNum);

static void addFDT(int fd, 
		uint64_t data_start, 
		uint64_t firstClus, 
		SHORT_FDT* fdt);


/* pre-allocate files in device.
 * it should be formatted before. */

void pre_allocation(char *device){
	int fd;
	uint64_t total_size;
	uint64_t total_sectors;
	uint64_t fat_sectors;
	uint64_t fat_backup_start;
	uint64_t data_start;
	uint64_t data_sectors;
	uint64_t cluster_number;

	if( (fd = open(device, O_RDWR)) < 0)	
		err_quit("open error");

	/* compute basic parameters about the disk */
	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )	
		err_quit("lseek error");

	total_sectors = total_size / SEC_SZ;

	fat_sectors	= (total_sectors - RSVD_SECS)  
		/ (FAT_NUM + SEC_SZ * SECS_PER_CLUS / FAT_ENT_SZ);
	fat_backup_start = FAT_START + fat_sectors;
	data_start	= FAT_START + fat_sectors*FAT_NUM;
	data_sectors = total_sectors - data_start ;
	cluster_number	= data_sectors / SECS_PER_CLUS ;

	if(data_sectors < MIN_DSK_SZ / SEC_SZ)	
		err_quit("fail to pre-allocation, there are not enough space");

	/* This variable indicates number of clusters every folder itself occupied. */
	/* each fdt contains .. and . directory entries. */
	/* Folder itself need more than one cluster if it has too many items. */
	uint32_t dirSize = (2 + INDEXS_PER_PACK + VIDEOS_PER_PACK - 1) / 
		(CLUS_SZ/ sizeof(SHORT_FDT)) + 1;

	disp16(cluster_number);
	disp(fat_backup_start);
	disp(dirSize);

	uint32_t folderNum = 0;
	uint32_t lastFolderFileNum = 0;
	/* compute how many folders and how many video files in last folder */

	if( (cluster_number - ROOT_CLUSTERS - ALLOC_CLUS) % 
			(INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK + 
			 dirSize) == 0)			/* clusters can be divided exactly, each folder has equal video files */
	{
		folderNum = 
			(cluster_number - ROOT_CLUSTERS - ALLOC_CLUS) / 
			(dirSize + 
			 INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK);
		lastFolderFileNum = VIDEOS_PER_PACK;
	}
	else /* there are some orphan clusters */
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
	disp(folderNum);
	disp(lastFolderFileNum);
	
	/* clear sectors used by root directory and subdirectories */

	clearSectors( fd, 
			data_start,
			(ROOT_CLUSTERS +folderNum * dirSize) * SECS_PER_CLUS);

	/* write the root directory's first entry(itself) ie. volumn label */
	SHORT_FDT rootDir = {0};
	fillFDT(rootDir, "VIDEO      ", 0x08, 0,
			0,0,0,0,0,0,
			0,0,0,
			0,
			0,0,0,0,0,0,
			0);
	addFDT(fd, data_start, 2, &rootDir);
	
	uint32_t nextClus			= 3;

	if(folderNum > 100)		
		err_quit("need too many folders,You should enlarge\
			VIDEO_SZ or VIDEOS_PER_PACK");

	SHORT_FDT fdt = {0};
	char name[12] = "FOLDER     ";

	/* allocate fat entries for folders */

	for(int i = 0; i < folderNum; i++){
		writeFatEntries(fd, FAT_START, nextClus, dirSize);
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

		/* add fdt . and .. in subdir */

		strncpy(fdt.FilName, ".           ", 11);
		addFDT(fd, data_start, nextClus, &fdt);
		strncpy(rootDir.FilName, "..         ", 11);
		rootDir.Attri = 0x10;
		addFDT(fd, data_start, nextClus, &rootDir);

		nextClus += dirSize;
	}

	/* allocate fat entries for allocInfo file */

	createFile(fd,
			FAT_START,
			fat_backup_start,
			data_start,
			nextClus,
			CLUS_SZ,
			"ALLOCIFO   ",
			2,
			0x20);
	nextClus += ALLOC_CLUS;

	/* create video files and index files in each folder */
	/* allocate fat entries and fill the items in Parent directory */
	strncpy(name, "INDEX      ",11);
	for(int i = 0; i < folderNum; i++){
		for(int j = 0; j < INDEXS_PER_PACK; j++){
			name[6] = '0' + j % 10;
			name[5] = '0' + j / 10;
			createFile(fd,
					FAT_START,
					fat_backup_start,
					data_start,
					nextClus,
					INDEX_CLUS * CLUS_SZ,
					name,
					i + 1 + 2,
					0x20);
			nextClus += INDEX_CLUS;
		}
	}
	strncpy(name, "CE      MP4", 11);
	for(int i = 0; i < folderNum - 1; i++){
		for(int j = 0; j < VIDEOS_PER_PACK; j++){
			int t = j;
			for(int k = 0; k < 6; k++){
				name[7-k] = '0' + t % 10;
				t = t / 10;
			}
			createFile(fd,
					FAT_START,
					fat_backup_start,
					data_start,
					nextClus,
					VIDEO_CLUS * CLUS_SZ,
					name,
					i + 1 + 2,
					0x20);
			nextClus += VIDEO_CLUS;
		}
	}
	for(int j = 0; j < lastFolderFileNum; j++){
		int t = j;
		for(int k = 0; k < 6; k ++){
			name[7-k] = '0' + t % 10;
			t = t/10;
		}
		createFile(fd,
				FAT_START,
				fat_backup_start,
				data_start,
				nextClus,
				VIDEO_CLUS*CLUS_SZ,
				name,
				folderNum + 2,
				0x20);
		nextClus += VIDEO_CLUS;
	}

	/* update the fsinfo sector */
	FSINFO fsInfoSec;
	lseek(fd, 1 * SEC_SZ, SEEK_SET);
	read(fd, &fsInfoSec, sizeof(fsInfoSec));
	fsInfoSec.FSINFO_LastClus = cluster_number - nextClus;
	fsInfoSec.FSINFO_SrchEnt = nextClus;

	disp16(fsInfoSec.FSINFO_LastClus);
	disp16(fsInfoSec.FSINFO_SrchEnt);

	lseek(fd, 1*SEC_SZ, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));
	lseek(fd, 7*SEC_SZ, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));


	/*  write allocInfo file. */
	uint64_t cur = 0;
	cur = lseek(fd, SEC_SZ*(data_start+SECS_PER_CLUS*(ALLOC_FILE_CLUS(folderNum) - 2)),SEEK_SET);
	disp(cur);
	write(fd, &folderNum, sizeof(folderNum));
	uint32_t fileNumPerFolder = VIDEOS_PER_PACK;
	write(fd, &fileNumPerFolder, sizeof(fileNumPerFolder));
	write(fd, &lastFolderFileNum,sizeof(lastFolderFileNum));
	
	

	close(fd);
}

/* fuction: allocate clusNum clusters for a file and fill the fat entries correspondly. */
/* fd: device's file descirptor */
/* fatStart: sector number of fat start */
/* entryNum: next cluster number available */
/* clusNum: number of clusters this file needed */
static void writeFatEntries(int fd,
		uint64_t fatStart, 
		uint32_t entryNum, 
		uint32_t clusNum)
{
	uint32_t content = entryNum + 1;

	lseek(fd, fatStart*SEC_SZ+ entryNum * FAT_ENT_SZ, SEEK_SET);
	for(int i  = 0; i < clusNum - 1; i++){
		write(fd, &content, sizeof(content));
		content ++;
	}
	content = 0x0fffffff;
	write(fd, &content, sizeof(content));
}

/* add a fdt to a directory */
/* fd: file descriptor of the device */
/* data_start: sectors number of the beginning of the data area */
/* firstClus: cluster number of the directory */
/* fdt: fdt address */
static void addFDT(int fd, 
		uint64_t data_start, 
		uint64_t firstClus, 
		SHORT_FDT* fdt)
{
	SHORT_FDT tempFDT;
	uint64_t offset = 
		(data_start + (firstClus - 2)*SECS_PER_CLUS )
		* SEC_SZ;
	lseek(fd,offset, SEEK_SET);
	do{	/* this while may cause too may read syscall */
		read(fd, &tempFDT, sizeof(tempFDT));
		offset += sizeof(tempFDT);
	}while(tempFDT.FilName[0]);
	offset -= sizeof(tempFDT);
	lseek(fd, offset, SEEK_SET);
	write(fd, fdt, sizeof(SHORT_FDT));
}

/* createFile: create file in a directory.This need two steps.
 * First, allocate fat entries for this file.
 * Second, write file descriptor table.
 *
 * fd:			file descriptor of the device
 * fatStart:	beginning sector of fat
 * fatBakStart: beginning sector of backup fat
 * nextClus:	next cluster available
 * fileSize:	size of file to be allocated
 * fileName:	file name
 * PDClus:		parent directory's cluster number
 * attri:		file attribution */

static void createFile(int fd,
		uint64_t fatStart, 
		uint64_t fatBakStart,
		uint64_t dataStart,
		uint32_t nextClus, 
		uint32_t fileSize,
		const char* fileName, 
		uint32_t PDClus,
		unsigned char attri)
{
	SHORT_FDT fdt = {0};
	time_t t;
	struct tm * curTime = NULL;
	time(&t);
	curTime = gmtime(&t);

	if(attri == 0x20 || attri == 0x10){
		writeFatEntries(fd, fatStart, nextClus, (fileSize - 1) / CLUS_SZ + 1);
		writeFatEntries(fd, fatBakStart, nextClus, (fileSize - 1)/ CLUS_SZ + 1);
		fillFDT(fdt, fileName, attri, 0,
				curTime->tm_sec/2,
				curTime->tm_min,
				curTime->tm_hour,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year - 80, /* tm_year is based on 1900 while year in fdt is based on 1980. */
				nextClus,
				curTime->tm_sec/2,
				curTime->tm_min,
				curTime->tm_hour,
				curTime->tm_mday,
				curTime->tm_mon,
				curTime->tm_year - 80,
				(attri == 0x20) ? fileSize : 0);
		addFDT(fd, dataStart, PDClus, &fdt);
	}
	
}




