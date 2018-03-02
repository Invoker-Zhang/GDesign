#include "ourhdr.h"

#define			SECTOR_SIZE		512
#define			FAT_NUMBER		2
#define			RESERVED_SECTORS		32
#define			SECTORS_PER_CLUSTER		8

/*
uint64	sector_size			= 512;
uint64	sectors_per_cluster = SECTORS_PER_CLUSTER;
uint64	total_size;
uint64	total_sectors;
uint64	reserved_start = 0;
uint64	reserved_sectors = RESERVED_SECTORS;
uint64	fat_start;
uint64	fat_sectors;
uint64	data_start;
uint64	data_sectors;
uint64	cluster_number;
*/

void writeFatEntries(int fd,uint64 fatStart, uint32 entryNum, uint32 clusNum);
void addFDT(int fd,uint64 data_start,  uint64 dirClus, SHORT_FDT* fdt);

void format(char* device ){
	int fd;
	uint64 sector_size = SECTOR_SIZE;
	uint64 sectors_per_cluster = SECTORS_PER_CLUSTER;
	uint64 total_size;
	uint64 total_sectors;
	uint64 reserved_sectors = RESERVED_SECTORS;
	uint64 fat_start;
	uint64 fat_sectors;
	uint64 data_start;
	uint64 data_sectors;
	uint64 cluster_number;

	if( (fd = open(device, O_RDWR, FILE_MODE)) < 0)		err_quit("open error");
/*
	if( ioctl(fd, BLKBSZGET, &sector_size) < 0){
		fprintf(stderr, "ioctl failed %s\n", strerror(errno));
		return;
	}
*/
	
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

	char buf[512] = {0};
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < data_start; i++){
		write(fd, buf,sizeof(buf));
	}

	//write DBR sector
	DBR		DBR_sector = {0};
	DBR_sector.DBR_BPB.BPB_BytePerSec = sector_size;
	DBR_sector.DBR_BPB.BPB_SecPerClus = SECTORS_PER_CLUSTER;
	DBR_sector.DBR_BPB.BPB_RsvdSecCnt = RESERVED_SECTORS;
	DBR_sector.DBR_BPB.BPB_NumFATs		= FAT_NUMBER;
	DBR_sector.DBR_BPB.BPB_Media		= 0xf8;
	//DBR_sector.DBR_BPB.BPB_SecPerTrk		= ??
	//DBR_sector.DBR_BPB.BPB_NumHeads		= ??
	//DBR_sector.DBR_BPB.BPB_HidSec			= ??
	DBR_sector.DBR_BPB.BPB_ToSec32		= total_sectors;
	DBR_sector.DBR_BPB.BPB_FATSz32		= fat_sectors;
	//DBR_sector.DBR_BPB.BPB_Flags		= ??
	//DBR_sector.DBR_BPB.BPB_FSVer		= ??
	DBR_sector.DBR_BPB.BPB_RootClus		= 2;
	DBR_sector.DBR_BPB.BPB_FSIfo		= 1;
	DBR_sector.DBR_BPB.BPB_BkBootSec	= 6;
	//TODO
	DBR_sector.DBR_BS.BS_FilSysType[0]  = 'F';
	DBR_sector.DBR_BS.BS_FilSysType[1]	= 'A';
	DBR_sector.DBR_BS.BS_FilSysType[2]	= 'T';
	DBR_sector.DBR_BS.BS_FilSysType[3]	= '3';
	DBR_sector.DBR_BS.BS_FilSysType[4]	= '2';
	DBR_sector.DBR_BootSign[0]			= 0x55;
	DBR_sector.DBR_BootSign[1]			= 0xAa;
	
	lseek(fd, 0, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector) );
	lseek(fd, 6*sector_size, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector));

	// write fs information
	FSINFO	FSINFO_sector = {0};
	FSINFO_sector.FSINFO_Sym			= 0x41615252;
	FSINFO_sector.FSINFO_Used			= 0x61417272;
	FSINFO_sector.FSINFO_LastClus				= cluster_number;
	FSINFO_sector.FSINFO_SrchEnt		= 3;
	FSINFO_sector.FSINFO_EndSign		= 0xaa550000;
	
	if( lseek(fd, 1*sector_size, SEEK_SET) < 0 ) err_sys("lseek error");
	if(write(fd, &FSINFO_sector, sizeof(FSINFO_sector) ) < 0) err_sys("write error");
	lseek(fd, 7*sector_size, SEEK_SET);
	write(fd, &FSINFO_sector, sizeof(FSINFO_sector) );


	//write initial fat entries
	uint32	fat_entry = 0x0ffffff8;
	lseek(fd, fat_start * sector_size + 0 * 4,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );

	char fileName[12] = "video      ";
	time_t t;
	struct tm * curTime = NULL;
	time(&t);
	curTime = gmtime(&t);

	SHORT_FDT rootDir = {0};
	fillFDT(rootDir, fileName, 0x08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, curTime->tm_sec/2 , curTime->tm_min, curTime->tm_hour, curTime->tm_mday, curTime->tm_mon, curTime->tm_year, 0);
	
	lseek(fd, data_start* sector_size, SEEK_SET);
	write(fd, &rootDir, sizeof(rootDir));
	

	close(fd);

	//todel
	disp(fat_start);
	disp(fat_sectors);
	disp(data_start);
	disp(data_sectors);
	disp(cluster_number);
}
//fuction: allocate clusNum clusters for a file and fill the fat entries correspondly.
//fd: device's file descirptor
//fatStart: sector number of fat start
//entryNum: next cluster number available
//clusNum: number of clusters this file needed
void writeFatEntries(int fd,uint64 fatStart, uint32 entryNum, uint32 clusNum){
	uint32 content = entryNum;

	lseek(fd, fatStart*SECTOR_SIZE+ entryNum * 4, SEEK_SET);
	for(int i  = 0; i < clusNum - 1; i++){
		write(fd, &content, sizeof(content));
		content ++;
	}
	content = 0x0fffffff;
	write(fd, &content, sizeof(content));
}

void addFDT(int fd, uint64 data_start, uint64 clusNum, SHORT_FDT* fdt){
	SHORT_FDT tempFDT;
	uint64 offset = (data_start + (clusNum - 2) * SECTORS_PER_CLUSTER ) * SECTOR_SIZE;
	lseek(fd,offset, SEEK_SET);
	do{
		read(fd, &tempFDT, sizeof(tempFDT));
		offset += sizeof(tempFDT);
	}while(tempFDT.FilName[0]);
	offset -= sizeof(tempFDT);
	lseek(fd, offset, SEEK_SET);
	write(fd, fdt, sizeof(SHORT_FDT));
}



