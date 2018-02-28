#include "ourhdr.h"

#define			FAT_NUMBER		2
#define			RESERVED_SECTORS		32
#define			SECTORS_PER_CLUSTER		8



unsigned int		sector_size;
unsigned int		sectors_per_cluster = SECTORS_PER_CLUSTER;
unsigned long long	total_size;
unsigned long		total_sectors;
unsigned long		reserved_start = 0;
unsigned long		reserved_sectors = RESERVED_SECTORS;
unsigned long		fat_start;
unsigned long		fat_sectors;
unsigned long		data_start;
unsigned long		data_sectors;

void format(char* device ){
	int fd;

	if( (fd = open(device, O_RDWR, FILE_MODE)) < 0)		err_quit("open error");

	if( ioctl(fd, BLKBSZGET, &sector_size) < 0){
		fprintf(stderr, "ioctl failed %s\n", strerror(errno));
		return;
	}
	
	unsigned long long end;
	if( (end = lseek(fd, 0, SEEK_END)) < 0 )		err_quit("lseek error");
	total_size = end;

	total_sectors = total_size / sector_size;

	fat_start				= reserved_sectors;
	fat_sectors				= 4 * (total_sectors - reserved_sectors) / (4*FAT_NUMBER + sector_size*sectors_per_cluster);
	data_start				= fat_start + fat_sectors*FAT_NUMBER;
	data_sectors			= total_sectors - data_start -1;

	char buf[512] = {0};
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < data_start; i++){
		write(fd, buf,sizeof(buf));
	}

	DBR		DBR_sector = {0};
	DBR_sector.DBR_BPB.BPB_BytePerSec = sector_size;
	DBR_sector.DBR_BPB.BPB_SecPerClus = SECTORS_PER_CLUSTER;
	DBR_sector.DBR_BPB.BPB_RsvdSecCnt = RESERVED_SECTORS;
	DBR_sector.DBR_BPB.BPB_NumFATs		= FAT_NUMBER;
	DBR_sector.DBR_BPB.BPB_Media		= 0xf8;
	DBR_sector.DBR_BPB.BPB_ToSec32		= total_sectors;
	DBR_sector.DBR_BPB.BPB_FATSz32		= fat_sectors;
	DBR_sector.DBR_BPB.BPB_RootClus		= 2;
	DBR_sector.DBR_BPB.BPB_FSIfo		= 1;
	DBR_sector.DBR_BPB.BPB_BkBootSec	= 6;
	DBR_sector.DBR_BS.BS_FilSysType[0]  = 'F';
	DBR_sector.DBR_BS.BS_FilSysType[1]	= 'A';
	DBR_sector.DBR_BS.BS_FilSysType[2]	= 'T';
	DBR_sector.DBR_BS.BS_FilSysType[3]	= '3';
	DBR_sector.DBR_BS.BS_FilSysType[4]	= '2';
	DBR_sector.DBR_BootSign[0]			= 0x55;
	DBR_sector.DBR_BootSign[1]			= 0xAA;
	
	lseek(fd, 0, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector) );
	lseek(fd, 6*sector_size, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector));

}












