#include "ourhdr.h"
#include "fat32.h"

static void createFile(struct filsys_fat32* fs, uint32_t nextClus, 
		uint32_t fileSize,
		const char* fileName, 
		uint32_t PDClus,
		unsigned char attri);

static void writeFatEntries(uint32_t entryNum, uint32_t clusNum);

static void addFDT(struct filsys_fat32* fs, uint64_t firstClus, SHORT_FDT* fdt);

static int		fd;

/* pre-allocate files in device.
 * it should be formatted before. */

void pre_allocation_fat32(struct filsys_fat32* fs_fat){

	printf("pre-allocating...\n");
	if( (fd = open(fs_fat->device, O_RDWR)) < 0)	
		err_quit("open error");

	if(fs_fat->data_secs < MIN_DSK_SZ / SEC_SZ)	
		err_quit("fail to pre-allocation, there are not enough space");

	/* This variable indicates number of clusters every folder itself occupied. */
	/* each fdt contains .. and . directory entries. */
	/* Folder itself need more than one cluster if it has too many items. */
	uint32_t dirSize = (2 + INDEXS_PER_PACK + VIDEOS_PER_PACK - 1) / 
		(CLUS_SZ/ sizeof(SHORT_FDT)) + 1;

	disp16(fs_fat->clus_num);
	disp(dirSize);

	uint32_t folderNum = 0;
	uint32_t lastFolderFileNum = 0;
	/* compute how many folders and how many video files in last folder */

	if( (fs_fat->clus_num - ROOT_CLUSTERS - ALLOC_CLUS) % 
			(INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK + 
			 dirSize) == 0)			/* clusters can be divided exactly, each folder has equal video files */
	{
		folderNum = 
			(fs_fat->clus_num - ROOT_CLUSTERS - ALLOC_CLUS) / 
			(dirSize + 
			 INDEXS_PER_PACK * INDEX_CLUS +
			 VIDEO_CLUS * VIDEOS_PER_PACK);
		lastFolderFileNum = VIDEOS_PER_PACK;
	}
	else /* there are some orphan clusters */
	{
		folderNum = 
			(fs_fat->clus_num - ROOT_CLUSTERS - ALLOC_CLUS) / 
			(dirSize + 
			INDEXS_PER_PACK * INDEX_CLUS + 
			VIDEO_CLUS * VIDEOS_PER_PACK) + 1;
		lastFolderFileNum = 
			(fs_fat->clus_num - ROOT_CLUSTERS - ALLOC_CLUS - 
			folderNum * (dirSize + INDEX_CLUS * INDEXS_PER_PACK)- 
			(folderNum - 1) * VIDEO_CLUS * VIDEOS_PER_PACK ) /
			VIDEO_CLUS ;		
	}
	disp(folderNum);
	disp(lastFolderFileNum);
	
	/* clear sectors used by root directory and subdirectories */

	clearSectors( fd, 
			fs_fat->data_secs,
			(ROOT_CLUSTERS +folderNum * dirSize) * SECS_PER_CLUS);

	/* write the root directory's first entry(itself) ie. volumn label */
	SHORT_FDT rootDir = {0};
	strncpy(rootDir.FilName, "VIDEO      ", 11);
	rootDir.Attri = ATTRI_VOLUME;
	rootDir.LowClus = ROOT_CLUS_NUM;
	addFDT(fs_fat, ROOT_CLUS_NUM, &rootDir);

	uint32_t nextClus			= 3;

	if(folderNum > 100)		
		err_quit("need too many folders,You should enlarge\
			VIDEO_SZ or VIDEOS_PER_PACK");

	SHORT_FDT fdt = {0};
	char name[12] = "FOLDER     ";

	/* allocate fat entries for folders */

	for(int i = 0; i < folderNum; i++){

		name[6] = '0' + i / 10;
		name[7] = '0' + i % 10;

		createFile(fs_fat, nextClus, dirSize*CLUS_SZ, name, ROOT_CLUS_NUM,ATTRI_DIR);
		
		nextClus += dirSize;
	}

	/* allocate fat entries for allocInfo file */

	createFile(fs_fat, nextClus, CLUS_SZ, "ALLOCIFO   ", ROOT_CLUS_NUM, ATTRI_ARCHIVE);
	nextClus += ALLOC_CLUS;

	/* create video files and index files in each folder */
	/* allocate fat entries and fill the items in Parent directory */
	strncpy(name, "INDEX      ",11);
	for(int i = 0; i < folderNum; i++){
		for(int j = 0; j < INDEXS_PER_PACK; j++){
			name[6] = '0' + j % 10;
			name[5] = '0' + j / 10;
			createFile(fs_fat, nextClus,
					INDEX_CLUS * CLUS_SZ,
					name,
					i + 1 + 2,
					ATTRI_ARCHIVE);
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
			createFile(fs_fat, nextClus, VIDEO_CLUS * CLUS_SZ, name, i + 1 + 2, ATTRI_ARCHIVE);
			nextClus += VIDEO_CLUS;
		}
	}
	for(int j = 0; j < lastFolderFileNum; j++){
		int t = j;
		for(int k = 0; k < 6; k ++){
			name[7-k] = '0' + t % 10;
			t = t/10;
		}
		createFile(fs_fat, nextClus, VIDEO_CLUS*CLUS_SZ, name, folderNum + 2, ATTRI_ARCHIVE);
		nextClus += VIDEO_CLUS;
	}

	/* update the fsinfo sector */
	FSINFO fsInfoSec;
	lseek(fd, FS_INFO_SEC * SEC_SZ, SEEK_SET);
	read(fd, &fsInfoSec, sizeof(fsInfoSec));
	fsInfoSec.FSINFO_LastClus = fs_fat->clus_num - nextClus;
	fsInfoSec.FSINFO_SrchEnt = nextClus;

	disp16(fsInfoSec.FSINFO_LastClus);
	disp16(fsInfoSec.FSINFO_SrchEnt);

	lseek(fd, FS_INFO_SEC*SEC_SZ, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));
	lseek(fd, FS_INFO_BK_SEC*SEC_SZ, SEEK_SET);
	write(fd, &fsInfoSec, sizeof(fsInfoSec));


	/*  write allocInfo file. */
	uint64_t cur = 0;
	cur = lseek(fd, SEC_SZ*(fs_fat->data_start+SECS_PER_CLUS*(ALLOC_FILE_CLUS(folderNum) - 2)),SEEK_SET);
	disp(cur);
	write(fd, &folderNum, sizeof(folderNum));
	uint32_t fileNumPerFolder = VIDEOS_PER_PACK;
	write(fd, &fileNumPerFolder, sizeof(fileNumPerFolder));
	write(fd, &lastFolderFileNum,sizeof(lastFolderFileNum));
	
	printf("pre-allocation finished\n");
	

	close(fd);
}

/* fuction: allocate clusNum clusters for a file and fill the fat entries correspondly. */
/* entryNum: next cluster number available */
/* clusNum: number of clusters this file needed */
static void writeFatEntries(uint32_t entryNum, uint32_t clusNum)
{
	uint32_t content = entryNum + 1;

	lseek(fd, FAT_START*SEC_SZ+ entryNum * FAT_ENT_SZ, SEEK_SET);
	for(int i  = 0; i < clusNum - 1; i++){
		write(fd, &content, sizeof(content));
		content ++;
	}
	content = 0x0fffffff;
	write(fd, &content, sizeof(content));
}

/* add a fdt to a directory */
/* firstClus: cluster number of the directory */
/* fdt: fdt address */
static void addFDT(struct filsys_fat32*fs, uint64_t firstClus, SHORT_FDT* fdt)
{
	SHORT_FDT tempFDT;
	uint64_t offset = 
		(fs->data_start + (firstClus - 2)*SECS_PER_CLUS )
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
 * nextClus:	next cluster available
 * fileSize:	size of file to be allocated
 * fileName:	file name
 * PDClus:		parent directory's cluster number
 * attri:		file attribution */

static void createFile(struct filsys_fat32* fs, uint32_t nextClus, 
		uint32_t fileSize,
		const char* fileName, 
		uint32_t PDClus,
		unsigned char attri)
{
	SHORT_FDT fdt = {0};
	time_t t;
	struct tm * curTime = NULL;
	time(&t);
	curTime = localtime(&t);

	writeFatEntries(nextClus, (fileSize - 1) / CLUS_SZ + 1);
	fillFDT(fdt, fileName, attri, 0,
			curTime,
			curTime,
			nextClus,
			curTime,
			attri == ATTRI_ARCHIVE ? fileSize : 0);
	addFDT(fs,PDClus, &fdt);

	if(attri == ATTRI_DIR){
		strncpy(fdt.FilName, ".          ", 11);
		addFDT(fs, nextClus, &fdt);
		memset(&fdt, 0, sizeof(fdt));
		strncpy(fdt.FilName, "..         ", 11);
		fdt.Attri = ATTRI_DIR;
		fdt.LowClus = ROOT_CLUS_NUM;
		addFDT(fs, nextClus, &fdt);
	}
	
}




